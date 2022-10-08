/*============================================================================
*
*                    Q u a n t u m  L e a P s
*                    ------------------------
*                    Modern Embedded Software
*
* Copyright (C) 2021 Quantum Leaps, LLC. All rights reserved.
*
* SPDX-License-Identifier: MIT
*
* Contact information:
* <www.state-machine.com>
* <info@state-machine.com>
============================================================================*/
#include <stdint.h>
#include <stdbool.h>

#include "ring_buf.h"
#include "sutest.h"

RingBufElement buf[8];
RingBuf rb;

/* ring-buffer "handler" function for RingBuf_process_all() */
static void rb_handler(RingBufElement const el);

static RingBufElement test_data[] = {
    0xAAU,
    0xBBU,
    0xCCU,
    0xDDU
};
static RingBufCtr test_idx;

void setUp(void) {
}

void tearDown(void) {
}

// Maximum available space in ring buffer. 
#define MAX_FREE (ARRAY_LEN(buf) - 1U)

void TEST_onRun(void) {

    RingBuf_ctor(&rb, buf, ARRAY_LEN(buf));

    TEST("RingBuf_num_free");
    {
        VERIFY(RingBuf_num_free(&rb) == MAX_FREE);
    }

    TEST("RingBuf_put 3");
    {
        VERIFY(true == RingBuf_put(&rb, 0xAAU));
        VERIFY(true == RingBuf_put(&rb, 0xBBU));
        VERIFY(true == RingBuf_put(&rb, 0xCCU));
        VERIFY(RingBuf_num_free(&rb) == MAX_FREE - 3U);
    }

    TEST("RingBuf_get");
    {
        RingBufElement el;
        VERIFY(true == RingBuf_get(&rb, &el));
        VERIFY(0xAAU == el);
        VERIFY(true == RingBuf_get(&rb, &el));
        VERIFY(0xBBU == el);
        VERIFY(true == RingBuf_get(&rb, &el));
        VERIFY(0xCCU == el);
        VERIFY(false == RingBuf_get(&rb, &el));
    }

    TEST("RingBuf_full");
    {
        RingBufElement el;
        for (RingBufCtr i = 0U; i < MAX_FREE; ++i) {
            VERIFY(true == RingBuf_put(&rb, i));
        }
        VERIFY(RingBuf_num_free(&rb) == 0);
        VERIFY(false == RingBuf_put(&rb, 123));
        VERIFY(RingBuf_num_free(&rb) == 0);
        for (RingBufCtr i = 0U; i < MAX_FREE; ++i) {
            VERIFY(true == RingBuf_get(&rb, &el));
            VERIFY(i == el);
        }
    }

    TEST("RingBuf_process_all test_data");
    {
        for (RingBufCtr i = 0U; i < ARRAY_LEN(test_data); ++i) {
            RingBuf_put(&rb, test_data[i]);
        }
        test_idx = 0U;
        RingBuf_process_all(&rb, &rb_handler);
        VERIFY(RingBuf_num_free(&rb) == MAX_FREE);
    }
}

static void rb_handler(RingBufElement const el) {
    VERIFY(test_data[test_idx] == el);
    ++test_idx;
}

