# c3ms: Halstead's Effort Calculator

## Installation

### Debug/Development Mode

```shell
make
```

### Production Mode

```shell
make PRODUCTION=1
```

### Usage

```shell
./c3ms [-g] [-v level] <files>
```

### Description 

The `c3ms` tool calculates Halstead's programming effort based on the number of tokens in the provided files. Each file is considered an independent module, and the final volume and programming effort are sums of the calculations for each file.

#### Criteria for Tokens

- **Operands**: Constants, basic types (int, float, etc.), and identifiers.
- **Operators**: Storage and type modifiers (const, static, etc.), language keywords, and operators.

Additionally, the tool outputs Halstead's volume and counts the number of conditional statements, including `case`, `default`, `for`, `if`, and `while`.

### Options

- `-g`: Computes global effort across all files as a single module. When used with `-v`, it provides detailed global statistics.
- `-v`: Varies the output detail level.
  - **Level 1**: Effort, volume, and number of conditions per file.
  - **Level 2**: Number of tokens and unique tokens of each kind.
  - **Level 3**: Number of operands and operators