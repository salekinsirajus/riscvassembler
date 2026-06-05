CXX = clang++
CFLAGS = -std=c++11
TARGET = riscvass
LEX = flex

LEX_SRC = riscvass.l
LEX_GEN = lex.yy.c

PARSE = bison
PARSE_SRC = riscvass.y
PARSE_GEN = riscvass.tab.c

TESTS_DIR = tests

SRC_DIR = src
BUILD_DIR = build
OBJ_DIR = build/objects

all: $(TARGET)

$(PARSE_GEN): $(PARSE_SRC)
	$(PARSE) -d $(PARSE_SRC)

$(LEX_GEN): $(LEX_SRC)
	$(LEX) $(LEX_SRC)

$(OBJ_DIR)/*.o: $(SRC_DIR)/*.cc
	$(CXX) $(CFLAGS) -c $< -o $@

$(TARGET): $(PARSE_GEN) $(LEX_GEN)
	$(CXX) $(CFLAGS) $(PARSE_GEN) $(LEX_GEN) $(SRC_DIR)/*.cc -o $(TARGET)

clean:
	rm -f $(LEX_GEN) $(TARGET) $(PARSE_GEN) $(ENC_GEN) riscvass.tab.h

cleantest:
	rm -f $(TESTS_DIR)/*.o

rebuild:
	$(MAKE) clean
	$(MAKE) all

.PHONY: all clean cleantest rebuild
