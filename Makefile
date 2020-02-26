COMPILER = jay
CXXFLAGS = -Wall -std=c++17
LDFLAGS = -lfl
DFLAGS := 
CXX = clang++

HEADERS = parser.tab.hh src/include/ast.h src/include/string_builder.h src/include/Driver.h
SOURCES = lex.yy.cc parser.tab.cc src/string_builder.c src/Driver.cpp

all: $(COMPILER)

lex.yy.cc: src/scanner.l
	flex src/scanner.l

parser.tab.hh parser.tab.cc &: src/parser.yy
	bison --debug -d src/parser.yy

$(COMPILER): $(HEADERS) $(SOURCES)
	$(CXX) $(CXXFLAGS) $(DFLAGS) -o $(COMPILER) $(SOURCES) $(LDFLAGS)

debug: DFLAGS=-g
debug: clear
debug: $(COMPILER)

clean:
	rm stack.hh *.tab.cc *.tab.hh *.yy.cc $(COMPILER)

clear: clean
