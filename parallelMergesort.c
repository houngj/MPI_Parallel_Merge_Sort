#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

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
    
  int local_buf[NodeNum];
  int temp[NodeNum];
  for(i = 0; i < s; i++){
    local_buf[i] = rand_r(&seed) % 100;
  }
  
  mergeSort(local_buf, temp, s-1); 
  
  
  int From;
  int To;
  int recv_buf[NodeNum];
  int merge_buf[NodeNum];
  int divisor = 2;
  localcount = s;
  int core_difference = 1;
  int localcount2 = s;
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
    //MPI_Barrier(MPI_COMM_WORLD);
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
    printf("Time: %f\n", elapsed);
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
