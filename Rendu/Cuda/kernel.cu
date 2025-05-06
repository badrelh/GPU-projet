#include <stdio.h>
#include <cuda_runtime.h>
#include <cmath>
#include <chrono>


//Cuda kernel function to add vector , based on Course CM 
//Global point d'entrée dans GPU a partir de CPU 
__global__ void vec_add(const float *a, const float *b, float *c, int N) {
    int i = threadIdx.x + blockIdx.x * blockDim.x; // i = indice globale du thread , blockIdx: indice de bloc dans la grille , blockDim : nombre de thread dans le bloc et threadIdx 
    if (i < N) {
        c[i] = a[i] + b[i];
    }
}

// Version CPU pour verifier 
void vec_add_cpu(const float *a, const float *b, float *c, int N){
    for(int i=0; i<N ; i++){
        c[i] = a[i] + b[i];
    }
}

// Vérifie que les vecteurs sont identiques
bool verify(const float* ref, const float* res, int N, int epsilon) {
    for (int i = 0; i < N; i++) {
        if (fabs(ref[i] - res[i]) > epsilon) return false;
    }
    return true;
}


int main() {
    int N = 1<<25;  
    size_t size = N * sizeof(float);
    
    // Allocation mémoire sur l'hôte
    float *h_A = (float*)malloc(size);
    float *h_B = (float*)malloc(size);
    float *h_C = (float*)malloc(size);
    float *h_C_ref = (float*)malloc(size);

    // Initialisation des vecteurs avec des valeurs aléatoires
    for (int i = 0; i < N; i++) {
        h_A[i] = static_cast<float>(rand()) / RAND_MAX;
        h_B[i] = static_cast<float>(rand()) / RAND_MAX;
    }

    // CPU reference
    vec_add_cpu(h_A, h_B, h_C_ref, N);

    //Allocation GPU 
    float *d_A, *d_B, *d_C;
    cudaMalloc(&d_A, size);
    cudaMalloc(&d_B, size);
    cudaMalloc(&d_C, size);


     // Chrono CUDA
     cudaEvent_t start, stop;
     cudaEventCreate(&start);
     cudaEventCreate(&stop);


    // Copie des données depuis l'hôte vers le device ( CPU vers GPU )
    cudaEventRecord(start);
    cudaMemcpy(d_A, h_A, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, h_B, size, cudaMemcpyHostToDevice);
    cudaEventRecord(stop);
    cudaEventSynchronize(stop);
    float time_h2d;
    cudaEventElapsedTime(&time_h2d, start, stop);



    // Lancer le kernel avec un nombre de threads par bloc et de blocs
    // Kernel
    int threadsPerBlock = 256;
    int blocksPerGrid = (N + threadsPerBlock - 1) / threadsPerBlock;
    cudaEventRecord(start);
    vec_add<<<blocksPerGrid, threadsPerBlock>>>(d_A, d_B, d_C, N);
    cudaEventRecord(stop);
    cudaEventSynchronize(stop);
    float time_kernel;
    cudaEventElapsedTime(&time_kernel, start, stop);


      // Transfert vers CPU
      cudaEventRecord(start);
      cudaMemcpy(h_C, d_C, size, cudaMemcpyDeviceToHost);
      cudaEventRecord(stop);
      cudaEventSynchronize(stop);
      float time_d2h;
      cudaEventElapsedTime(&time_d2h, start, stop);

          // Verify results
    if (verify(h_C_ref, h_C, N, 1e-5f)) {
        printf("CPU addition verified successfully!\n");
    } else {
        printf("Error in CPU addition!\n");
    }

    // Output the times
    printf("CPU Time: %f seconds\n", time_h2d / 1000);
    printf("GPU Kernel Time: %f seconds\n", time_kernel / 1000);
    printf("Data Transfer Time (D->H): %f seconds\n", time_d2h / 1000);
  
      // Nettoyage
      cudaFree(d_A); cudaFree(d_B); cudaFree(d_C);
      free(h_A); free(h_B); free(h_C); free(h_C_ref);
  

    return 0;
}