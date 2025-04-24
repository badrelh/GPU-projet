#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

/* macro de mesure de temps, retourne une valeur en �secondes */
#define TIME_DIFF(t1, t2)                                                      \
  ((t2.tv_sec - t1.tv_sec) * 1000000 + (t2.tv_usec - t1.tv_usec))

#define N (10 * 1024 * 1024)
#define TILE 1024

int tab[N];

int main (int argc, char *argv[])
{
  struct timeval t1, t2;
  int sum;
  unsigned long temps;

  srand (1);

  for (int i = 0; i < N; i++)
    tab[i] = rand ();

  sum = 0;
  gettimeofday (&t1, NULL);

  // version s�quentielle
  for (int i = 0; i < N; i++)
    sum += tab[i];

  gettimeofday (&t2, NULL);

  temps = TIME_DIFF (t1, t2);
  printf ("seq\t\t: %ld.%03ldms   sum = %u\n", temps / 1000, temps % 1000, sum);

  ///////////// quatri�me technique : reduction OpenMP
  
  gettimeofday (&t1, NULL);

// TODO
sum = 0;

#pragma omp parallel
{
  #pragma omp single
  {
    #pragma omp taskgroup task_reduction(+:sum)
    {
      for (int i = 0; i < N; i += TILE){
        #pragma omp task in_reduction(+: sum)
          for (int j = 0; j < TILE; j++)
              sum += tab[j + i];    
      }    
    }    
  }

}
  gettimeofday (&t2, NULL);

  temps = TIME_DIFF (t1, t2);
  printf ("reduction\t: %ld.%03ldms   sum = %u\n", temps / 1000, temps % 1000,
          sum);

  ////////// Taskloop ///////////

  gettimeofday (&t1, NULL);

  // TODO
  sum = 0;
  
  #pragma omp parallel
  {
    #pragma omp single
    {
      #pragma omp taskgroup task_reduction(+:sum)
      {
          #pragma omp taskloop grainsize(TILE) in_reduction(+: sum)
          for (int i = 0; i < N; i ++){
                sum += tab[i];    
        }    
      }    
    }
  
  }
    gettimeofday (&t2, NULL);
  
    temps = TIME_DIFF (t1, t2);
    printf ("reduction taskloop \t: %ld.%03ldms   sum = %u\n", temps / 1000, temps % 1000,
            sum);

  return 0;
}
