#define _XOPEN_SOURCE

#include <unistd.h>
#include <stdio.h>
#include <cs50.h>
#include <string.h>

#define MAX_PASS_LENGTH 4

const string passwordChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

string bruteForce(string hash, int charsCount);

int main(int argc, string argv[]) {

    if (argc != 2) {
        printf("Usage: ./crack hash\n");
        return 1;
    }

    string hash = argv[1];
    for (int size = 1; size <= MAX_PASS_LENGTH; size++) {
        string outPassword = bruteForce(hash, size);
        if (outPassword != NULL) {
            printf("%s\n", outPassword);
            return 0;
        }
    }
    
    return -1;
}

bool strEqual(unsigned long len, string s1, string s2) {
    for(int i = 0; i < len; i++) {
        if (s1[i] != s2[i])
            return false;
    }
    return true;
}

string bruteForce(string hash, int charsCount) {
    unsigned long len = strlen(hash);
    unsigned long passwordCharsLen = strlen(passwordChars);

    long long optionsCount = 1;
    for (int i = 0; i < charsCount; i++) {
        optionsCount *= passwordCharsLen;
    }

    char *password = malloc((charsCount + 1) * sizeof(char));
    password[charsCount] = '\0';
    
    char salt[3];
    salt[0] = hash[0];
    salt[1] = hash[1];
    salt[2] = '\0';

    for (long long i = 0; i < optionsCount; i++) {
        int index = i;
        for(int j = 0; j < charsCount; j++) {
            password[j] = passwordChars[index % passwordCharsLen];
            index /= passwordCharsLen;
        }
        
        //printf(" -> %s\n", password);
        string thisHash = crypt(password, salt);
        if (strEqual(len, hash, thisHash))
             return password;
    }

    free(password);
    return NULL;
}