while True:
    try:
        h = int(input("Height = "))
    except:
        continue
    if h > 0 and h < 9:
        break

for i in range(h):
    print(" " * (h - (i + 1)) + "#" * (i + 1) + "  " + "#" * (i + 1))