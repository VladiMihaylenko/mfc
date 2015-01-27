//
//  mfc.h
//  mfc
//
//  Created by Vlad Mihaylenko on 27/01/15.
//  Copyright (c) 2015 Vlad Mihaylenko. All rights reserved.
//

#ifndef __mfc__mfc__
#define __mfc__mfc__
#include <stdbool.h>
#include <pthread.h>
#include <assert.h>

typedef unsigned int mutex_t;
typedef unsigned int count_record_count_t;

#define forceinline __inline__ __attribute__((always_inline))

forceinline bool mfc_try_lock(volatile mutex_t* mutex) {
    unsigned int expected = 0;
    if ( __atomic_compare_exchange_n(mutex, &expected, 1, false, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED)) {
        return true;
    }
    return false;
}

forceinline void mfc_unlock(volatile mutex_t* mutex){
    __atomic_store_n(mutex, 0, __ATOMIC_RELEASE);
}

typedef struct CountRecord {
    volatile mutex_t mutex;
    count_record_count_t count;
} CountRecord_t __attribute__ ((aligned (16)));

typedef struct CountCharsArgs {
    char* charSubArray;
    int charSubbArrayLength;
    CountRecord_t *commonCountArray;
} CountCharArgs_t;

char mostFrequentCharacter(char* str, int size);


#endif /* defined(__mfc__mfc__) */
