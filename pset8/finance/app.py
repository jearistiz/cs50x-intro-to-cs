import os

from cs50 import SQL
from flask import Flask, flash, jsonify, redirect, render_template, request, session
from flask_session import Session
from tempfile import mkdtemp
from werkzeug.exceptions import default_exceptions, HTTPException, InternalServerError
from werkzeug.security import check_password_hash, generate_password_hash

from helpers import apology, login_required, lookup, usd

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
    return apology("TODO")


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock"""
    return apology("TODO")


@app.route("/history")
@login_required
def history():
    """Show history of transactions"""
    return apology("TODO")


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
        # Lookup symbol info
        symbol_info = lookup(request.form.get("symbol"))
        
        # If lookup went ok
        if symbol_info:
            symbol_info["price"] = usd(symbol_info["price"])
            return render_template("quote.html", symb_inf=symbol_info)
        
        # If some error in lookup, (probably symbol does not exist)
        else:
            return render_template("quote.html", ask_price=True, no_symb=True)


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""
    if request.method == "POST":
        
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
            return render_template("register.html", username_status="empty")
        
        # Username already taken
        elif username_exists:
            return render_template("register.html", username_status="not_avaliable")
        
        # Empty password or empty confirm password
        elif not psswd or not psswd_confirm:
            return render_template("register.html", psswd_status="empty")

        # Passwords do not coincide
        elif not psswd_coincide:
            return render_template("register.html", psswd_status="no_coincide")

        else:
            db.execute(
                """INSERT INTO users (username, hash)
                VALUES(:username, :hash)
                """, username=username, hash=generate_password_hash(psswd)
            )
            return render_template("register.html", register_success=True)
    
    else:
        return render_template("register.html")


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock"""
    return apology("TODO")


def errorhandler(e):
    """Handle error"""
    if not isinstance(e, HTTPException):
        e = InternalServerError()
    return apology(e.name, e.code)


# Listen for errors
for code in default_exceptions:
    app.errorhandler(code)(errorhandler)
