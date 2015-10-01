#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

#include "sort.h"

int main(int argc, char **argv)
{
  char *outfile;
  struct bench_args_t data;
  count_t i;
  int fd;
  struct prng_rand_t state;

  if( argc>1 )
    outfile = argv[1];
  else
    outfile = "input.data";

  // Fill data structure
  prng_srand(1,&state);
  for(i=0; i<SIZE; i++)
    data.a[i] = prng_rand(&state) & BIT_MASK;

  // Open and write
  fd = open(outfile, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
  assert( fd>0 && "Couldn't open input data file" );
  data_to_input(fd, (void *)(&data));

  return 0;
}
