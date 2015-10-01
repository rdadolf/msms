#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "support.h"

// 
#define sort_t ${sort_type} // sizeof(sort_t)*8 should be > RADIX_BITS
#define BIT_MASK ${bit_mask}
#define BITWIDTH ${bitwidth}
#define count_t uint32_t // MAX(count_t) should be > SIZE

#define SIZE (${n_elements})
#define RADIX (${radix})
#define RADIX_BITS (${radix_bits})
#define RADIX_MASK (${radix_mask})


void ss_sort(sort_t a[SIZE], sort_t b[SIZE], count_t counts[RADIX]);

////////////////////////////////////////////////////////////////////////////////
// Test harness interface code.

struct bench_args_t {
  sort_t a[SIZE];
  sort_t b[SIZE];
  count_t counts[RADIX+1];
};
