import os

from cs50 import SQL
from flask import Flask, flash, jsonify, redirect, render_template, request, session
from flask_session import Session
from tempfile import mkdtemp
from werkzeug.exceptions import default_exceptions, HTTPException, InternalServerError
from werkzeug.security import check_password_hash, generate_password_hash

from helpers import apology, login_required, lookup, usd, utcnow

# Configure application
app = Flask(__name__)

# Ensure templates are auto-reloaded
app.config["TEMPLATES_AUTO_RELOAD"] = True

# Ensure responses aren't cached
@app.after_request
def after_request(response):
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response

# Custom filter
app.jinja_env.filters["usd"] = usd

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_FILE_DIR"] = mkdtemp()
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")

# Make sure API key is set
if not os.environ.get("API_KEY"):
    raise RuntimeError("API_KEY not set")


@app.route("/")
@login_required
def index():
    """Show portfolio of stocks"""

    # Get portfolio
    # If there was an error searching a symbol render template with message
    try:
        shrs_owned_now, owned_shares_total_price = portfolio()
        cash = user_cash()
        # Format prices
        for i, share in enumerate(shrs_owned_now):
            shrs_owned_now[i]["price"] = usd(share["price"])
            shrs_owned_now[i]["total_price"] = usd(share["total_price"])
    except:
        return render_template("index.html", lookup_error=True), 403

    account_balance = {
        "shares_balance": usd(owned_shares_total_price),
        "cash": usd(cash),
        "grand_totale": usd(cash + owned_shares_total_price),
    }
    
    # Render templates (different if user does not own shares)
    if shrs_owned_now:
        return render_template(
            "index.html", 
            owned_shares=shrs_owned_now,
            balance=account_balance
        )
    else:
        return render_template(
            "index.html",
            no_history=True,
            balance=account_balance
        )


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock"""
    if request.method == "GET":
        return render_template("/buy.html", ask_buy=True)
    
    else:
        return buy_post()


@app.route("/history")
@login_required
def history():
    """Show history of transactions"""

    # Get history information of current user from database
    history = lookup_history()
    
    # Render template (different if user has no history)
    if history:
        return render_template("/history.html", table=True, history=history)
    else:
        return render_template("/history.html")



@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in"""

    # Forget any user_id
    session.clear()

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 403)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 403)

        # Query database for username
        rows = db.execute("SELECT * FROM users WHERE username = :username",
                          username=request.form.get("username"))

        # Ensure username exists and password is correct
        if len(rows) != 1 or not check_password_hash(rows[0]["hash"], request.form.get("password")):
            return apology("invalid username and/or password", 403)

        # Remember which user has logged in
        session["user_id"] = rows[0]["id"]
        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")


@app.route("/logout")
def logout():
    """Log user out"""

    # Forget any user_id
    session.clear()

    # Redirect user to login form
    return redirect("/")


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""
    # If method is GET
    if request.method == "GET":
        return render_template("quote.html", ask_price=True)
    
    # If method is POST
    else:
        return quote_post()


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""
    if request.method == "POST":
        return register_post()
    
    else:
        return render_template("register.html")


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock"""

    # Get portfolio
    # If there was an error searching a symbol render template with message
    try:
        shrs_owned_now, owned_shares_total_price = portfolio()
    except:
        return render_template("sell.html", lookup_error=True), 403

    # Simplify list of owned shares
    for i, shares in enumerate(shrs_owned_now):
        shrs_owned_now[i] = {
            "symbol": shares["symbol"],
            "total_shares": shares["total_shares"],
    }

    # If request method is "GET", render appropiate template
    if request.method == "GET":
        return render_template(
            "/sell.html", ask_sell=True, shares_owned_now=shrs_owned_now 
        )

    # If request method is "POST", render appropiate templates
    else:
        return sell_post(shrs_owned_now)





def errorhandler(e):
    """Handle error"""
    if not isinstance(e, HTTPException):
        e = InternalServerError()
    return apology(e.name, e.code)


# Listen for errors
for code in default_exceptions:
    app.errorhandler(code)(errorhandler)


###############################################################################
################### SOME USEFUL DATABASE-RELATED FUNCTIONS ####################
###############################################################################

# Looks for currently owned shares by user
def portfolio():
    """Returns portfolio of current user
    
    If everything is OK it returns portfolio of current user as two
    variables. If not it returns None:
    
    Returns
    -------
    shrs_owned_now : list
        Contains dictionaries containing the info of each owned share by user.
        Each dictionary contains the following information
        
        {
            "symbol": str,          # Symbol
            "name": str,            # Name of the company
            "price": float,         # Single share price
            "total_shares": int,    # Total number of shares owned by user
            "total_price": float,   # Total current price of owned shares
        }
        
    owned_shares_total_price : float
        Sum of value of all shares owned by user.
    """
    try:
        shrs_symbls_hstry = db.execute(
            """
            SELECT DISTINCT symbol FROM history
            WHERE user_id = :user_id;
            """, user_id=session["user_id"]
        )
        
        # Stores portfolio of stocks. Each element of the list refers to a
        # different symbol and will be a dictionary with relevant info of the 
        # shares owned by user.
        shrs_owned_now = []
        owned_shares_total_price = 0.
            
        for shares in shrs_symbls_hstry:
            
            # Get number of bought and sold 
            symbol = shares["symbol"]

            bought = db.execute(
                """
                SELECT sum(shares) FROM history
                WHERE user_id = :user_id and transaction_type = "BUY" 
                    and symbol = :symbol;
                """, user_id=session["user_id"], symbol=symbol
            )[0]["sum(shares)"]

            sold = db.execute(
                """
                SELECT sum(shares) FROM history
                WHERE user_id = :user_id and transaction_type = "SELL"
                    and symbol = :symbol;
                """, user_id=session["user_id"], symbol=symbol
            )[0]["sum(shares)"]

            # Change null values to 0
            sold = sold if sold else 0
            bought = bought if bought else 0
            
            # Total owned shares current symbol
            total_shares = bought - sold

            # Check if user still owns some of the shares for current symbol
            if total_shares:
                # Lookup current symbol info
                share_info = lookup(symbol)
            else:
                continue

            if not share_info:
                return None

            # Total shares' price for current symbol
            share_info["total_shares"] = total_shares
            total_price = share_info["price"] * total_shares
            share_info["price"] = share_info["price"]
            share_info["total_price"] = total_price
            
            owned_shares_total_price += total_price

            # Add info of shares of current symbol
            shrs_owned_now.append(share_info)

        return shrs_owned_now, owned_shares_total_price 
    
    except:
        return None


def user_cash():
    """Get ammount of cash and grand totale of current user"""
    
    cash = db.execute(
        """
        SELECT * FROM users
        WHERE id = :user_id;
        """, user_id=session["user_id"]
    )[0]["cash"]

    return cash


def db_insert_history_entry(transaction_info):
    """Add transaction info to history table in database

    Parameters
    ----------
    transaction_info : dict
        A dictionary that contains all the information needed to insert
        a row in history table

    Retruns
    -------
    history_entry : cs50.SQL.execute() 
        Output of execute method of cs50.SQL class.
    """
    
    history_entry = db.execute(
        """
        INSERT INTO history 
        (date, user_id, transaction_type, symbol,
        company_name, shares, total_price)
        VALUES(:date, :user_id, :transaction_type, :symbol,
        :company_name, :shares, :total_price);
        """,
        **transaction_info
    )

    return history_entry

def update_cash(new_cash):
    """Updates cash entry for current user

    Parameters
    ----------
    new_cash : float
        New cash value to be stored in database for current user.

    Retruns
    -------
    cash_updated : cs50.SQL.execute() 
        Output of execute method of cs50.SQL class.
    """
    
    cash_updated = db.execute(
        """
        UPDATE users
        SET cash = :new_cash
        WHERE id = :user_id
        """,
        new_cash=new_cash, user_id=session["user_id"]
    )
    return cash_updated

def lookup_history():
    """Looks up transaction history of current user
    
    Retruns
    -------
    history : list
        Each element of the list is a dictionary whose keys are the columns
        of a single history entry. The dicts contain the following information

        {
            "transaction_id": int,      # Transaction id number
            "date": str,                # Format "YYYY-MM-DD HH:MM:SS.mmm"
            "user_id": int,             # User id number
            "transaction_type": str,    # "BUY" or "SELL"
            "symbol": str,              # Stock symbol
            "company_name": str,        # Company Name
            "shares": int,              # Number of shares involved in transaction
            "total_price": float,       # Total price of transaction
        }

    """

    history = db.execute(
        """
        SELECT * FROM history
        WHERE user_id = :user_id
        """, user_id=session["user_id"]
    )

    return history



###############################################################################
##################### SOME USEFUL ROUTE-RELATED FUNCTIONS #####################
###############################################################################


def buy_post():
    """Renders templates of route "/buy" when when ``request.method="POST"``

    Renders appropiate templates according to some mistakes the user may 
    have made during buying process. If no mistakes were made in
    buying process, the purchase is registered in database and success message
    is rendered
    """
    
    # Request symbol info
    symbol_info = lookup(request.form.get("symbol"))
    
    # If symbol does not exist or if error in lookup
    if not symbol_info:
        return render_template("/buy.html", ask_buy=True, no_symb=True), 403
    
    # If symbol exists:
    shares = request.form.get("shares")
    shares = int(shares) if shares else 0
    
    # Get ammount of cash user owns
    cash = user_cash()
    
    # Check if user doesn't have enough cash
    total_price = shares * symbol_info["price"]
    if cash < total_price:
        return render_template("/buy.html", ask_buy=True, no_cash=True), 403
    
    # Check if number of shares is negative
    elif shares <= 0:
        return render_template(
            "/buy.html", ask_buy=True, negative_shares=True
        ), 403
    

    # If all checks passed, register the transaction
    transaction_info = {
        "date": utcnow(),
        "user_id": session["user_id"],
        "transaction_type": "BUY",
        "symbol": symbol_info["symbol"],
        "company_name": symbol_info["name"],
        "shares": shares,
        "total_price": total_price,
    }

    # Register transaction in history
    db_insert_history_entry(transaction_info)
    
    # Register transaction in users (update cash value)
    update_cash(cash - total_price)

    return render_template("/buy.html", trnscn_inf=transaction_info)


def quote_post():
    """Renders templates of route "/quote" when when ``request.method="POST"``

    Renders appropiate templates according to some mistakes the user may 
    have made during searching process. If no mistakes were made, success
    message is rendered and relevant information is displayed
    """

    # Lookup symbol info
    symbol_info = lookup(request.form.get("symbol"))
    
    # If lookup went ok
    if symbol_info:
        symbol_info["price"] = usd(symbol_info["price"])
        return render_template("quote.html", symb_inf=symbol_info)
    
    # If some error in lookup, (probably symbol does not exist)
    else:
        return render_template(
            "quote.html", ask_price=True, no_symb=True
        ), 403


# Defines route "/register" with method="post"
def register_post():
    """Renders templates of route "/register" when ``request.method="POST"``

    Renders appropiate templates according to some mistakes the user may 
    have made during registration process. If no mistakes were made in
    registration, user is registered into the database and the function renders
    a success message. 
    """

    # Preprocess username entry
    username = request.form.get("username")
    if " " in username or not username:
        username = None
    else:
        username_exists = bool(db.execute(
            """SELECT COUNT(username) FROM users
            WHERE username = :username""", username=username
        )[0]['COUNT(username)'])

    # Preprocess password entry
    psswd = request.form.get("password")
    psswd_confirm = request.form.get("confirmation")
    if psswd == psswd_confirm:
        psswd_coincide = True
    else:
        psswd_coincide = False

    # Username empty or contains spaces
    if not username:
        return render_template("register.html", username_status="empty"), 403
    
    # Username already taken
    elif username_exists:
        return render_template(
            "register.html", username_status="not_avaliable"
        ), 403
    
    # Empty password or empty confirm password
    elif not psswd or not psswd_confirm:
        return render_template("register.html", psswd_status="empty"), 403

    # Passwords do not coincide
    elif not psswd_coincide:
        return render_template(
            "register.html", psswd_status="no_coincide"
        ), 403

    else:
        db.execute(
            """INSERT INTO users (username, hash)
            VALUES(:username, :hash)
            """, username=username, hash=generate_password_hash(psswd)
        )
        return render_template("register.html", register_success=True)


def sell_post(shrs_owned_now):
    """Renders templates of route "/sell" when when ``request.method="POST"``

    Renders appropiate templates according to some mistakes the user may 
    have made during selling process. If no mistakes were made in
    selling process, sale is registered in database and success message is
    rendered
    """
    
    # Get form information
    symbol = request.form.get("symbol")
    shares = request.form.get("shares")

    # Update shares value
    shares = int(shares) if shares else 0

    # Check if inputs where ok 
    if not symbol:
        return render_template(
            "/sell.html",
            ask_sell=True,
            no_symb=True,
            shares_owned_now=shrs_owned_now
        ), 403

    elif shares <= 0:
        return render_template(
            "/sell.html", 
            ask_sell=True,
            negative_shares=True,
            shares_owned_now=shrs_owned_now
        ), 403
    
    # Check if user owns sufficient shares of symbol
    sell = False
    for share_owned in shrs_owned_now:
        if (
            share_owned["symbol"] == symbol 
            and share_owned["total_shares"] >= shares
        ):
            sell = True

    if not sell:
        return render_template(
            "/sell.html",
            ask_sell=True,
            not_enough_stocks=True,
            shares_owned_now=shrs_owned_now
        ), 403

    # If all checks passed (and assuming symbol exists), 
    # register the transaction
    symbol_info = lookup(symbol)
    total_price = symbol_info["price"] * shares
    cash = user_cash()

    transaction_info = {
        "date": utcnow(),
        "user_id": session["user_id"],
        "transaction_type": "SELL",
        "symbol": symbol_info["symbol"],
        "company_name": symbol_info["name"],
        "shares": shares,
        "total_price": total_price,
    }

    # Register transaction in history
    db_insert_history_entry(transaction_info)
    
    # Register transaction in users (update cash value)
    update_cash(cash + total_price)

    return render_template("/sell.html", trnscn_inf=transaction_info)
