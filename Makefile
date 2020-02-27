COMPILER = jay
TEST_EXEC = jay.test
CXXFLAGS = -Wall -std=c++17
LDFLAGS = -lfl
DFLAGS := 
TESTLIB := -I lib/catch2
CXX = clang++

HEADERS = parser.tab.hh src/include/ast.h src/include/string_builder.h src/include/Driver.h
SOURCES = lex.yy.cc parser.tab.cc src/string_builder.c src/Driver.cpp

.PHONY: all
all: $(COMPILER)

lex.yy.cc: src/scanner.l
	flex src/scanner.l

parser.tab.hh parser.tab.cc &: src/parser.yy
	bison -t -d src/parser.yy

$(COMPILER): $(HEADERS) $(SOURCES)
	$(CXX) $(CXXFLAGS) $(DFLAGS) -o $(COMPILER) $(SOURCES) $(LDFLAGS)

.PHONY: debug
debug: DFLAGS=-g -DYYTRACE
debug: clear
debug: $(COMPILER)

.PHONY: test
test: $(TEST_EXEC)

$(TEST_EXEC): test/jay.test.cpp
	$(CXX) $(TESTLIB) test/jay.test.cpp -o $@

clean:
	rm stack.hh *.tab.cc *.tab.hh *.yy.cc $(COMPILER)

clear: clean
