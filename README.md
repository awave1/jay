# j--

![Version](https://img.shields.io/badge/version-0.0.1-blue.svg?cacheSeconds=2592000)

> J-- Compiler

## Getting Started

These are the steps to build the parser

### Building

To build the parser:

```sh
make build
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

### Running tests

The project contains a regular, simple test runner and some unit tests. All test files are located in `test` directory.

#### Test runner

The `test.sh` runs all of the test files in the `test/codegen` directory. To run a single file, refer to **'Running the parser'** section above.

To run the test runner, run the following command (it will perform a clean build):

```sh
make test_runner
```

> Note: instead of using `make test_runner` you can also call `./test.sh` instead (ensure to make it executable using `chmod +x ./test.sh`).

## Author

**Artem Golovin**
