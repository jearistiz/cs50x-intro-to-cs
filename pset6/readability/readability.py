from cs50 import get_string


def main():

    # Get text from user
    text = get_string("Text: ")

    # Count letters, words and sentences
    l, w, s = count_params(text)

    # Calculate Coleman-Liau index
    L = (l / w) * 100
    S = (s / w) * 100
    CL_index = round(0.0588 * L - 0.296 * S - 15.8)

    #print(f"l: {l}   w: {w}   s: {s}")

    # Print index value
    if CL_index >= 16:
        print("Grade 16+")
    elif CL_index < 1:
        print("Before Grade 1")
    else:
        print(f"Grade {CL_index}")


def count_params(text):
    """
        Counts letters, words and sentences
    """
    letters = 0
    words = 1
    sentences = 0
    for c in text:
        c = c.lower()
        if (c >= 'a' and c <= 'z'):
            letters += 1
        elif (c == ' '):
            words += 1
        elif c == '.' or c == '!' or c == '?':
            sentences += 1

    return letters, words, sentences


main()
