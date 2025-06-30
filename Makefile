CXX = clang++
CFLAGS = -g -std=c++11
TARGET = riscvass
LEX = flex

LEX_SRC = riscvass.l
LEX_GEN = lex.yy.c

PARSE = bison
PARSE_SRC = riscvass.y
PARSE_GEN = riscvass.tab.c

ENC_GEN = encoding
ENC_SRC = encoding.cc

LINUX_SRC = linux/elf.cc

all: $(TARGET)

$(PARSE_GEN): $(PARSE_SRC)
	$(PARSE) -d $(PARSE_SRC)

$(LEX_GEN): $(LEX_SRC)
	$(LEX) $(LEX_SRC)

$(ENC_GEN): $(ENC_SRC)
	$(CXX) $(CFLAGS) -c $(ENC_SRC) -o $(ENC_GEN)

$(TARGET): $(PARSE_GEN) $(LEX_GEN) $(LINUX_SRC)
	$(CXX) $(CFLAGS) $(PARSE_GEN) $(LEX_GEN) $(ENC_SRC) $(LINUX_SRC) -o $(TARGET)

clean:
	rm -f $(LEX_GEN) $(TARGET) $(PARSE_GEN) $(ENC_GEN) riscvass.tab.h

rebuild:
	$(MAKE) clean
	$(MAKE) all

.PHONY: all clean rebuild
