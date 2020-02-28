COMPILER = jay
TEST_EXEC = jay.test
CXXFLAGS = -Wall -std=c++17
LDFLAGS = -lfl
DFLAGS := 
INCLUDE := -I ./src/include/ -I parser.tab.hh
TESTINCLUDE := -I lib/catch2
CXX = clang++

HEADERS = parser.tab.hh src/include/ast.hpp src/include/string_builder.h src/include/JayCompiler.hpp
SOURCES = lex.yy.cc parser.tab.cc src/string_builder.c src/JayCompiler.cpp

.PHONY: all
all: $(COMPILER)

lex.yy.cc: src/scanner.l
	flex src/scanner.l

parser.tab.hh parser.tab.cc &: src/parser.yy
	bison -t -d src/parser.yy

$(COMPILER): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(DFLAGS) -o $@ ./src/main.cpp $(SOURCES) $(LDFLAGS)

.PHONY: debug
debug: DFLAGS=-g -DYYTRACE
debug: clear
debug: $(COMPILER)

.PHONY: test
test: clean
test: $(COMPILER)
test: $(TEST_EXEC)

$(TEST_EXEC): test/jay.test.cpp $(HEADERS) $(SOURCES)
	$(CXX) -g $(TESTINCLUDE) $(DFLAGS) -I src/include test/jay.test.cpp $(SOURCES) -o $@
	./$(TEST_EXEC)

.PHONY: test_runner
test_runner: clean
test_runner: $(COMPILER)
test_runner:
	@ chmod +x ./test.sh
	./test.sh $(COMPONENT)

.PHONY: export
export: clean
export: ARCHIVE_NAME := artem-golovin-$(MSPART)
export:
	git archive -o $(ARCHIVE_NAME).zip HEAD

clear clean:
	@ echo "> cleaning build & misc files..."
	@ -rm stack.hh *.tab.cc *.tab.hh *.yy.cc $(COMPILER) $(TEST_EXEC) *.totallynotzip 2> /dev/null
	@ echo "> done"
