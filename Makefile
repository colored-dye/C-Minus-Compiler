all:
	make -C src

parser:
	./cmmc samples/test1.cm > out/out1.txt
	# ./cmmc samples/test2.cm > out/out2.txt
	# ./cmmc samples/test3.cm > out/out3.txt
	# ./cmmc samples/test4.cm > out/out4.txt

run:
	./cmmc samples/test1.cm

.PHONY: clean
clean:
	make -C src clean