#include "sort.h"

// Count array:

#define IN_BUF(i) (in_buffer_is_a?a[i]:b[i])
void ss_sort(sort_t a[SIZE], sort_t b[SIZE], count_t counts[RADIX+1] ) {
  uint8_t exp;
  sort_t *in_buffer, *out_buffer, *swap;
  count_t i;
  sort_t r;
  sort_t digit;

  in_buffer = a;
  out_buffer = b;
  sort_loop: for (exp=0; exp<BITWIDTH; exp+=RADIX_BITS) {
    clear_loop: for(r=0; r<RADIX+1; r++) {
      counts[r] = 0;
    }

    count_loop: for(i=0; i<SIZE; ++i) {
      digit = (in_buffer[i]>>exp) & RADIX_MASK;
      ++counts[digit+1];
    }

    scan_loop: for(r=1; r<RADIX; r++) {
      counts[r] += counts[r-1];
    }

    update_loop: for(i=0; i<SIZE; i++) {
      digit = (in_buffer[i]>>exp) & RADIX_MASK;
      out_buffer[counts[digit]] = in_buffer[i];
      ++counts[digit];
    }

    swap = out_buffer;
    out_buffer = in_buffer;
    in_buffer = swap;
  }

  // Result is in ${which_output_buffer}
}
