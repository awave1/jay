COMPILER = jay
TEST_EXEC = jay.test
CXXFLAGS = -Wall -std=c++17
LDFLAGS = -lfl
DFLAGS := 
INCLUDE := -I ./src/include/ -I parser.tab.hh
TESTINCLUDE := -I lib/catch2
CXX = clang++

HEADERS = parser.tab.hh src/include/ast.hpp src/include/string_builder.h src/include/Driver.hpp
SOURCES = lex.yy.cc parser.tab.cc src/string_builder.c src/Driver.cpp

.PHONY: all
all: $(COMPILER)

lex.yy.cc: src/scanner.l
	flex src/scanner.l

parser.tab.hh parser.tab.cc &: src/parser.yy
	bison -t -d src/parser.yy

$(COMPILER): $(SOURCES) $(HEADERS)
	echo $(INCLUDE)
	$(CXX) $(CXXFLAGS) $(DFLAGS) -o $@ ./src/main.cpp $(SOURCES) $(LDFLAGS)

.PHONY: debug
debug: DFLAGS=-g -DYYTRACE
debug: clear
debug: $(COMPILER)

.PHONY: test
test: clean
test: $(TEST_EXEC)

$(TEST_EXEC): test/jay.test.cpp $(HEADERS) $(SOURCES)
	$(CXX) $(TESTINCLUDE) $(DFLAGS) -I src/include test/jay.test.cpp $(SOURCES) -o $@
	./$(TEST_EXEC)

clear clean:
	-rm stack.hh *.tab.cc *.tab.hh *.yy.cc $(COMPILER) $(TEST_EXEC)
