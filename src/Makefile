CXX = g++
LEX = flex
YACC = bison

CFLAGS := -g
ifneq ($(DEBUG), )
CFLAGS += $(DEBUG)
endif

TARGET = ../cmmc

all: $(TARGET)

# C legacy compile
# $(TARGET): main.c lex.yy.c y.tab.c node.c node.h semantic.c semantic.h symtable.c symtable.h
# 	$(CC) -o $@ $^ $(CFLAGS)

$(TARGET): main.c y.tab.c lex.yy.c node.c node.h semantic.c semantic.h symtable.c symtable.h ast.cpp ast.hpp
	$(CXX) -o $@ $^ $(CFLAGS)

lex.yy.c: lexer.l
	$(LEX) $<

y.tab.c: parser.y
	$(YACC) -vdy $^

.PHONY: clean
clean:
	rm ../cmmc y.tab.* lex.yy.c y.output
