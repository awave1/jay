all: a.out

a.out: parser.tab.hh parser.tab.cc lex.yy.cc
	g++ -std=c++17 *.cc *.cpp -Wno-register

parser.tab.hh parser.tab.cc &: parser.yy
	bison parser.yy
	bison -d parser.yy

lex.yy.cc: lex.l
	flex -o lex.yy.cc lex.l

clean:
	rm *.cc *.hh *.out
