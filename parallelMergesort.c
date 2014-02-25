#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
  int rank;
  int buf;
  int n;
  MPI_Status status;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if(rank == 0) {
    n = atoi(argv[1]);
    buf = n;
    MPI_Bcast(&buf, 1, MPI_INT, 0, MPI_COMM_WORLD);
  }
  else {
    MPI_Bcast(&buf, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
  }
  printf("rank %d receiving received %d\n", rank, buf);
  MPI_Finalize();
  return 0;
}
