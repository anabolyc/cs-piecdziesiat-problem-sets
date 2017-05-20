import cs50

def main():
    height = -1
    while height < 0 or height > 23:
        print("Height: ", end="")
        height = cs50.get_int()

    for l in range(1, height + 1):
        for i in range(height - l):
            print(" ", end="")
        for i in range(height - l, height):
            print("#", end="")
        
        print("  ", end="")
        
        for i in range(height - l, height):
            print("#", end="")

        print("")


if __name__ == "__main__":
    main()