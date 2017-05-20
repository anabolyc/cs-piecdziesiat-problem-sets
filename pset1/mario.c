#include <cs50.h>
#include <stdio.h>

int main(void) {
    int height = -1;
    while (height < 0 || height > 23) {
        printf("Height: ");
        height = get_int();
    } 
    
    for (int l = 1; l <= height; l++) {
        for (int i = 0; i < height - l; i++)
            printf(" ");
        for (int i = height - l; i < height; i++)
            printf("#");
        
        printf("  ");
        
        for (int i = height - l; i < height; i++)
            printf("#");
        //for (int i = 0; i < height - l; i++)
        //    printf(" ");
        
        printf("\n");
    }
}