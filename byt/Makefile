CC = gcc
LEX = flex
YACC = bison

CFLAGS = -g -lfl

all: cmmc

cmmc: main.c lex.yy.c y.tab.c node.c
	$(CC) -o $@ $^ $(CFLAGS)

lex: lexer.l
	$(LEX) $<

yacc: parser.y
	$(YACC) -vdy $^

.PHONY: clean
clean:
	rm cmmc y.tab.* lex.yy.c y.output
