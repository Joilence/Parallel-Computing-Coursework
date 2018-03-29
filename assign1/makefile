TARGET := sieve
N := $(n)
P := $(p)

run: build
	mpirun -np $(P) --oversubscribe $(TARGET) $(N);

build:
	mpicc -o $(TARGET) $(TARGET).c

clean:
	@echo "Cleaning..."
	@echo "rm $(TARGET)"; rm $(TARGET)