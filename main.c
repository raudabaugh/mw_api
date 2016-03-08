/* my test program main.c */

#include <stdio.h>
#include <mpi.h>
#include "mw_api.h"

struct userdef_work_t {
  int number;
  int *factors_to_try;
};

struct userdef_result_t {
  int *factors;
};

mw_work_t** create_work(int argc, char **argv) {
  if (argc < 2) {
    return NULL;
  }

  int sz, myid;

  MPI_Comm_size(MPI_COMM_WORLD, &sz);

  MPI_Comm_rank(MPI_COMM_WORLD, &myid);

  if (myid == 0) {
    printf("%s\n", argv[1]);
  }
  return NULL;
}

int process_results(int sz, mw_result_t *res) {
  return 0;
}

mw_result_t* do_work(mw_work_t *work) {
  return NULL;
}

int main (int argc, char **argv)
{
  struct mw_api_spec f;

  MPI_Init (&argc, &argv);

  f.create = create_work;
  f.result = process_results;
  f.compute = do_work;
  f.work_sz = sizeof (struct userdef_work_t);
  f.res_sz = sizeof (struct userdef_result_t);

  MW_Run (argc, argv, &f);

  MPI_Finalize ();

  return 0;

}
