#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "mpool.h"

#include IMPL

#ifdef OPT

#define OUT_FILE "opt.txt"
#define HASH_TABLE_SIZE 997

#else

#define OUT_FILE "orig.txt"
#define HASH_TABLE_SIZE 1

#endif

#define POOL_SIZE 8400000
#define DICT_FILE "./dictionary/words.txt"

int getHashKey(const char* str)
{
    int length = strlen(str);
    unsigned int seed = 131; /* 31 131 1313 13131 131313 etc.. */
    unsigned int hash = 0;
    unsigned int i = 0;

    for(i = 0; i < length; ++str, ++i) {
        hash = (hash * seed) + (*str);
    }

    return hash % HASH_TABLE_SIZE;
}

static double diff_in_second(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec + diff.tv_nsec / 1000000000.0);
}

int main(int argc, char *argv[])
{
    FILE *fp;
    int i = 0;
    char line[MAX_LAST_NAME_SIZE];
    struct timespec start, end;
    double cpu_time1, cpu_time2;

#ifdef OPT
    mpool = pool_create(POOL_SIZE);
#endif

    /* check file opening */
    fp = fopen(DICT_FILE, "r");
    if (fp == NULL) {
        printf("cannot open the file\n");
        return -1;
    }

    printf("size of entry : %lu bytes\n", sizeof(entry));

    /* build the entry */
    entry *hash_e[HASH_TABLE_SIZE], *hash_pHead[HASH_TABLE_SIZE];
    for(int j = 0; j < HASH_TABLE_SIZE; j++) {
        hash_pHead[j] = (entry *)malloc(sizeof(entry));
        hash_e[j] = hash_pHead[j];
        hash_e[j]->pNext = NULL;
    }

#if defined(__GNUC__)
    for(int j = 0; j < HASH_TABLE_SIZE; j++) {
        __builtin___clear_cache((char *)hash_pHead[j],
                                (char *)hash_pHead[j] + sizeof(entry));
    };
#endif
    clock_gettime(CLOCK_REALTIME, &start);
    while (fgets(line, sizeof(line), fp)) {
        while (line[i] != '\0')
            i++;
        line[i - 1] = '\0';
        i = 0;
        int key = getHashKey(line);
        hash_e[key] = append(line, hash_e[key]);
    }
    clock_gettime(CLOCK_REALTIME, &end);
    cpu_time1 = diff_in_second(start, end);

    /* close file as soon as possible */
    fclose(fp);

//    e = pHead;
    for(int j = 0; j < HASH_TABLE_SIZE; j++) {
        hash_e[j] = hash_pHead[j];
    }

    /* the givn last name to find */
    char input[MAX_LAST_NAME_SIZE] = "zyxel";
    int key = getHashKey(input);

    assert(findName(input, hash_e[key]) &&
           "Did you implement findName() in " IMPL "?");
    assert(0 == strcmp(findName(input, hash_e[key])->lastName, "zyxel"));

#if defined(__GNUC__)
    for(int j = 0; j < HASH_TABLE_SIZE; j++) {
        __builtin___clear_cache((char *)hash_pHead[j],
                                (char *)hash_pHead[j] + sizeof(entry));
    }

#endif
    /* compute the execution time */
    clock_gettime(CLOCK_REALTIME, &start);
    findName(input, hash_e[key]);
    clock_gettime(CLOCK_REALTIME, &end);
    cpu_time2 = diff_in_second(start, end);

    FILE *output = fopen(OUT_FILE, "a");
    fprintf(output, "append() findName() %lf %lf\n", cpu_time1, cpu_time2);
    fclose(output);

    printf("execution time of append() : %lf sec\n", cpu_time1);
    printf("execution time of findName() : %lf sec\n", cpu_time2);


#ifdef OPT
    for(int j=0; j < HASH_TABLE_SIZE; j++) {
        free(hash_pHead[j]);
    }
    pool_destroy(mpool);
#else

    for(int j = 0; j < HASH_TABLE_SIZE; j++) {
        while((hash_e[j] = hash_pHead[j]) != NULL) {
            hash_pHead[j] = hash_e[j]->pNext;
            free(hash_e[j]);
        }

    }

#endif
    return 0;
}
