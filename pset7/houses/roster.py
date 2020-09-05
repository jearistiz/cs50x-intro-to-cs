from sys import argv, exit
import cs50


# Check correct usage
if len(argv) != 2:
    print("Usage: python roaster.py [house]")
    exit(1)

# Open the database
hogwarts_db = cs50.SQL("sqlite:///students.db")

# Ask dbase for relevant info
query = hogwarts_db.execute(
    """SELECT first, middle, last, birth FROM students
    WHERE house = ?
    ORDER BY last ASC, first ASC
    """, argv[1]
)

# Print info
for stdnt in query:
    
    middle = " " + stdnt["middle"] if stdnt["middle"] != None else ""
    
    print(f"{stdnt['first']}{middle} {stdnt['last']}, born {stdnt['birth']}")
