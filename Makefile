COMPILER = jay
C_FLAGS = -Wall
LDFLAGS = -lfl
CXX = clang++
DEBUGFLAGS = -DYYDEBUG

HEADERS = parser.tab.hh src/include/ast.h src/include/string_builder.h src/include/Driver.h
SOURCES = lex.yy.cc parser.tab.cc src/string_builder.c src/Driver.cpp

all: $(COMPILER)

lex.yy.cc: src/scanner.l
	flex src/scanner.l

parser.tab.hh parser.tab.cc &: src/parser.yy
	bison -d src/parser.yy

$(COMPILER): $(HEADERS) $(SOURCES)
	$(CXX) $(CFLAGS) -o $(COMPILER) $(SOURCES) $(LDFLAGS) $(DEBUGFLAGS)

clean:
	rm stack.hh *.tab.cc *.tab.hh *.yy.cc $(COMPILER)
