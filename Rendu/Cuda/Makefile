# CUDA_KERNEL = kernel 
# SRC = kernel.cu 

CUDA_KERNEL = kernel_for_plot
SRC = kernel_for_plot.cu 

all: $(CUDA_KERNEL)


$(CUDA_KERNEL): $(SRC)
	nvcc -O3 -o $@ $^ 



clean:
	rm -f $(CUDA_KERNEL)