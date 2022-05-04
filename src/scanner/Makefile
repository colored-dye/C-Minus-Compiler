out: scanner

scanner: lex.yy.c ../../include/token.h
	gcc -o $@ $<

lex.yy.c: scanner.l
	flex $<