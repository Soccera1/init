# Simple Init System

A dependency-aware init system that resolves service order and executes them.

## Building

```bash
mkdir -p bin
gcc src/main.c -o bin/main
gcc src/solver.c -o bin/solver
```

## Usage

1. **Resolve dependencies** for a runlevel (e.g., `1`):
   ```bash
   bin/solver 1
   ```
2. **Execute** the runlevel:
   ```bash
   bin/main 1
   ```

## Configuration

Example service scripts are located in `examples/`. Dependencies are declared on the first line of runlevel-specific scripts (e.g., `1.service.sh`) using a `#` prefix followed by the generic script names.
