CC = gcc
LEX = flex
YACC = bison

CFLAGS := -g -lfl
ifneq ($(DEBUG), )
CFLAGS += $(DEBUG)
endif

TARGET = ../cmmc

all: $(TARGET)

$(TARGET): main.c lex.yy.c y.tab.c node.c node.h semantic.c semantic.h symtable.c symtable.h
	$(CC) -o $@ $^ $(CFLAGS)

lex.yy.c: lexer.l
	$(LEX) $<

y.tab.c: parser.y
	$(YACC) -vdy $^

.PHONY: clean
clean:
	rm ../cmmc y.tab.* lex.yy.c y.output
