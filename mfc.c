//
//  mfc.c
//  mfc
//
//  Created by Vlad Mihaylenko on 27/01/15.
//  Copyright (c) 2015 Vlad Mihaylenko. All rights reserved.
//

#include "mfc.h"

#define kCharCount 256
#define kThreadCount 2

void* most_freq_char_countChars(void* args) {
    CountCharArgs_t* arg = (CountCharArgs_t*)args;
    CountRecord_t *commonCountRecord = arg->commonCountArray;
    char* charSubArray = arg->charSubArray;
    __builtin_prefetch(charSubArray);
    int charSubArrayLength = arg->charSubbArrayLength;
    for (int i = 0; i < charSubArrayLength; i++) {
        char scannedChar = charSubArray[i];
        CountRecord_t* record = commonCountRecord + scannedChar;
        if (mfc_try_lock(&record->mutex)) {
            record->count++;
            mfc_unlock(&record->mutex);
        }
    }
    return NULL;
}

char mostFrequentCharacter(char* str, int size) {
    assert(size>0);
    if (size == 1)
        return *str;
    
    CountRecord_t countRecords[kCharCount] = {{0, 0}};
    CountCharArgs_t args[kThreadCount];
    for (int i = 0; i < kThreadCount; i++)
        args[i].commonCountArray = countRecords;
    const int blockSize = size / kThreadCount;
    for (int i = 0; i < kThreadCount - 1; i++) {
        char* charPosition = (str + i * blockSize);
        args[i].charSubArray = charPosition;
        args[i].charSubbArrayLength = blockSize;
    }
    
    int lastPartIndex = (kThreadCount - 1) * blockSize;
    args[kThreadCount - 1].charSubArray = str + lastPartIndex;
    args[kThreadCount - 1].charSubbArrayLength = size - lastPartIndex;
    
    pthread_t count_chars_thread[kThreadCount - 1];
    for (int i = 0; i < kThreadCount - 1; i++) {
        pthread_create(&count_chars_thread[i], NULL, most_freq_char_countChars, &args[i]);
    }
    
    most_freq_char_countChars(&args[kThreadCount - 1]);
    
    for (int i = 0; i < kThreadCount; i++)
        pthread_join(count_chars_thread[i], NULL);
    
    char result = 0;
    count_record_count_t max = 0;
    for (int i = 0; i < kCharCount; i++)
        if (countRecords[i].count > max) {
            max = countRecords[i].count;
            result = i;
        }
    
    return result;
}