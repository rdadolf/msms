#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

#include "md.h"

#define domain_edge ${domain_edge}
#define min_distance ${min_distance}

static inline TYPE dist_sq(TYPE x1, TYPE y1, TYPE z1, TYPE x2, TYPE y2, TYPE z2) {
  TYPE dx, dy, dz;
  dx=x2-x1;
  dy=y2-y1;
  dz=z2-z1;
  return dx*dx + dy*dy + dz*dz;
}

typedef struct {
  int index;
  TYPE dist_sq;
} neighbor_t;

int neighbor_compar(const void *v_lhs, const void *v_rhs) {
  neighbor_t lhs = *((neighbor_t *)v_lhs);
  neighbor_t rhs = *((neighbor_t *)v_rhs);
  return lhs.dist_sq==rhs.dist_sq ? 0 : ( lhs.dist_sq<rhs.dist_sq ? -1 : 1 );
}

int main(int argc, char **argv)
{
  char *outfile;
  struct bench_args_t data;
  int i, j, reject, fd;
  neighbor_t neighbor_list[n_atoms];
  TYPE x, y, z;
  const TYPE infinity = (domain_edge*domain_edge*3.)*1000;//(max length)^2 * 1000
  struct prng_rand_t state;

  if( argc>1 )
    outfile = argv[1];
  else
    outfile = "input.data";

  // Create random positions in the box [0,domain_edge]^3
  prng_srand(1,&state);
  i=0;
  while( i<n_atoms ) {
    // Generate a new point
    x = domain_edge*(((TYPE)prng_rand(&state))/((TYPE)PRNG_RAND_MAX));
    y = domain_edge*(((TYPE)prng_rand(&state))/((TYPE)PRNG_RAND_MAX));
    z = domain_edge*(((TYPE)prng_rand(&state))/((TYPE)PRNG_RAND_MAX));
    // Assure that it's not directly on top of another atom
    reject = 0;
    for( j=0; j<i; j++ ) {
      if( dist_sq(x,y,z, data.position_x[j], data.position_y[j], data.position_z[j])<min_distance ) {
        reject=1;
        break;
      }
    }
    if(!reject) {
      data.position_x[i] = x;
      data.position_y[i] = y;
      data.position_z[i] = z;
      //printf("%lf %lf %lf\n", data.position_x[i], data.position_y[i], data.position_z[i] );
      ++i;
    }
  }

  // Compute k-nearest neighbors
  memset(data.NL, 0, n_atoms*max_neighbors*sizeof(int32_t));
  for( i=0; i<n_atoms; i++ ) {
    for( j=0; j<n_atoms; j++ ) {
      neighbor_list[j].index = j;
      if( i==j )
        neighbor_list[j].dist_sq = infinity;
      else
        neighbor_list[j].dist_sq = dist_sq(data.position_x[i], data.position_y[i], data.position_z[i], data.position_x[j], data.position_y[j], data.position_z[j]);
    }
    qsort(neighbor_list, n_atoms, sizeof(neighbor_t), neighbor_compar);
    //printf("%d:", i);
    for( j=0; j<max_neighbors; j++ ) {
      data.NL[i*max_neighbors +j] = neighbor_list[j].index;
      //printf(" {%d,%lf}", neighbor_list[j].index, neighbor_list[j].dist_sq);
    }
    //printf("\n\n");
  }

  // Open and write
  fd = open(outfile, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
  assert( fd>0 && "Couldn't open input data file" );
  data_to_input(fd, (void *)(&data));

  return 0;
}
