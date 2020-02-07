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

To show the list of available arguments, run

```sh
make run ARGS=-h
```

And then:

```sh
make run ARGS="<list of arguments>"
```

Typically, to run a scanner on a specified file, run the following command:

```sh
make run ARGS="-f ./test/data/legit-token-salad.t3"
```

For more compact output, use the flag `-c`:

```sh
make run ARGS="-f ./test/data/legit-token-salad.t3 -c"
```

The scanner supports three different modes of output:

- default, JSON-like output
- `-c` - compact version
- `-r` - raw output, displays tokens only

## Run tests

> Note: instead of using `make test...` you can call ./test.sh <args> instead.

The `test.sh` runs all of the test files in the `test/data` directory. To run a single file, refer to **'Running the scanner'** section above.

To run the tests, run the following command

```sh
make test
```

To get more compact output:

```sh
make test ARGS=-c
```

## Author

**Artem Golovin**

