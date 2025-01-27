CXX = clang++
CFLAGS = -Wall -g -v
TARGET = riscvass
LEX = flex

LEX_SRC = riscvass.l
LEX_GEN = lex.yy.c

PARSE = bison
PARSE_SRC = riscvass.y
PARSE_GEN = riscvass.tab.c

LINUX_SRC = linux/elf.cc

all: $(TARGET)

$(PARSE_GEN): $(PARSE_SRC)
	$(PARSE) -d $(PARSE_SRC)

$(LEX_GEN): $(LEX_SRC)
	$(LEX) $(LEX_SRC)

$(TARGET): $(PARSE_GEN) $(LEX_GEN) $(LINUX_SRC)
	$(CXX) $(PARSE_GEN) $(LEX_GEN) $(LINUX_SRC) -o $(TARGET)

clean:
	rm -f $(LEX_GEN) $(TARGET) $(PARSE_GEN) riscvass.tab.h

rebuild:
	$(MAKE) clean
	$(MAKE) all

.PHONY: all clean rebuild
