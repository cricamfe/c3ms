# c3ms: Halstead's Effort and Complexity Metrics Calculator

## Installation

### Debug/Development Mode

```shell
make DEBUG=1
```

### Release Mode

```shell
make
```

### Usage

```shell
./c3ms [-h] [-f] [-a] [-g] [-v level] <files>
```

### Description 

The `c3ms` tool calculates Halstead's programming effort and various complexity metrics based on the number of tokens in the provided files. Each file is considered an independent module, and metrics are calculated for each file individually as well as aggregated across all files.

#### Criteria for Tokens

- **Operands**: Constants, basic types (int, float, etc.), and identifiers.
- **Operators**: Storage and type modifiers (const, static, etc.), language keywords, and operators.

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

- A complete restructuring of the project, introducing a new directory structure (`bin`, `src`, `include`, `obj`, etc.) and an updated Makefile that supports both `RELEASE` and `DEBUG versions`.
- Expansion of the lexicon to incorporate modern programming concepts and technologies such as `C++`, `SYCL`, `oneTBB`, `AVX2`, `SIMD`, etc.
- Refinement of the `CodeStatistics` component using `string_view` to improve performance.
- Introduction of new metrics such as `cyclomatic complexity`, `maintainability index`, `time required` and `bugs delivered`.
- Improved parsing capabilities, allowing files to be broken down into functions using flags such as `--function-metrics`, `--file-metrics` and `--global-metrics`.
- Improved presentation of results with colour-coded output and tabular formats.
- Development of new `CodeUtils` and `CodeMetrics` files for centralized metric calculations and utility functions.
- Redistribution of functions from `c3ms.cpp` into these new support files.
- General refactoring of the original code by Basilio B. Fraguela (2009-2010) to take advantage of new C++ features and add additional functions.

These updates are intended to significantly increase the performance, usability and analytical capabilities of the library.

---

Copyright (c) 2009, 2010 Basilio B. Fraguela. University of A Coruña
