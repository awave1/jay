COMPILER = jay
TEST_EXEC = jay.test
CXXFLAGS = -Wall -std=c++17
LDFLAGS = -lfl
DFLAGS := 
CXX = g++

SRC_PATH = ./src
BUILD_PATH = ./build
# INCLUDE := -I $(SRC)/include/ -I parser.tab.hh
INCLUDE := -I $(SRC_PATH)/include
TESTINCLUDE := -I lib/catch2

SOURCES = lex.yy.cc parser.tab.cc src/string_builder.c src/SymTable.cpp src/SemanticAnalyzer.cpp src/JayCompiler.cpp
CPP_SOURCES = $(shell find $(SRC_PATH) \( -name '*.cpp' \) -printf '%T@\t%p\n' | sort -k 1nr | cut -f2-)
CPP_HEADERS = $(shell find $(SRC_PATH) -name '*.h*' -printf '%T@\t%p\n' | sort -k 1nr | cut -f2-)
OBJECTS = $(CPP_SOURCES:$(SRC_PATH)/%.cpp=$(BUILD_PATH)/%.o)

export V := false
export CMD_PREFIX := @
ifeq ($(V),true)
	CMD_PREFIX := 
endif

.PHONY: all
all:
	@ $(MAKE) -s scanner
	@ $(MAKE) -s parser
	@ $(MAKE) -s $(COMPILER)

scanner: $(SRC_PATH)/scanner.l
	@ echo "flex: generating scanner"
	$(CMD_PREFIX)flex -o $(SRC_PATH)/lex.yy.cpp src/scanner.l

parser: $(SRC_PATH)/parser.yy
	@ echo "bison: generating parser"
	$(CMD_PREFIX)bison -t -d $(SRC_PATH)/parser.yy -o $(SRC_PATH)/parser.tab.cpp
	@ mv $(SRC_PATH)/*.h* $(SRC_PATH)/include/

$(COMPILER): $(OBJECTS)
	@ echo "compiling executable: \`$@\`"
	$(CMD_PREFIX)$(CXX) $(CXXFLAGS) $(DFLAGS) $(OBJECTS) -o $@
	@ echo "compiled. executable is \`$@\`"

$(BUILD_PATH)/%.o: $(SRC_PATH)/%.c*
	@ echo "compiling $< -> $@"
	$(CMD_PREFIX)$(CXX) $(CXXFLAGS) $(DFLAGS) $(INCLUDE) -c $< -o $@

.PHONY: build
build: clear
build: all

.PHONY: debug
debug: DFLAGS=-g -DYYTRACE
debug: clear
debug: all

.PHONY: run
run: clear
run: $(COMPILER)
run:
	./$(COMPILER) $(FILE)

.PHONY: run
run_debug: debug
run_debug:
	./$(COMPILER) $(FILE)

.PHONY: test
test: clean
test: all
test: $(TEST_EXEC)

$(TEST_EXEC): test/semantic.test.cpp $(HEADERS) $(SOURCES)
	$(CMD_PREFIX)$(CXX) -g $(TESTINCLUDE) $(DFLAGS) -I src/include test/semantic.test.cpp $(SOURCES) -o $@
	./$(TEST_EXEC)

.PHONY: test_runner
test_runner: clean
test_runner: all
test_runner:
	@ chmod +x ./test.sh
	./test.sh $(COMPONENT)

.PHONY: export
export: clean
export: ARCHIVE_NAME := artem-golovin-$(MSPART)
export:
	git archive -o $(ARCHIVE_NAME).zip HEAD

.PHONY: clear
clear:
	@ echo "> cleaning build & misc files..."
	@ -rm -rf src/include/stack.hh src/*.tab.cpp src/include/*.tab.hpp src/*.yy.cpp $(COMPILER) $(TEST_EXEC) *.totallynotzip 2> /dev/null
	@ -rm -rf build/*.o 2> /dev/null
	@ echo "> done"
