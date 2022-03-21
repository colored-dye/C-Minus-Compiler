CC = gcc
LEX = flex
YACC = bison

CFLAGS = -g

all: cmmc

cmmc: main.c lex.yy.c y.tab.c node.c
	$(CC) -o $@ $^

lex: lexer.l
	$(LEX) $<

yacc: parser.y
	$(YACC) -dy $^

.PHONY: clean
clean:
	rm test y.tab.* lex.yy.c *.o y.output
