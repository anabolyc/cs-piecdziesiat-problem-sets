import cs50
import sys
import crypt

MAX_PASS_LENGTH = 4
PASSWORD_CHARS  = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"

def main():
    if len(sys.argv) != 2:
        print("Usage: ./crack hash")
        sys.exit(1)

    hash_string = sys.argv[1]
    for size in range(1, MAX_PASS_LENGTH + 1):
        outPassword = bruteForce(hash_string, size)
        if outPassword != None:
            print(outPassword)
            sys.exit(0)
    sys.exit(-1)


def bruteForce(hash_string, charsCount):
    hash_len = len(hash_string)
    passwordCharsLen = len(PASSWORD_CHARS)

    optionsCount = 1
    for i in range(0, charsCount):
        optionsCount *= passwordCharsLen

    salt = hash_string[0] + hash_string[1]
    
    for i in range(optionsCount):
        index = i
        password = ""
        for j in range(charsCount):
            password += PASSWORD_CHARS[index % passwordCharsLen]
            index //= passwordCharsLen

        #print(" -> {}".format(password))
        if crypt.crypt(password, salt) == hash_string:
             return password

    return None


if __name__ == "__main__":
    main()