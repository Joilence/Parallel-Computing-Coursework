TARGET := sieve

run: build
	mpirun -np 31 --oversubscribe $(TARGET) 1000;

build:
	mpicc -o $(TARGET) $(TARGET).c

clean:
	@echo "Cleaning..."
	@echo "rm $(TARGET)"; rm $(TARGET)