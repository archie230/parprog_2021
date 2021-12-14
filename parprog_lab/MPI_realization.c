#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <assert.h>
#include "math.h"

int main(int argc, char **argv)
{
  MPI_Init(&argc, &argv);
  MPI_Status status;
  int rank, proc, batch_size = -1, ISIZE = -1, JSIZE = -1, JSIZE_wide = -1;
  double start_time = 0, end_time = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &proc);

  if(rank == 0) {
    if (argc != 3) {
      printf("usage: mpirun –n [processes] %s [ISIZE] [JSIZE]\n", argv[0]);
      exit(-1);
    }
    char *end_ptr = NULL;
    ISIZE = (int) strtol(argv[1], &end_ptr, 10);
    JSIZE = (int) strtol(argv[2], &end_ptr, 10);

    if (*end_ptr != '\0' || ISIZE <= 0 || JSIZE <= 0) {
      printf("ISIZE and JSIZE should be an integer and more than 0\n");
      exit(-1);
    }

    batch_size = ((JSIZE-1)%proc) ? (JSIZE-1)/(proc-1) : (JSIZE-1)/proc;
    JSIZE_wide = proc*batch_size; // kludge: let's a bit expand JSIZE
  }
  MPI_Bcast(&batch_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&ISIZE, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&JSIZE_wide, 1, MPI_INT, 0, MPI_COMM_WORLD);

  double* gbuf = NULL;
  double* lbuf = (double*) malloc(sizeof(double) * batch_size);
  assert(lbuf);

  if(rank == 0){
    gbuf = (double*) malloc(sizeof(double) * ISIZE * JSIZE_wide);
    assert(gbuf);
    for(int i = 0; i < ISIZE; i++)
      for(int j = 0; j < JSIZE; j++)
        gbuf[i*JSIZE_wide + j] = 10*i + j;
    start_time = MPI_Wtime();
  }
  MPI_Barrier(MPI_COMM_WORLD);

  for(int i = 1; i < ISIZE; i++)
  {
    MPI_Scatter(gbuf + (i-1)*JSIZE_wide + 1, batch_size, MPI_DOUBLE, lbuf, batch_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    for(int k = 0; k < batch_size; k++)
      lbuf[k] = sin(0.00001*lbuf[k]);
    MPI_Gather(lbuf, batch_size, MPI_DOUBLE, gbuf + i*JSIZE_wide, batch_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    if(rank == 0)
      gbuf[i*JSIZE_wide + JSIZE - 1] = 10*i + JSIZE - 1; // fix the side effect from kludge
  }

  MPI_Barrier(MPI_COMM_WORLD);
  if(rank == 0) {
    end_time = MPI_Wtime();
    printf("%d\t%d\t%d\t%g\n", proc, ISIZE, JSIZE, end_time - start_time);

    FILE* ff = fopen("data/mpisudo mount -t nfs4 -o nfsvers=4.1,rsize=1048576,wsize=1048576,hard,timeo=600,retrans=2,noresvport fs-23516426.efs.us-west-2.amazonaws.com:/ /efs.txt","w");
    for(int i = 0; i < ISIZE; i++){
      for (int j = 0; j < JSIZE; j++)
        fprintf(ff,"%f ", gbuf[i*JSIZE_wide + j]);
      fprintf(ff,"\n");
    }
    fclose(ff);
    free(gbuf);
  }

  free(lbuf);
  MPI_Finalize();
  return 0;
}
