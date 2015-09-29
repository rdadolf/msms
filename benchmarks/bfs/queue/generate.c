#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

// SKG parameters (range 0->PRNG_RAND_MAX)
#define A UINT64_C(${A})
#define B UINT64_C(${B})
#define C UINT64_C(${C})
#define D UINT64_C(${D})

#include "bfs.h"

#define USELESS_THRESHOLD 10000

int main(int argc, char **argv)
{
  char *outfile;
  struct bench_args_t *data;
  int fd;
  node_index_t *adjmat;
  node_index_t *permute;
  node_index_t r,rp,c,cp,s,temp;
  edge_index_t e;
  int scale;
  long int rint;
  struct prng_rand_t state;
  int useless=0;

  if( argc>1 )
    outfile = argv[1];
  else
    outfile = "input.data";

  // Allocate data structure and temporary adjacency matrix
  data = (struct bench_args_t *)malloc(sizeof(struct bench_args_t));
  adjmat = (node_index_t *)calloc(N_NODES*N_NODES, sizeof(node_index_t));

  // Generate dense matrix
  prng_srand(1, &state);

  // The naive way is to create a normal SKG matrix and then shuffle it to
  // eliminate degree locality. But that takes a while (and lots of memops).
  // An alternate way is to generate a permute schedule a priori, then map
  // the generated edges directly to the shuffled matrix.
  // (For this shuffle, see Knuth v2-3.4.2 alg P.
  //  Ours is sufficiently identical.)
  permute = (node_index_t *)malloc(N_NODES*sizeof(node_index_t));
  for( s=0; s<N_NODES; s++ ) {
    permute[s] = s; // Default is no-swap
  }
  for( s=0; s<N_NODES; s++ ) {
    rint = prng_rand(&state)%N_NODES;
    // Swap row s with row rint
    temp = permute[s];
    permute[s] = permute[rint];
    permute[rint] = temp;
  }

  printf("Generating edges (SKG parameters: %0.2f,%0.2f,%0.2f,%0.2f)...\n",
    ((double)(A))/PRNG_RAND_MAX,
    ((double)(B))/PRNG_RAND_MAX,
    ((double)(C))/PRNG_RAND_MAX,
    ((double)(D))/PRNG_RAND_MAX );
  e = 0;
  // generate N_EDGES/2 undirected edges (N_EDGES directed), but give up after
  // a set number of tries without gainfully producing one. This prevents bad
  // parameter settings (e.g.- n_edges>n_nodes^2) from spinning forever.
  while( e<N_EDGES/2 && useless<USELESS_THRESHOLD ) {
    r = 0;
    c = 0;
    // Pick a random edge according to SKG parameters
    for( scale=SCALE; scale>0; scale-- ) { // each level of the quadtree
      rint = prng_rand(&state); // implicit modulo PRNG_RAND_MAX
      if( rint>=(A+B) ) // C or D (bottom half)
        r += 1<<(scale-1);
      if( (rint>=A && rint<A+B) || (rint>=A+B+C) ) // B or D (right half)
        c += 1<<(scale-1);
    }       
    if( r!=c ) { // ignore self-edges, they're irrelevant
      rp = permute[r];
      cp = permute[c];
      if( adjmat[rp*N_NODES+cp]==0 ) {
        // We make undirected edges
        adjmat[rp*N_NODES+cp]=1;
        adjmat[cp*N_NODES+rp]=1;
        ++e;
        useless = 0;
      }     
    }     
    useless++;
  }
  if( useless>=USELESS_THRESHOLD ) {
    printf("WARNING: stopped generating edges due to fruitless search for unconnected nodes.\n");
    return -1;
  }

  // printf("Creating CSR form...\n");
  // Scan rows for edge list lengths, and fill edges while we're at it
  e = 0;
  for( r=0; r<N_NODES; r++ ) { // count first
    data->nodes[r].edge_begin = 0;
    data->nodes[r].edge_end = 0;
    for( c=0; c<N_NODES; c++ ) {
      if( adjmat[r*N_NODES+c] ) {
        ++data->nodes[r].edge_end;
        data->edges[e].dst = c;
        //data->edges[e].weight = random()%(MAX_WEIGHT-MIN_WEIGHT)+MIN_WEIGHT;
        ++e;
      }
    }
    //if( data->nodes[r].edge_begin==data->nodes[r].edge_end ) {
    //  printf("Isolated node %lu\n", r);
    //}
  }

  for( r=1; r<N_NODES; r++ ) { // now scan
    data->nodes[r].edge_begin = data->nodes[r-1].edge_end;
    data->nodes[r].edge_end += data->nodes[r-1].edge_end;
  }

  // Pick starting node
  do {
    rint = random()%N_NODES;
  } while( (data->nodes[rint].edge_end-data->nodes[rint].edge_begin)<2 );
  data->starting_node = rint;

  // Fill data structure
  memset(data->queue, 0, N_NODES*sizeof(node_index_t));
  memset(data->level, MAX_LEVEL, N_NODES*sizeof(level_t));
  memset(data->level_counts, 0, MAX_LEVEL*sizeof(edge_index_t));

  // Open and write
  fd = open(outfile, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
  assert( fd>0 && "Couldn't open input data file" );
  data_to_input(fd, data);

  return 0;
}
