from cs50 import get_int


def main():

    while True:
        card_number = get_int("Number: ")
        if type(card_number) == int:
            break

    numb_digits = len(str(card_number))
    first_digits = int(str(card_number)[:2])
    master = [51, 52, 53, 54, 55]
    amex = [34, 37]

    if checksum(card_number):

        if numb_digits == 15 and (first_digits in amex):
            print("AMEX")

        elif numb_digits == 16 and (first_digits in master):
            print("MASTERCARD")

        elif (
            (numb_digits == 16 or numb_digits == 13)
            and int(str(first_digits)[0]) == 4
        ):
            print("VISA")

        else:
            print("INVALID")

    else:
        print("INVALID")


def checksum(card_number):

    check_sum = 0

    for i, digit in enumerate(str(card_number)[::-1]):
        digit_2 = int(digit)
        if i % 2:
            digit_2 *= 2
        while digit_2:
            check_sum += digit_2 % 10
            digit_2 = int(digit_2 / 10)

    return not (check_sum % 10)


main()
