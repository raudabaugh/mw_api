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

struct factor {
  char value[200];
};

struct userdef_result_t {
  int sz;
  struct factor factors[200];
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
    mpz_init_set_ui(lb, 1);
    mpz_init_set_ui(one, 1);
    mpz_init_set_ui(processors, sz);

    mpz_init_set_str(number, argv[1], 10);
    mpz_sqrt(sqrt, number);

    work_arr = malloc(sizeof(mw_work_t*)*sz);

    mpz_sub(step, number, one);
    mpz_tdiv_q(step, step, processors);
    mpz_add(ub, lb, step);
    mpz_sub(ub, ub, one);

    int i;
    for (i = 0; i < sz; ++i) {
      mw_work_t *work = malloc(sizeof(mw_work_t));
      strcpy(work->number, mpz_get_str(NULL, 10, number));
      strcpy(work->lb, mpz_get_str(NULL, 10, lb));

      if (i == sz - 1) {
        strcpy(work->ub, mpz_get_str(NULL, 10, number));
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


int process_results(mw_result_t *res) {
  int i;
  for (i = 0; i < res->sz - 1; ++i) {
    /*printf("%s, ", res->factors[i].value);*/
  }
  /*printf("%s\n", res->factors[res->sz - 1].value);*/
  return 1;
}

mw_result_t* do_work(mw_work_t *work) {
  mpz_t number, lb, ub, one, zero, r;
  mpz_init_set_str(number, work->number, 10);
  mpz_init_set_str(lb, work->lb, 10);
  mpz_init_set_str(ub, work->ub, 10);
  mpz_init_set_ui(one, 1);
  mpz_init_set_ui(zero, 0);
  mpz_init(r);

  int sz = 0;
  mw_result_t *result = malloc(sizeof(mw_result_t));

  while (mpz_cmp(lb,ub) <= 0) {
    mpz_add(lb, lb, one);
    mpz_mod(r, number, lb);
    if (mpz_cmp(zero, r) == 0) {
      struct factor *f = malloc(sizeof(struct factor));
      strcpy(result->factors[sz++].value, mpz_get_str(NULL, 10, lb));
    }
  }
  result->sz = sz;

  return result;
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

  int sz, myid;

  MPI_Comm_size(MPI_COMM_WORLD, &sz);

  MPI_Comm_rank(MPI_COMM_WORLD, &myid);

  double t1;
  if (myid == 0) {
    t1 = MPI_Wtime() * 1000;
  }
  int i;
  for (i = 0; i < 20; ++i) {
    MW_Run (argc, argv, &f);
  }
  if (myid == 0) {
    double t2 = MPI_Wtime() * 1000;
    printf("%f ms\n", (t2 - t1)/20);
  }

  MPI_Finalize ();

  return 0;

}
