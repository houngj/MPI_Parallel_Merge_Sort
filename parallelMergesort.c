#include <mpi.h>
#include <stdio.h>

#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

void quickSort( int[], int, int);
int partition( int[], int, int);
void Merge(int[], int[], int[], int);
int main(int argc, char** argv) {
  int rank;
  int NodeNum;
  int n;
  int comm_sz;
  
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
  
  int s;
  unsigned int seed = rank;
  int i = 0;
  if(rank == 0) {
    n = atoi(argv[1]);
    NodeNum = n;
    MPI_Bcast(&NodeNum, 1, MPI_INT, 0, MPI_COMM_WORLD);
    s = NodeNum/comm_sz;
    
    
    
  }
  else {
    MPI_Bcast(&NodeNum, 1, MPI_INT, 0, MPI_COMM_WORLD);
    s = NodeNum/comm_sz;
    
  }
  int local_buf[NodeNum];
  for(i = 0; i < s; i++){
    
    local_buf[i] = rand_r(&seed) % 100;
  }
  quickSort(local_buf, 0, s-1);
  
  printf("rank %d received %d with random variables ", rank, NodeNum);
  
  for(i = 0; i < s; i++){
    printf("%d ", local_buf[i]);
  }
  printf("\n");
  //send sorted list to pair processor
  int recv_buf[NodeNum];
  int merge_buf[NodeNum];
  int divisor = 2;
  int core_difference = 1;
  int depthlimit = (int) ceil(log2(comm_sz));
  while(depthlimit != 0){
    
    
    if(rank%divisor == 0){
    	if(rank+core_difference < comm_sz){
	  
	  MPI_Recv(&recv_buf, s*core_difference, MPI_INT, rank+core_difference, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	  Merge(local_buf, recv_buf, merge_buf, s*core_difference);
	  printf("Merge for %d from %d : ", rank, rank+core_difference);
	  for(i = 0; i < s*2*core_difference; i++){
	    local_buf[i] = merge_buf[i];
	    printf("%d ", merge_buf[i]);
	  }
	  printf("\n");
	}
    }
    else if(rank%divisor == core_difference){
      
      MPI_Send(&local_buf, s*core_difference, MPI_INT, rank-core_difference, 0, MPI_COMM_WORLD);
      
    }

    MPI_Barrier(MPI_COMM_WORLD);
    depthlimit--;
    divisor *=2;
    core_difference *= 2;
    
  }
  
  
    
  
    
  
  
  MPI_Finalize();
  return 0;
}

void Merge(int a[], int b[], int c[], int s){
  int ac = 0;
  int bc = 0;
  int cc = 0;
  int smallest;
  int t = 0;
  while(1){
    if(ac >= s){
      for(t = bc; t < s; t++)
	c[cc++] = b[t];
      break;
    } else if(bc >= s){
      for(t = ac; t < s; t++)
	c[cc++] = a[t];
      break;
    }
    if(a[ac] < b[bc]){
      smallest = a[ac];
      ac++;
      
     
      
    } else {
      smallest = b[bc];
      bc++;
      
    }
    
    c[cc++] = smallest;
    
  }
}


void quickSort( int a[], int l, int r)
{
  int j;

  if( l < r ) 
    {
      // divide and conquer
      j = partition( a, l, r);
      quickSort( a, l, j-1);
      quickSort( a, j+1, r);
    }
  
}

int partition( int a[], int l, int r) {
  int pivot, i, j, t;
  pivot = a[l];
  i = l; j = r+1;
  
  while( 1)
    {
      do ++i; while( a[i] <= pivot && i <= r );
      do --j; while( a[j] > pivot );
      if( i >= j ) break;
      t = a[i]; a[i] = a[j]; a[j] = t;
    }
  t = a[l]; a[l] = a[j]; a[j] = t;
  return j;
}
