/* my test program main.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <gmp.h>
#include "mw_api.h"

struct userdef_work_t {
  char lb[200];
  char ub[200];
  char number[200];
};

struct userdef_result_t {
  mpz_t *factors;
};

mw_work_t** create_work(int argc, char **argv) {
  if (argc < 2) {
    return NULL;
  }

  int sz, myid;

  MPI_Comm_size(MPI_COMM_WORLD, &sz);

  MPI_Comm_rank(MPI_COMM_WORLD, &myid);

  mw_work_t** work_arr = NULL;

  if (myid == 0) {
    mpz_t number, sqrt, lb, ub, step, processors, one;
    mpz_init(sqrt);
    mpz_init(step);
    mpz_init(ub);
    mpz_init_set_ui(lb, 2);
    mpz_init_set_ui(one, 1);
    mpz_init_set_ui(processors, sz);

    mpz_init_set_str(number, argv[1], 10);
    mpz_sqrt(sqrt, number);

    work_arr = malloc(sizeof(mw_work_t*)*sz);

    mpz_sub(step, sqrt, one);
    mpz_tdiv_q(step, step, processors);
    mpz_add(ub, lb, step);
    mpz_sub(ub, ub, one);

    int i;
    for (i = 0; i < sz; ++i) {
      mw_work_t *work = malloc(sizeof(mw_work_t));
      strcpy(work->number, mpz_get_str(NULL, 10, number));
      strcpy(work->lb, mpz_get_str(NULL, 10, lb));

      if (i == sz - 1) {
        strcpy(work->ub, mpz_get_str(NULL, 10, sqrt));
      } else {
        strcpy(work->ub, mpz_get_str(NULL, 10, ub));
      }

      work_arr[i] = work;

      mpz_add(lb, lb, step);
      mpz_add(ub, ub, step);
    }
  }

  return work_arr;
}


int process_results(int sz, mw_result_t *res) {
  return 0;
}

mw_result_t* do_work(mw_work_t *work) {
  printf("Received work: %s,%s,%s\n", work->number, work->lb, work->ub);
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
