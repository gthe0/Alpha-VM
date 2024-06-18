# Alpha-VM
Welcome to the Alpha-VM Project!
This Repository contains the source code for the Alpha Virtual Machine used to run the Alpha-Binary files produced by the Alpha-Compiler!

## Overview
Alpha-VM was written specifically for educational purposes. It serves as a stepping stone for understanding how a modern-day VM such as the Java Virtual Machine (JVM) works and should not be regarded as a fully-fledged one.

## Introduction
The Alpha-VM runs the Binary Files produced by the Alpha-Compiler!

The VM is a CLI tool writen in C. 

## Installation

Clone the repository to your local machine using either the GitHub Desktop app or by using the following command:

```console
git clone https://github.com/gthe0/Alpha-VM
```

## Compilation
For compiling the project, you need to have installed
- A C compiler

For \*nix platform, a Makefile is being provided, so simply run:
```console
make
```

For Windows, compile and link all the object files, forming the executable.

## Usage
As mentioned previously, the Alpha-VM is a CLI tool, so to use it you can run the executable and pass an Alpha Binary File 

You can also pass the following flag: 

- -i Produces a file called tcg_instructions.txt containing the target code/instructions and the buffers contained inside the Binary File in a human readable format.

## Contributing

Contributions to this project are welcome! If you would like to contribute, please follow these steps:

- Fork the repository.
- Create a new branch for your feature or bug fix.
- Make your changes and commit them.
- Push your changes to your forked repository.
- Submit a pull request, explaining the changes you have made.

