all: compiler.out

parser.source: parser.yy
	bison parser.yy
	bison -d parser.yy

lex.yy.cc: parser.source lex.l
	flex -o lex.yy.cc lex.l

compiler.out: Driver.cpp Driver.h lex.yy.cc parser.source
	g++ -std=c++17 -o compiler.out Driver.cpp lex.yy.cc parser.tab.cc

clean:
	rm *.cc *.hh *.out

