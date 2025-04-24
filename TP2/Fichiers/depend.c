#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define T 10
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
int A[T][T];

int k = 0;

void tache (int i, int j)
{
  volatile int x = random () % 1000000;
  for (int z = 0; z < x; z++)
    ;
#pragma omp atomic capture
  A[i][j] = k++;
}

int main (int argc, char **argv)
{
  int i, j;

  // génération des taches
// #pragma omp parallel
// #pragma omp single
// #pragma omp task depend(out: A[0][0])
//   tache(0, 0);
//   for (j=0; j < T; j++ ){
// #pragma omp task firstprivate(i,j) depend(in: A[0][j-1]) depend(out: A[0][j])
//         tache(0,j);
//   }
//   for (i=1; i < T; i++ ){
//     for (j=0; j < T; j++ ){
//       if( j == 0){
// #pragma omp task firstprivate(i,j) depend(in: A[i-1][j]) depend(out: A[i][j])
//         tache(i, j);
//       }else{
// #pragma omp task firstprivate(i,j) depend(in: A[i-1][j],A[i][j-1])
// depend(out: A[i][j], A[i][j])
//       tache(i,j);
//       }
//   }}

#pragma omp parallel
#pragma omp single
    {
#pragma omp task depend(out : A[0][0])
        tache(0, 0);

        for (int k = 1; k < 2 * T; k++) {
            for (i = 0; i < T; i++) {
                int j = k - i;
                if (j >= 0 && j < T) {
                    if (i == 0) {
#pragma omp task depend(in: A[0][j-1]) depend(out: A[0][j])
                        tache(0, j);
                    } else if (j == 0) {
#pragma omp task depend(in: A[i-1][0]) depend(out: A[i][0])
                        tache(i, 0);
                    } else {
#pragma omp task depend(in: A[i-1][j], A[i][j-1]) depend(out: A[i][j])
                        tache(i, j);
                    }
                }
            }
        }
    }

    
  // affichage du tableau
  for (i = 0; i < T; i++) {
    puts ("");
    for (j = 0; j < T; j++)
      printf (" %2d ", A[i][j]);
    printf ("\n");
  }

  return 0;
}
