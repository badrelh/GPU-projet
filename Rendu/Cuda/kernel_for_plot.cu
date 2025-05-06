#include <stdio.h>
#include <cuda_runtime.h>
#include <cmath>
#include <chrono>
#include <cstdlib>

// CUDA kernel function to add vectors
__global__ void vec_add(const float *a, const float *b, float *c, int N) {
    int i = threadIdx.x + blockIdx.x * blockDim.x;  // Global thread index
    if (i < N) {
        c[i] = a[i] + b[i];
    }
}

// CPU version of vector addition for reference
void vec_add_cpu(const float *a, const float *b, float *c, int N) {
    for (int i = 0; i < N; i++) {
        c[i] = a[i] + b[i];
    }
}

// Verifies if two vectors are identical within a given tolerance
bool verify(const float* ref, const float* res, int N, float epsilon = 1e-5f) {
    for (int i = 0; i < N; i++) {
        if (fabs(ref[i] - res[i]) > epsilon) {
            return false; // Not equal
        }
    }
    return true; // Equal
}

int main() {
    // Open file for writing results
    FILE *file = fopen("timing_results.csv", "w");
    if (file == NULL) {
        printf("Error opening file for writing results.\n");
        return -1;
    }
    fprintf(file, "N,CPU_Time,GPU_Time\n");

    // Loop over different values of N
    for (int i = 0; i < 15; i++) {
        int N = 1 << (i + 10);  // Varying N from 2^10 to 2^25
        size_t size = N * sizeof(float);

        // Allocate memory on the host
        float *h_A = (float*)malloc(size);
        float *h_B = (float*)malloc(size);
        float *h_C = (float*)malloc(size);
        float *h_C_ref = (float*)malloc(size);

        // Initialize vectors with random values
        for (int i = 0; i < N; i++) {
            h_A[i] = (float)rand() / RAND_MAX;
            h_B[i] = (float)rand() / RAND_MAX;
        }

        // CPU reference
        clock_t start_cpu, end_cpu;
        double cpu_time_used;
        start_cpu = clock();
        vec_add_cpu(h_A, h_B, h_C_ref, N);
        end_cpu = clock();  // Fin du chronométrage
        cpu_time_used = ((double) (end_cpu - start_cpu)) / CLOCKS_PER_SEC;

        // Allocate GPU memory
        float *d_A, *d_B, *d_C;
        cudaMalloc(&d_A, size);
        cudaMalloc(&d_B, size);
        cudaMalloc(&d_C, size);

        // Chrono CUDA
        cudaEvent_t start, stop;
        cudaEventCreate(&start);
        cudaEventCreate(&stop);

        // Copy data from host to device (CPU -> GPU)
        cudaEventRecord(start);
        cudaMemcpy(d_A, h_A, size, cudaMemcpyHostToDevice);
        cudaMemcpy(d_B, h_B, size, cudaMemcpyHostToDevice);
        cudaEventRecord(stop);
        cudaEventSynchronize(stop);
        float time_h2d;
        cudaEventElapsedTime(&time_h2d, start, stop);

        // Launch the kernel
        int threadsPerBlock = 256;
        int blocksPerGrid = (N + threadsPerBlock - 1) / threadsPerBlock;
        cudaEventRecord(start);
        vec_add<<<blocksPerGrid, threadsPerBlock>>>(d_A, d_B, d_C, N);
        cudaEventRecord(stop);
        cudaEventSynchronize(stop);
        float time_kernel;
        cudaEventElapsedTime(&time_kernel, start, stop);

        // Copy back data to host (GPU -> CPU)
        cudaEventRecord(start);
        cudaMemcpy(h_C, d_C, size, cudaMemcpyDeviceToHost);
        cudaEventRecord(stop);
        cudaEventSynchronize(stop);
        float time_d2h;
        cudaEventElapsedTime(&time_d2h, start, stop);

        // Verify results
        if (verify(h_C_ref, h_C, N, 1e-5f)) {
            printf("CPU addition verified successfully for N = %d!\n", N);
        } else {
            printf("Error in CPU addition for N = %d!\n", N);
        }

        // Output the times to the file
        fprintf(file, "%d,%f,%f\n", N,  cpu_time_used, time_kernel / 1000);

        // Free memory
        cudaFree(d_A);
        cudaFree(d_B);
        cudaFree(d_C);
        free(h_A);
        free(h_B);
        free(h_C);
        free(h_C_ref);
    }

    // Close the file
    fclose(file);
    printf("Timing results have been written to 'timing_results.csv'.\n");

    return 0;
}
