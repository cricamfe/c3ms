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
./c3ms [-f] [-v level] <files>
```

### Description 

The `c3ms` tool calculates Halstead's programming effort and various complexity metrics based on the number of tokens in the provided files. Each file is considered an independent module, and metrics are calculated for each file individually as well as aggregated across all files.

#### Criteria for Tokens

- **Operands**: Constants, basic types (int, float, etc.), and identifiers.
- **Operators**: Storage and type modifiers (const, static, etc.), language keywords, and operators.

Additionally, the tool outputs Halstead's volume and counts the number of conditional statements, including `case`, `default`, `for`, `if`, and `while`.

### Options

- `-f`,`--function`: Analyzes the effort for each function in the provided files.
- `-v [level]`, `--verbosity [level]`: Adjusts the output detail level.
  - **Level 0**: No additional metrics (default).
  - **Level 1**: Basic metrics (Effort, Volume, Conditions, Cyclomatic Complexity, Difficulty, Time Required, Bugs, Maintainability).
  - **Level 2**: Basic metrics and Additional Statistics (Types, Constants, Identifiers, Cspecs, Keywords, Operators).
  - **Level 3**: All the above metrics plus Detailed Metrics (n1 - unique operators, n2 - unique operands, N1 - total operators, N2 - total operands).