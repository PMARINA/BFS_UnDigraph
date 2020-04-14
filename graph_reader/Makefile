all:
	make build && make run
build:
	g++ main.cpp -fopenmp -o a.out
run:
	./a.out ../com-orkut.ungraph.txt_beg_pos.bin ../com-orkut.ungraph.txt_csr.bin ../com-orkut.ungraph.txt_weight.bin
clean:
	rm a.out
