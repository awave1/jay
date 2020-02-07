CXX=gcc
CFLAGS=-Wall --std=c11

FLEX=flex

SRC=./src
BUILD=./build
GENERATED_SRC=$(BUILD)/generated-sources
BIN=$(BUILD)/bin
TARGET=scanner
LEX_EXT=yy.c

SOURCE = \
				 src/string_builder.c

HEADERS = \
					src/include/string_builder.h

scanner:
	$(FLEX) -o $(GENERATED_SRC)/$(TARGET).$(LEX_EXT) $(SRC)/scanner/scanner.l

run:
	$(BIN)/scanner ${ARGS}

build: $(SOURCE) $(HEADERS)
	mkdir -p $(BUILD)
	mkdir -p $(BIN)
	mkdir -p $(GENERATED_SRC)
	make scanner
	$(CXX) $(CFLAGS) -ll $(GENERATED_SRC)/$(TARGET).$(LEX_EXT) $(SOURCE) -o $(BIN)/$(TARGET)
	ln -s $(BIN)/$(TARGET) $(TARGET)

test: clear build
	chmod +x ./test.sh
	./test.sh

clear:
	rm -rf build
	rm $(TARGET)
