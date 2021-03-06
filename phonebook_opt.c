#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "mpool.h"
#include "phonebook_opt.h"

/* original version */
entry *findName(char lastName[], entry *pHead)
{
    while(pHead != NULL) {
        if(strcasecmp(lastName, pHead->lastName) == 0)
            return pHead;
        pHead = pHead->pNext;
    }
    return NULL;
}

entry *append(char lastName[], entry *e)
{

    /* allocate memory for the new entry and put lastName */
    e->pNext = (entry *)pool_alloc(mpool, sizeof(entry));
    e = e->pNext;
    strcpy(e->lastName, lastName);
    e->pNext = NULL;

    return e;
}
