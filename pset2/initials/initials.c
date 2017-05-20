#include <stdio.h>
#include <cs50.h>
#include <string.h>

int main(void) {
    string name = get_string();
    bool firstLetter = true;
    for (int i = 0; i < strlen(name); i++) {
        char current = name[i];
        if (current != ' ') {
            if (firstLetter) {
                if (current > 'Z')
                    current -= 32;
                printf("%c", current);
            }
            firstLetter = false;
        } else {
            firstLetter = true;
        }
    }
    printf("\n");
}