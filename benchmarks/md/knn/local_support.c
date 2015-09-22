#include "md.h"
#include <string.h>

int INPUT_SIZE = sizeof(struct bench_args_t);

#define EPSILON ((TYPE)1.0e-6)

void run_benchmark( void *vargs ) {
  struct bench_args_t *args = (struct bench_args_t *)vargs;
  md_kernel( args->force_x, args->force_y, args->force_z,
             args->position_x, args->position_y, args->position_z,
             args->NL );
}

/* Input format:
%% Section 1
TYPE[n_atoms]: x positions
%% Section 2
TYPE[n_atoms]: y positions
%% Section 3
TYPE[n_atoms]: z positions
%% Section 4
int32_t[n_atoms*max_neighbors]: neighbor list
*/

void input_to_data(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  char *p, *s;
  // Zero-out everything.
  memset(vdata,0,sizeof(struct bench_args_t));
  // Load input string
  p = readfile(fd);

  s = find_section_start(p,1);
  STAC(parse_,TYPE,_array)(s, data->position_x, n_atoms);

  s = find_section_start(p,2);
  STAC(parse_,TYPE,_array)(s, data->position_y, n_atoms);

  s = find_section_start(p,3);
  STAC(parse_,TYPE,_array)(s, data->position_z, n_atoms);

  s = find_section_start(p,4);
  parse_int32_t_array(s, data->NL, n_atoms*max_neighbors);

}

void data_to_input(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->position_x, n_atoms);

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->position_y, n_atoms);

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->position_z, n_atoms);

  write_section_header(fd);
  write_int32_t_array(fd, data->NL, n_atoms*max_neighbors);

}

/* Output format:
%% Section 1
TYPE[n_atoms]: new x force
%% Section 2
TYPE[n_atoms]: new y force
%% Section 3
TYPE[n_atoms]: new z force
*/

void output_to_data(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  char *p, *s;
  // Zero-out everything.
  memset(vdata,0,sizeof(struct bench_args_t));
  // Load input string
  p = readfile(fd);

  s = find_section_start(p,1);
  STAC(parse_,TYPE,_array)(s, data->force_x, n_atoms);

  s = find_section_start(p,2);
  STAC(parse_,TYPE,_array)(s, data->force_y, n_atoms);

  s = find_section_start(p,3);
  STAC(parse_,TYPE,_array)(s, data->force_z, n_atoms);

}

void data_to_output(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->force_x, n_atoms);

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->force_y, n_atoms);

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->force_z, n_atoms);
}

int check_data( void *vdata, void *vref ) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  struct bench_args_t *ref = (struct bench_args_t *)vref;
  int has_errors = 0;
  int i;
  TYPE diff_x, diff_y, diff_z;

  for( i=0; i<n_atoms; i++ ) {
    diff_x = data->force_x[i] - ref->force_x[i];
    diff_y = data->force_y[i] - ref->force_y[i];
    diff_z = data->force_z[i] - ref->force_z[i];
    has_errors |= (diff_x<-EPSILON) || (EPSILON<diff_x);
    has_errors |= (diff_y<-EPSILON) || (EPSILON<diff_y);
    has_errors |= (diff_z<-EPSILON) || (EPSILON<diff_z);
  }

  // Return true if it's correct.
  return !has_errors;
}
