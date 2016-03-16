#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include "mw_api.h"

void MW_Run (int argc, char **argv, struct mw_api_spec *f) {

  int sz, myid;

  MPI_Comm_size(MPI_COMM_WORLD, &sz);

  MPI_Comm_rank(MPI_COMM_WORLD, &myid);

  mw_work_t *work = malloc(f->work_sz);

  if (myid == 0) {
    mw_work_t** work_arr = f->create(argc, argv);
    work = work_arr[0];
    int i;
    for (i = 1; i < sz; ++i) {
      MPI_Send(work_arr[i], f->work_sz, MPI_CHAR, i, 0, MPI_COMM_WORLD);
    }
  } else {
      MPI_Recv(work, f->work_sz, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  f->compute(work);
}
