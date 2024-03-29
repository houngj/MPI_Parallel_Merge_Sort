#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
void quickSort( int[], int, int);
int partition( int[], int, int);
void Merge(int[], int[], int[], int, int);
void mergeSort(int[], int[], int);
void m_sort(int[], int[], int, int);
void merge1(int[], int[], int, int, int);

int main(int argc, char** argv) {
  int rank;
  int NodeNum;
  int n;
  int comm_sz;
  double local_start, local_finish, local_elapsed, elapsed;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
  int localcount;
  int s;
  MPI_Barrier(MPI_COMM_WORLD);
  unsigned int seed = rank;
  int z = 0;
  int i = 0;
  int local_buf[atoi(argv[1])];
  while(z != 50){
    local_start = MPI_Wtime();
    
    //process 0 receives n and broadcasts to other processors
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
    
    //variable that keeps track of each processors buffer
    
    
    int temp[NodeNum];
    
    //if(z == 49)
    //  printf("Rank %d has random numbers: ", rank);
    for(i = 0; i < s; i++)
      local_buf[i] = rand_r(&seed) % 100;
    int d = 0;
    if(z == 49){
      if(rank == 0){
	int buffer[s];
	printf("Processor 0 has ");
	for(i = 0; i < s; i++)
	  printf("%d ", local_buf[i]);
	printf("\n");
	for(i = 1; i < comm_sz; i++){
	  printf("Processor %d has ", i); 
	  MPI_Recv(&buffer, s, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	  for(d = 0; d < s; d++)
	    printf("%d ", buffer[d]);
	  printf("\n");
	}
      }
      else{
	MPI_Send(&local_buf, s, MPI_INT, 0, 0, MPI_COMM_WORLD);
      }
    }
    //  if(z == 49)
    //	printf("%d ", local_buf[i]);
    //}
    //if(z == 49)
    //  printf("\n");
    mergeSort(local_buf, temp, s-1); 
    
    
    int From;
    int To;
    int recv_buf[NodeNum];
    int merge_buf[NodeNum];
    int divisor = 2;
    localcount = s;
    int core_difference = 1;
    
    int recvcount;
    int depthlimit = (int) ceil(log2(comm_sz));
    while(depthlimit != 0){
      
      From = rank+core_difference;
      To = rank-core_difference;
      if(rank%divisor == 0){
    	if(rank+core_difference < comm_sz){  
	  //Recv from rank+core_difference
	  MPI_Recv(&recvcount, 1, MPI_INT, From, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	  MPI_Recv(&recv_buf, recvcount, MPI_INT, From, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	  //	  if(z == 49){
	  //  printf("Rank %d's local buffer is: ", rank);
	  // for(i = 0; i < localcount; i++)
	  //  printf("%d ", local_buf[i]);
	  //printf("; Recieves: ");
	  //for(i = 0; i < recvcount; i++)
	  //  printf("%d ", recv_buf[i]);
	  //printf("from Rank %d\n", From);
	  //}
	  //merge
	  Merge(local_buf, recv_buf, merge_buf, localcount, recvcount);
	  for(i = 0; i < (localcount+recvcount); i++){
	    local_buf[i] = merge_buf[i];
	  }
	  localcount = localcount+recvcount;
	  //reassign local_buf 
	}	
      }
      else if(rank%divisor == core_difference){
      //Send to rank-coredifference
	MPI_Send(&localcount, 1, MPI_INT, To, 0, MPI_COMM_WORLD);
	MPI_Send(&local_buf, localcount, MPI_INT, To, 0, MPI_COMM_WORLD);
      }
      
      depthlimit--;
      divisor *=2;
      core_difference *= 2;
    } 
    local_finish = MPI_Wtime();
    z++;
  }
  local_elapsed = local_elapsed+(local_finish - local_start);
  MPI_Reduce(&local_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
  
  if(rank == 0){
    printf("Rank 0 ends up with: ");
    for(i = 0; i < NodeNum; i++)
      printf("%d ", local_buf[i]);
    printf("\nTime: %f\n", elapsed);  

  }
  
  
  MPI_Finalize();
  return 0;
}



void Merge(int a[], int b[], int c[], int localcount, int recvcount){
  int ac = 0;
  int bc = 0;
  int cc = 0;
  int smallest;
  int t = 0;
  while(1){
    if(ac == localcount){
      for(t = bc; t < recvcount; t++)
	c[cc++] = b[t];
      
      break;
    } else if(bc == recvcount){
      for(t = ac; t < localcount; t++)    
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
    if(cc == (recvcount+localcount))
      break;
  }
}



void mergeSort(int numbers[], int temp[], int array_size)
{
  m_sort(numbers, temp, 0, array_size - 1);
}
 
 
void m_sort(int numbers[], int temp[], int left, int right)
{
  int mid;
 
  if (right > left)
    {
      mid = (right + left) / 2;
      m_sort(numbers, temp, left, mid);
      m_sort(numbers, temp, mid+1, right);
 
      merge1(numbers, temp, left, mid+1, right);
    }
}
 
void merge1(int numbers[], int temp[], int left, int mid, int right)
{
  int i, left_end, num_elements, tmp_pos;
 
  left_end = mid - 1;
  tmp_pos = left;
  num_elements = right - left + 1;
 
  while ((left <= left_end) && (mid <= right))
    {
      if (numbers[left] <= numbers[mid])
	{
	  temp[tmp_pos] = numbers[left];
	  tmp_pos = tmp_pos + 1;
	  left = left +1;
	}
      else
	{
	  temp[tmp_pos] = numbers[mid];
	  tmp_pos = tmp_pos + 1;
	  mid = mid + 1;
	}
    }
 
  while (left <= left_end)
    {
      temp[tmp_pos] = numbers[left];
      left = left + 1;
      tmp_pos = tmp_pos + 1;
    }
  while (mid <= right)
    {
      temp[tmp_pos] = numbers[mid];
      mid = mid + 1;
      tmp_pos = tmp_pos + 1;
    }
 
  for (i=0; i <= num_elements; i++)
    {
      numbers[right] = temp[right];
      right = right - 1;
    }
}
