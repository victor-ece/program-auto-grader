# Programming II Project - Semester 2

## Overview
This project is designed to automate the compilation, execution, and evaluation of C programs based on their compilation, termination, output, and memory access behavior. The system consists of three main components:

1. **`hw4.c`**: The main program that orchestrates the compilation, execution, and evaluation of a C program.
2. **`p4diff.c`**: A utility program that compares the output of the compiled program with the expected output and calculates a similarity score.
3. **`test.c`**: A simple test program that demonstrates the functionality of the system.

## Features
- **Compilation**: Automatically compiles a C program using `gcc` and captures compilation errors and warnings.
- **Execution**: Runs the compiled program with input from a specified file and captures its output.
- **Output Comparison**: Compares the program's output with the expected output using `p4diff.c` and calculates a similarity score.
- **Timeout Handling**: Enforces a timeout for program execution and terminates the program if it exceeds the specified time.
- **Scoring**: Calculates scores based on:
  - **Compilation**: Deducts points for warnings and errors.
  - **Termination**: Deducts points if the program doesn't terminate normally or exceeds the timeout.
  - **Output**: Compares the program's output with the expected output and assigns a score based on similarity.
  - **Memory Access**: Deducts points if the program crashes due to memory-related issues (e.g., segmentation fault).

## How It Works
1. **Compilation**: `hw4.c` compiles the C program (`argv[1]`) using `gcc`. If there are compilation errors or warnings, it deducts points accordingly.
2. **Execution**: The compiled program is executed with input from a `.in` file (`argv[3]`). The output is captured and compared with the expected output from a `.out`
 file (`argv[4]`).
3. **Comparison**: `p4diff.c` compares the actual output with the expected output and calculates a similarity score.
4. **Timeout Handling**: If the program takes too long to execute (based on the timeout value in `argv[5]`), it is terminated, and points are deducted.
5. **Scoring**: The final scores for compilation, termination, output, and memory access are printed.

## Usage

### Building the Project
The project includes a `Makefile` to simplify the build process. The `Makefile` contains the following rules:

- **`hw4`**: Compiles `hw4.c` and links it with the AddressSanitizer for memory error detection.
- **`p4diff`**: Compiles `p4diff.c` to create the output comparison utility.
- **`clean`**: Removes the compiled binaries (`hw4` and `p4diff`).

To build the project, run the following command in the terminal:

```bash
make
```

### Running the Project
After building the project, you can run the `hw4` program to compile, execute, and evaluate a C program. The `hw4` program takes five command-line arguments:

```bash
./hw4 <progname.c> <progname.args> <progname.in> <progname.out> <timeout>