COMPILER_NAME = jay
C_FLAGS = -Wall

HEADERS = parser.tab.h src/include/scanner.h src/include/ast.h src/include/string_builder.h
SOURCES = lex.yy.c parser.tab.c src/ast.c src/string_builder.c

all: compiler.out

lex.yy.c: src/scanner.l
	flex src/scanner.l

parser.tab.h parser.tab.c &: src/parser.y
	bison -d src/parser.y


$(COMPILER_NAME): $(HEADERS) $(SOURCES)
	clang -o $(COMPILER_NAME) -Wall $(SOURCES) -DYYDEBUG -lfl


clean:
	rm *.tab.c *.tab.h *.yy.c *.out
