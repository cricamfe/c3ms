# c3ms: Halstead's Effort and Complexity Metrics Calculator

## Installation

### Debug/Development Mode

```shell
./configure --with-debug
make
```

### Release Mode

```shell
./configure
make
```

### Usage

```shell
./C3MS [-h] [-f] [-a] [-g] [-v level] <files>
```

### Description 

The `c3ms` tool calculates Halstead's programming effort and various complexity metrics based on the number of tokens in the provided files. Each file is considered an independent module, and metrics are calculated for each file individually as well as aggregated across all files.

#### Criteria for Tokens

- **Operands**: Constants, types (int, float, etc.), identifiers, C specifiers and API (high level) keywords like oneTBB, SYCL, etc.
- **Operators**: Language keywords, API (low level) keywords and custom keywords from developer and operators.

Additionally, the tool outputs Halstead's volume and counts the number of conditional statements, including `case`, `default`, `for`, `if`, and `while`.

### Options

- `-h`, `--help`: Show this help message.
- `-f`, `--function-metrics`: Analyze and report metrics for each function.
- `-a`, `--file-metrics`: Analyze and report metrics for each file.
- `-g`, `--global-metrics`: Analyze and report global metrics across all files.
- `-v [level]`, `--verbosity [level]`: Adjusts the output detail level.
  - **Level 1:** Basic metrics _(Effort, Volume, Conditions, Cyclomatic Complexity, Difficulty, Time Required, Bugs, Maintainability)_
  - **Level 2:** Basic metrics and Additional Statistics _(Types, Constants, Identifiers, Cspecs, Keywords, Operators)_
  - **Level 3:** All the above metrics plus Detailed Metrics _(n1 - unique operators, n2 - unique operands, N1 - total operators, N2 - total operands)_

### Updates

#### 2023

##### Contributions by Cristian Campos Ferrer (University of Málaga)

In 2023, updates and improvements have been made to the library. Key improvements include:

- A complete restructuring of the project, introducing a new directory structure (`src`, `test`, `docs`, etc.) and an updated CMakeLists files that supports both `RELEASE` and `DEBUG` versions.
- Expansion of the lexicon to incorporate modern programming concepts and technologies such as `C++`, `SYCL`, `oneTBB`, `AVX2`, `SIMD`, etc.
- Enhanced the tool's capability to automatically process functions developed by the programmer, which are not derived from any API. This significantly improves the detection and analysis of operators within the code.
- Refinement of the `CodeStatistics` component, the implementation of which has been moved to `codestatistics.hh` and `codestatistics.cc`.
- Introduction of new metrics such as `cyclomatic complexity`, `maintainability index`, `time required` and `bugs delivered`.
- Improved parsing capabilities, allowing files to be broken down into functions using flags such as `--function-metrics`(-f), `--file-metrics`(-a) and `--global-metrics`(-g).
- Improved presentation of results with colour-coded output and tabular formats.
- Development of new `CodeUtils` and `CodeMetrics` files for centralized metric calculations and utility functions.
- Redistribution of functions from `main.cpp` into these new support files.
- General refactoring of the original code by Basilio B. Fraguela (2009-2010) to take advantage of new C++ features and add additional functions.
- The project structure has been generated using the [template](https://github.com/remusao/Bison-Flex-CPP-template) created by remusao, which incorporates the latest versions of Flex and Bison in C++. This approach ensures modern, efficient parsing and lexing capabilities within the project.

These updates are intended to significantly increase the performance, usability and analytical capabilities of the library.

---

Copyright (c) 2009, 2010 Basilio B. Fraguela. University of A Coruña
