CUDA_KERNEL = kernel 
SRC = kernel.cu 

CUDA_KERNEL_PLOT = kernel_for_plot
SRC_PLOT = kernel_for_plot.cu 

all: $(CUDA_KERNEL)


$(CUDA_KERNEL): $(SRC)
	nvcc -O3 -o $@ $^ 

$(CUDA_KERNEL_PLOT): $(SRC_PLOT)
	nvcc -O3 -o $@ $^

plot: $(CUDA_KERNEL_PLOT)
	./kernel_for_plot
	python3 plotCuda.py


clean:
	rm -f $(CUDA_KERNEL) $(CUDA_KERNEL_PLOT)