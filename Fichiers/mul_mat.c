#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/time.h>

#define N 1024

#define TIME_DIFF(t1, t2) \
  ((double)((t2.tv_sec - t1.tv_sec) * 1000000 + (t2.tv_usec - t1.tv_usec)))

char a[N][N];
char b[N][N];
char c[N][N];

void mulMat1() {
	int i,j,k;
	bzero(c,N*N);
	for(j=0;j<N;j++) {
		for(i=0;i<N;i++) {
			for(k=0;k<N;k++)
				c[i][j]+=a[i][k]*b[k][j];			
		}
	}
}


void mulMat2() {
	int i,j,k;
	int block_size = 45;
	bzero(c,N*N);
	for(int jj=0;jj<N;jj+=block_size) {
		for(int ii=0;ii<N;ii+=block_size) {
			for(int kk=0;kk<N;kk+=block_size){


				for(i=ii; i<ii+block_size & i<N ;i++){
					for(j=jj; j<jj+block_size & j<N;j++){
						char sum = 0;
						for(k=kk; k<kk+block_size & k<N ; k++ ){
							sum += a[i][k]*b[k][j];
						}
						c[i][j] += sum;

					}
				}
			}
		}
	}
}



// void mulMat2() {
// 	int i,j,k;
// 	bzero(c,N*N);
// 	for(i=0;i<N;i++) {
// 		char* ci = c[i];
// 		for(k=0;k<N;k++) {
// 			char* bk = b[k];
// 			char aik = a[i][k];
// 			for(j=0;j<N;j++)
// 				ci[j]+=aik*bk[j];			
// 		}
// 	}
// }


int main(int argc, char ** argv){
        struct timeval t1,t2,t3;
	int i,j;
	for(i=0;i<N;i++)
		for(j=0;j<N;j++){			
			a[i][j]=rand()%2;
			b[i][j]=rand()%2;
		}

        gettimeofday(&t1,NULL);
	mulMat1();
        gettimeofday(&t2,NULL);
	mulMat2();
        gettimeofday(&t3,NULL);

	printf("%g / %g  = acceleration = %g\n", TIME_DIFF(t1,t2) / 1000,  TIME_DIFF(t2,t3) / 1000, TIME_DIFF(t1,t2)/TIME_DIFF(t2,t3));

	return 0;
}
