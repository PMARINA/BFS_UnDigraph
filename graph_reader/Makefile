all:
	make build && make run
build:
	g++ main.cpp -fopenmp -o a.out
run:
	./a.out ../duplicated.txt_beg_pos.bin ../duplicated.txt_csr.bin ../duplicated.txt_weight.bin
clean:
	rm a.out
