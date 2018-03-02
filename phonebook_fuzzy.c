#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "fuzzy.h"
#include "phonebook_fuzzy.h"

/* original version */
entry *findName(char lastName[], entry *pHead)
{
    int best_score = INT_MIN;
    char *maybe = NULL;
    while (pHead != NULL) {
        if (strcasecmp(lastName, pHead->lastName) == 0)
            return pHead;
        int score = fuzzy_match(lastName, pHead->lastName);
        if(score > best_score) {
            best_score = score;
            maybe = pHead->lastName;
        }
        pHead = pHead->pNext;
    }
    // execute here means findName fail.
    printf("No result for %s, Did you mean: %s\n", lastName, maybe);
    return NULL;
}

entry *append(char lastName[], entry *e)
{
    /* allocate memory for the new entry and put lastName */
    e->pNext = (entry *) malloc(sizeof(entry));
    e = e->pNext;
    strcpy(e->lastName, lastName);
    e->pNext = NULL;

    return e;
}
