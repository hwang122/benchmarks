#!/bin/sh
make all
:<<!EOF!
mkdir cpu
for opt in flops iops
do
	for thread in 1 2 4
	do
		for (( i = 0; i < 5; i++ ))
		do
			echo "operation type: "$opt", thread number: "$thread
			./benchmark_cpu $opt $thread >>cpu"_"$opt"_"$thread.txt
		done
		mv cpu"_"$opt"_"$thread.txt cpu
	done
done

mkdir memory
for access in seq ran
do
	for blocksize in 1b 1kb 1mb
	do
		for thread in 1 2
		do
			for (( i = 0; i < 5; i++ ))
			do
				echo "access type: "$access", block size:"$blocksize	\
				", thread number: "$thread
				./benchmark_memory $access $blocksize $thread >>	\
				memory"_"$access"_"$blocksize"_"$thread.txt
			done
			mv memory"_"$access"_"$blocksize"_"$thread.txt memory
		done
	done
done
!EOF!
mkdir disk
for opt in Write Read
do
	for access in seq ran
	do
		for blocksize in 1b 1kb 1mb 1gb
		do
			for thread in 1 2
			do
				for (( i = 0; i < 1; i++ ))
				do
					echo "operation type: "$opt", access type: "$access	\
					", block size:"$blocksize", thread number: "$thread
					./benchmark_disk $opt $access $blocksize $thread >>	\
					disk"_"$opt"_"$access"_"$blocksize"_"$thread.txt
				done
				mv disk"_"$opt"_"$access"_"$blocksize"_"$thread.txt disk
			done
		done
	done
done


:<<!EOF!
mkdir network
for connect in tcp udp
do
	for packet in 1b 1kb 64kb
	do
		for thread in 1 2
		do
			for (( i = 0; i < 1; i++ ))
			do
				echo "connect type: "$connect", packet size:"$packet	\
				", thread number: "$thread
				./benchmark_network_Ser $connect $thread &
				./benchmark_network_Cli $connect $packet $thread >>	\
				network"_"$connect"_"$packet"_"$thread.txt
			done
			mv network"_"$connect"_"$packet"_"$thread.txt network
		done
	done
done
!EOF!
make clean