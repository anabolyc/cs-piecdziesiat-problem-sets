import cs50

def main():
    print("Number: ", end = "")
    num = cs50.get_int()

    if not isValid(num):
        print("INVALID")
    else:
        if getMajorNums(num, 1) == 4:
            print("VISA")
        else:
            prefix = getMajorNums(num, 2)
            if prefix in [34, 37]:
                print("AMEX")
            elif prefix in [51, 52, 53, 54, 55]:
                print("MASTERCARD")
            else:
                print("INVALID")


def getNumLength(number):
    result = 0
    if number == 0:
        return 1
    while number > 0:
        number //= 10
        result += 1
    return result

def getMajorNums(number, count):
    numsToRemove = getNumLength(number) - count
    for i in range(numsToRemove):
        number //= 10
    return number

def getNumAtPosition(number, pos):
    return getMajorNums(number, getNumLength(number) - pos) % 10


def isValid(number):
    length = getNumLength(number)
    sumOfOdd = 0
    for i in range(1, length, 2):
        x = getNumAtPosition(number, i) * 2
        for j in range(0, getNumLength(x)):
            y = getNumAtPosition(x, j)
            sumOfOdd += y

    sumOfEven = 0
    for i in range(0, length, 2):
        x = getNumAtPosition(number, i)
        sumOfEven += x

    return (sumOfEven + sumOfOdd) % 10 == 0


if __name__ == "__main__":
    main()
