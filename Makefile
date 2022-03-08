CC = gcc
LEX = flex
YACC = bison

CFLAGS = -g

lex: lexer.l
	$(LEX) $<

yacc: parser.y
	$(YACC) -dy $^

.PHONY: clean
clean:
	rm test y.tab.* lex.yy.c *.o y.output
