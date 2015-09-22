/*
Implemenataion based on:
A. Danalis, G. Marin, C. McCurdy, J. S. Meredith, P. C. Roth, K. Spafford, V. Tipparaju, and J. S. Vetter.
The scalable heterogeneous computing (shoc) benchmark suite.
In Proceedings of the 3rd Workshop on General-Purpose Computation on Graphics Processing Units, 2010.
*/

#include <stdlib.h>
#include <stdio.h>
#include "support.h"

#define TYPE double

// Problem Constants
#define n_atoms        ${n_atoms}
#define max_neighbors  ${max_neighbors}
// LJ coefficients
#define lj1           1.5
#define lj2           2.0

void md_kernel(TYPE force_x[n_atoms],
               TYPE force_y[n_atoms],
               TYPE force_z[n_atoms],
               TYPE position_x[n_atoms],
               TYPE position_y[n_atoms],
               TYPE position_z[n_atoms],
               int32_t NL[n_atoms*max_neighbors]);
////////////////////////////////////////////////////////////////////////////////
// Test harness interface code.

struct bench_args_t {
  TYPE force_x[n_atoms];
  TYPE force_y[n_atoms];
  TYPE force_z[n_atoms];
  TYPE position_x[n_atoms];
  TYPE position_y[n_atoms];
  TYPE position_z[n_atoms];
  int32_t NL[n_atoms*max_neighbors];
};
