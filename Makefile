all: benchmark_cpu benchmark_memory benchmark_disk benchmark_network_Ser benchmark_network_Cli

benchmark_cpu: benchmark_cpu.o
	cc -o benchmark_cpu benchmark_cpu.c -lpthread

benchmark_memory: benchmark_memory.o
	cc -o benchmark_memory benchmark_memory.c -lpthread

benchmark_disk: benchmark_disk.o
	cc -o benchmark_disk benchmark_disk.c -lpthread

benchmark_network_Ser: benchmark_network_Ser.o
	cc -o benchmark_network_Ser benchmark_network_Ser.c -lpthread

benchmark_network_Cli: benchmark_network_Cli.o
	cc -o benchmark_network_Cli benchmark_network_Cli.c -lpthread

clean:
	rm benchmark_cpu benchmark_cpu.o benchmark_memory benchmark_memory.o benchmark_disk benchmark_disk.o \
	benchmark_network_Ser benchmark_network_Ser.o benchmark_network_Cli benchmark_network_Cli.o