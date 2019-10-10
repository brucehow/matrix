# Sparse Matrices &middot; [![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/brucehow/matrix/blob/master/LICENSE) [![mac version](https://img.shields.io/badge/macOS-10.12.6-blue.svg)]()

The motivation behind this project is to understand the fundamentals behind the time and space complexity of algebraic routines on sparse matrices, given different conditions. A major part of this project is understanding the im- portance of preconditioning, where the use of different preconditioners, which are the different sparse matrix representations; as well as multi-threading and parallelism, can lead to a significant reduction routine computation times.

Information regarding OpenMP multi-threading and the different sparse matrix representations have been covered in detail in the report.

## Installation
Use the provided `Makefile` to compile the program which will create an executable `matrix` program.
```bash
make install
```

## Usage
The program must be run with parameters. There are a few optional and required parameters. View the CLA usage below.

```bash
matrix {routines} [options] -f matrix1 [matrix2]
```
#### Routines:
* `--sm scalar` perform scalar multiplcation with value scalar
*  `--tr`		compute the matrix trace value
*  `--ad`		perform matrix addition on two matrices, matrix2 must be specified
*  `--ts`		transpose the given matrix
*  `--mm`		perform matrix multiplication on two matrices, matrix2 must be specified

#### Options:
* `-t threads` specify the number of execution threads to use
*  `-l`	if present, results will be logged to an output file
*  `-s`	if present, results will not contain the matrix data

This program has been tested on Mac environments.

## Authors
- [Bruce How](https://github.com/brucehow)

## License

This project is licensed under the [MIT License](https://github.com/brucehow/matrix/blob/master/LICENSE).

