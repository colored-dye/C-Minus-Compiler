all:
	make -C src #DEBUG=-DSEMANTIC_DEBUG

parser: all
	./cmmc samples/test2.cm > out/out2.txt
	# ./cmmc samples/test2.cm > out/out2.txt
	# ./cmmc samples/test3.cm > out/out3.txt
	# ./cmmc samples/test4.cm > out/out4.txt

run: all
	./cmmc samples/test2.cm

.PHONY: clean
clean:
	make -C src clean