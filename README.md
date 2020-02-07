# j--

![Version](https://img.shields.io/badge/version-0.0.1-blue.svg?cacheSeconds=2592000)

> J-- Compiler

## Building Process

To build the scanner, run the following command

```sh
make build
```

To remove the compiled and generated files:

```sh
make clear
```

## Running the scanner

> Note: instead of using `make run...` you can call ./scanner <args> instead

Typically, to run a scanner on a specified file, run the following command:

```sh
make run ARGS="./test/data/legit-token-salad.t3"

# or
./scanner ./test/data/legit-token-salad.t3
```

## Run tests

> Note: instead of using `make test...` you can call ./test.sh <args> instead.

The `test.sh` runs all of the test files in the `test/data` directory. To run a single file, refer to **'Running the scanner'** section above.

To run the tests, run the following command

```sh
make test
```

## Author

**Artem Golovin**
