# j--

![Version](https://img.shields.io/badge/version-0.0.1-blue.svg?cacheSeconds=2592000)

> J-- Compiler

## Getting Started

These are the steps to build the parser

### Building

To build the parser:

```sh
make

# or
make all
```

To remove the compiled and generated files:

```sh
make clear
```

### Running the parser

Executable will be generated in the project root. To run it:

```sh
./jay <path to a file>
```

If no file is specified, the parser will fallback to reading from `stdin`.

### Running tests

The project contains a regular, simple test runner and some unit tests. All test files are located in `test` directory.

#### Test runner

The `test.sh <dirname>` runs all of the test files in the `test/<dirname>` directory. To run a single file, refer to **'Running the parser'** section above.

To run the test runner, run the following command (it will perform a clean build):

```sh
make test_runner
```

> Note: instead of using `make test...` you can also call ./test.sh <args> instead (ensure to make it executable using `chmod +x ./test.sh`).

#### Unit Tests

The project contains a test suite, built using [Catch2](https://github.com/catchorg/Catch2) test framework. To run the tests:

```sh
make test
```

`make test` will perform a clean build of both test and regular executables. The test executable that it produces, will be located in the project root. To run it:

```sh
./jay.test
```

To display more verbose output:

```sh
./jay.test -s
```

To display all possible options,

```sh
./jay.test -?

# if using fish shell:
./jay.test "-?"
```


## Author

**Artem Golovin**
