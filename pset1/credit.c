#include <cs50.h>
#include <stdio.h>

bool isValid(long long number);

long long getMajorNums(long long number, int count);

int main(void) {
    printf("Number: ");
    long long num = get_long_long();
    
    if (!isValid(num)) {
        printf("INVALID\n");
    } else  {
        if (getMajorNums(num, 1) == 4) {
            printf("VISA\n");
        } else {
            long long prefix = getMajorNums(num, 2);
            switch(prefix) {
                case 34:
                case 37:
                    printf("AMEX\n");
                    break;
                case 51:
                case 52: 
                case 53: 
                case 54:
                case 55:
                    printf("MASTERCARD\n");
                    break;
                default:
                    printf("INVALID\n");
                    break;
            }
            
        }
    }   
}

int getNumLength(long long number) {
    int result = 0;
    if (number == 0)
        return 1;
    while (number > 0) {
        number /= 10;
        result++;
    }
    return result;
}

long long getMajorNums(long long number, int count) {
    int numsToRemove = getNumLength(number) - count;
    for (int i = 0; i < numsToRemove; i++) {
        number /= 10;
    }
    return number;
}

int getNumAtPosition(long long number, int pos) {
    return getMajorNums(number, getNumLength(number) - pos) % 10;
}

bool isValid(long long number) {
    int length = getNumLength(number);
    int sumOfOdd = 0;
    for (int i = 1; i < length; i += 2) {
        int x = getNumAtPosition(number, i) * 2;
        for (int j = 0; j < getNumLength(x); j++) {
            int y = getNumAtPosition(x, j);
            sumOfOdd += y;    
        }
    }    
    
    int sumOfEven = 0;
    for (int i = 0; i < length; i += 2) {
        int x = getNumAtPosition(number, i);
        sumOfEven += x;
    }    
    
    return (sumOfEven + sumOfOdd) % 10 == 0;
}
