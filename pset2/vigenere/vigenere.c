#include <stdio.h>
#include <cs50.h>
#include <string.h>
#include <ctype.h>

const string alphabet = "abcdefghijklmnopqrstuvwxyz";

bool isAlphaChar(char c);
bool testCode(string code);
void cipher(string text, string k, string out);

int main(int argc, string argv[]) {
    
    if (argc != 2) {
        printf("Usage: ./vigenere k\n");
        return 1;
    }
    
    string k = argv[1];
    if (!testCode(k)) {
        printf("Usage: ./vigenere k\n");
        return 1;
    }
    
    printf("plaintext: ");
    string text = get_string();
    int textLen = strlen(text);
    string encText = malloc((textLen + 1) * sizeof(char));
    cipher(text, k, encText);
    printf("ciphertext: %s\n", encText);
    free(encText);

    return 0;
}

void cipher(string text, string k, string out) {
    unsigned long len = strlen(text);
    unsigned long alen = strlen(alphabet);
    unsigned long klen = strlen(k);
    unsigned long kshift = 0;
    
    for(int i = 0; i < klen; i++){
        k[i] = tolower(k[i]);
    }
    
    for (int i = 0; i < len; i++) {
        char kc = k[(i - kshift) % klen];
        char c = text[i];
        if (isAlphaChar(c)) {
            int shift = kc - 'a';
            char pos = c - 'a' + (c < 'a' ? 32 : 0);
            char x = alphabet[(pos + shift) % alen];
            if (c < 'a')
                x -= 32;
            
            //printf("%c (%i) + %c (%i) -> %c (%i)\n", c, c, kc, kc - 'a', x, x);
            out[i] = x;
        } else {
            out[i] = c;
            kshift++;
        }
    }
}

bool testCode(string k) {
    unsigned long len = strlen(k);
    
    for (int i = 0; i < len; i++) {
        char c = k[i];
        if (!isAlphaChar(c))
            return false;
    }
    return true;
}

bool isAlphaChar(char c) {
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}