from sys import argv, exit
import cs50
import csv


# Check correct usage
if len(argv) != 2:
    print("Usage: python import.py file.csv")
    exit(1)

# Open the database
hogwarts_db = cs50.SQL("sqlite:///students.db")

with open(argv[1], "r") as csv_file:
    students_table = csv.DictReader(csv_file)

    for student in students_table:
        print(student)

        name = student["name"].split(" ")

        if len(name) == 2:
            first = name[0]
            middle = None
            last = name[1]
        else:
            first = name[0]
            middle = name[1]
            last = name[2]
            
        hogwarts_db.execute(
            "INSERT INTO students (first, middle, last, house, birth) "
            + "VALUES(?, ?, ?, ?, ?)",
            first, middle, last, student["house"], student["birth"]
        )
