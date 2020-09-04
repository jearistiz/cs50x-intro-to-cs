from sys import argv, exit


def main():

    if len(argv) != 3:
        print("Usage: python dna.py [database.csv] [sequence.txt]")
        exit(1)

    database_name = argv[1]
    sequence_name = argv[2]

    # Dictionary to store database of names vs STR ocurrences
    database = {}

    # Open datebase files and update database dictionary
    with open(database_name, 'r') as db:
        for i, line in enumerate(db):
            if i == 0:
                line = line.strip('\n').split(",")
                STRs = line[1:]
            else:
                line = line.strip('\n').split(",")
                database[line[0]] = [int(n) for n in line[1:]]

    # Stores maximum continuous counts (in DNA sequence) of each STR
    STRs_max_counts = []

    # Open sequence file and store the DNA sequence in variable seq
    with open(sequence_name, 'r') as sequence:
        for seq in sequence:
            seq = seq.strip('\n')

    # For each STR repeat the searching process
    for STR in STRs:
        # Stores maximum count of continuous STR appearence
        STR_max_counts = 0

        # Tracks the searching position
        position = 0

        len_STR = len(STR)

        # Look for repeating STR in all positions
        while position < len(seq):
            STR_counts = 0

            # Look for continuous STR appearances
            while seq[position:position + len_STR] == STR:
                STR_counts += 1
                position += len_STR

            # If no STR found in current position go to next position
            if not STR_counts:
                position += 1

            # If continuous STR found, go to next position of initial STR
            else:
                position -= STR_counts * len_STR - 1

                # Update maximum STR count for current STR
                if STR_counts > STR_max_counts:
                    STR_max_counts = STR_counts

        STRs_max_counts.append(STR_max_counts)

    # Set a "found" label
    found = False

    # Look if DNA profile coincides with someone in database
    for name in database:
        if database[name] == STRs_max_counts:
            print(name)
            found = True
            break

    if not found:
        print("No match")

    exit(0)


main()
