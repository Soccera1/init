# GEMINI.md - Project Context

## Project Overview
This project is an init system (currently incomplete). It manages the execution of system services in a dependency-aware order. The system consists of two main components:
1.  **Solver (`bin/solver`):** Analyzes service scripts for a given "runlevel," resolves their dependencies using a topological sort, and generates an execution plan.
2.  **Main Runner (`bin/main`):** Executes the generated plan for a specific runlevel using the interpreter specified by the `SHELL` macro in `src/config.h`.

The individual service scripts are interpreted by the configured `SHELL` (default: `/bin/sh`). Consequently, all scripts must be compatible with this interpreter.

## Core Components
- `src/main.c`: The entry point for the runner. It sets up the environment (adding the script directory to `PATH`) and invokes the `SHELL` to execute the generated runlevel plan.
- `src/solver.c`: The dependency solver logic. It parses script headers for dependencies and outputs an ordered list of generic script names to a runlevel file.
- `src/config.h`: Contains configuration constants such as `INIT_DIRECTORY`, `SHELL`, and `DEPENDENCY_PREFIX`.
- `examples/`: Directory containing both the individual service scripts and the generated runlevel plans.

## Building and Running

### Prerequisites
- GCC or another C compiler.
- The interpreter specified in `src/config.h` (e.g., `/bin/sh`).

### Building
Compile the components with make:
```bash
make
```

### Running
1.  **Solve Dependencies:** Generate the execution plan for a runlevel (e.g., runlevel `1`):
    ```bash
    bin/solver 1
    ```
    This creates a file named `1` in the `examples/` directory.

2.  **Execute Runlevel:** Start the services in the correct order:
    ```bash
    bin/main 1
    ```

## Development Conventions

### Service Scripts
- **Location:** Place scripts in the directory defined by `INIT_DIRECTORY` (default: `/etc/init/init.d`).
- **Naming:**
    - Runlevel-specific: `<runlevel>.<generic_name>` (e.g., `1.logging.sh`). These are typically **symlinks** to the generic scripts.
    - Generic: `<generic_name>` (e.g., `logging.sh`). This is the file actually executed by the runner.
- **No Shebangs:** **Do not use shebangs** (e.g., `#!/bin/sh`). The first line of every runlevel-specific script is reserved for dependency declarations. Including a shebang will break the dependency solver.
- **Language:** Scripts are interpreted by the language defined in `src/config.h` via the `SHELL` macro.

### Declaring Dependencies
- Dependencies MUST be declared on the **first line** of the runlevel-specific script.
- Use the prefix defined by `DEPENDENCY_PREFIX` (default: `#`).
- List the generic names of the dependencies, separated by spaces.
- **Example (`examples/3.app.sh`):**
  ```bash
  # db.sh network.sh
  echo "Application service started"
  ```
  In this example, `app.sh` depends on `db.sh` and `network.sh`.

### Coding Style
- The codebase uses standard C with POSIX system calls (`fork`, `execl`, `waitpid`, etc.).
- Error handling is performed via `perror` and `exit`.
