CXX = clang++
CFLAGS = -Wall -g
TARGET = riscvass
LEX = flex

LEX_SRC = riscvass.l
LEX_GEN = lex.yy.c

PARSE = bison
PARSE_SRC = riscvass.y
PARSE_GEN = riscvass.tab.c

all: $(TARGET)

$(PARSE_GEN): $(PARSE_SRC)
	$(PARSE) -d $(PARSE_SRC)

$(LEX_GEN): $(LEX_SRC)
	$(LEX) $(LEX_SRC)

$(TARGET): $(PARSE_GEN) $(LEX_GEN)
	$(CXX) $(PARSE_GEN) $(LEX_GEN) -o $(TARGET)

clean:
	rm -f $(LEX_GEN) $(TARGET) $(PARSE_GEN) riscvass.tab.h

rebuild:
	$(MAKE) clean
	$(MAKE) all

.PHONY: all clean rebuild
