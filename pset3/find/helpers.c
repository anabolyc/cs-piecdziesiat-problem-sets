/**
 * helpers.c
 *
 * Helper functions for Problem Set 3.
 */
 
#include <cs50.h>
#include <stdio.h>
#include "helpers.h"

const int MAX_N = 65536;

/**
 * Returns true if value is in subarray [start, end] of array of n values, else false.
 */
bool search_sub(int value, int values[], int start, int end)
{
    if (start >= end)
        return values[start] == value;
        
    int mid = (end + start) / 2;
    int midv = values[mid];
    //printf("start: %i, end: %i, mid: %i | find: %i, midv: %i, \n", start, end, mid, value, midv);
    
    if (value < midv)
        return search_sub(value, values, start, mid - 1);
    else if (value > midv)
        return search_sub(value, values, mid + 1, end);
    else
        return true;
}

/**
 * Returns true if value is in array of n values, else false.
 */
bool search(int value, int values[], int n)
{
    if (value <= 0)
        return false;
    else
        return search_sub(value, values, 0, n);
}

/**
 * Sorts array of n values.
 */
void sort(int values[], int n)
{
    int stats[MAX_N];
    for (int i = 0; i < MAX_N; i++)
        stats[i] = 0;

    for (int i = 0; i < n; i++)
        stats[values[i]] += 1;
        
    int p = 0;
    for (int i = 0; i < MAX_N; i++)
        for (int j = 0; j < stats[i]; j++, p++)
            values[p] = i;

    return;
}

/**
 * Prints array (for debugging only)
 */
void print(int values[], int n)
{
    printf(" >>> \n");
    for (int i = 0; i < n; i++)
        printf("%i\n", values[i]);
    printf(" <<< \n");
}