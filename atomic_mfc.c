//
//  mfc.c
//  mfc
//
//  Created by Vlad Mihaylenko on 27/01/15.
//  Copyright (c) 2015 Vlad Mihaylenko. All rights reserved.
//

#include "atomic_mfc.h"
#include <stdbool.h>
#include <pthread.h>
#include <assert.h>

#define kCharCount 256
#define kThreadCount 2

typedef unsigned int count_record_count_t;

typedef struct CountCharsArgs {
    char* charSubArray;
    int charSubbArrayLength;
    count_record_count_t* commonCountArray;
} CountCharArgs_t __attribute__ ((aligned(16)));

void* most_freq_char_countChars(void* args) {
    CountCharArgs_t* arg = (CountCharArgs_t*)args;
    count_record_count_t* commonCountRecord = arg->commonCountArray;
    char* charSubArray = arg->charSubArray;
    __builtin_prefetch(charSubArray);
    int charSubArrayLength = arg->charSubbArrayLength;
    for (int i = 0; i < charSubArrayLength; i++) {
        char scannedChar = charSubArray[i];
        count_record_count_t* record = commonCountRecord + scannedChar;
        __atomic_add_fetch(record, 1, __ATOMIC_ACQUIRE);
    }
    return NULL;
}

char mostFrequentCharacter(char* str, int size) {
    assert(size>0);
    if (size == 1)
        return *str;
    
    count_record_count_t countRecords[kCharCount] = {0};
    CountCharArgs_t args[kThreadCount];
    const int blockSize = size / kThreadCount;
    char* charPosition = str;
    args[0].commonCountArray = countRecords;
    args[0].charSubArray = charPosition;
    args[0].charSubbArrayLength = blockSize;
    
    int lastPartIndex = blockSize;
    args[1].commonCountArray = countRecords;
    args[1].charSubArray = str + lastPartIndex;
    args[1].charSubbArrayLength = size - lastPartIndex;
    
    pthread_t count_chars_thread;
    
    pthread_create(&count_chars_thread, NULL, most_freq_char_countChars, &args[1]);
    
    most_freq_char_countChars(&args[0]);
    pthread_join(count_chars_thread, NULL);
    
    char result = 0;
    count_record_count_t max = 0;
    for (int i = 0; i < kCharCount; i++)
        if (countRecords[i] > max) {
            max = countRecords[i];
            result = i;
        }
    
    return result;
}