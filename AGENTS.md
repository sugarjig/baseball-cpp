# Baseball-CPP Agent Guidelines

## Project Overview
`baseball-cpp` is a C++26 framework designed for baseball simulation and Retrosheet data processing. It serves as a modern C++ wrapper around the [Chadwick](https://github.com/chadwickbureau/chadwick) C library, providing a high-level, object-oriented interface for game simulation.

## Technical Architecture

### Core Components
- **`Simulator`**: The central engine that drives the simulation by pulling records from an `EventSource` and applying them to an `IGame` object.
- **`EventSource`**: An interface for providing a stream of baseball events (plays, substitutions, comments). `StaticEventSource` is a concrete implementation used for testing and fixed scenarios.
- **`IGame` / `Game`**: `IGame` defines the interface for a baseball game that can receive events. `Game` (in the `chadwick` namespace) is the primary implementation that wraps Chadwick's `CWGame` structures.
- **`IGameState` / `GameState`**: `IGameState` is the interface for the current state of a game (inning, score, outs, etc.). `GameState` (in the `chadwick` namespace) is the implementation backed by Chadwick's `cw_game_state`.
- **`Scorebook`**: A container for multiple games, providing functionality to read from and write to Retrosheet event files. Located in the `chadwick` namespace.
- **`SimulatorObserver`**: An optional observer that can hook into simulation events (pre-event, post-event, specific events).

### Chadwick Integration
The project wraps the Chadwick C library. Key integration points:
- Chadwick headers are included via `extern "C"`.
- Wrappers for Chadwick structures (`Game`, `GameState`, `Scorebook`) are encapsulated in the `chadwick` namespace.
- `Game` class manages the lifecycle of `CWGame` and `CWGameIterator` using RAII (manual cleanup in destructor).
- `Scorebook` manages the lifecycle of `CWScorebook`.
- **Move Semantics**: `Game` supports move semantics to transfer ownership of Chadwick resources efficiently.
- Raw pointers from Chadwick are kept private within wrapper classes.
- **Encapsulate all Chadwick usage in wrappers**: Avoid exposing Chadwick's internal structures directly.
- **Never modify vendored library source code**: If a bug is found in Chadwick, implement a workaround in the C++ wrapper instead of patching the C library.

## Development Standards

### Toolchain
- **Standard**: C++26 (enforced in `CMakeLists.txt`).
- **Build System**: CMake (minimum version 4.2).
- **Testing**: GoogleTest (GTest) and GMock.

### Coding Conventions
- **Naming**:
  - Methods and Functions: `PascalCase` (e.g., `SimulateGame()`, `AddEvent()`).
  - Variables and Members: `camelCase` (e.g., `eventSource`, `gameState`).
  - Classes and Structs: `PascalCase` (e.g., `Simulator`, `PlayInfo`).
- **Namespaces**:
  - All Chadwick wrappers and internal structures should reside in the `chadwick` namespace.
- **RAII**: Always use destructors or smart pointers to manage C resources from Chadwick to prevent memory leaks.
- **Interfaces**: Use abstract base classes (prefixed with `I`) for components that need decoupling or mocking (e.g., `IGame`, `EventSource`, `IGameState`).

### Quality Gates
To maintain code quality and consistency, all agents must run the following checks before submitting their work:
- **`clang-format`**: Ensures code adheres to the project's formatting standards.
- **`clang-tidy`**: Performs static analysis to catch potential bugs and enforce best practices.

A utility script is provided to run both checks:
```bash
./scripts/run-quality-checks.sh
```

**Requirements**:
1. Run `./scripts/run-quality-checks.sh` before every submission.
2. Fix all issues reported by `clang-tidy`. The script will exit with a non-zero code if any issues (including warnings) are found.
3. Ensure the project is configured with `CMAKE_EXPORT_COMPILE_COMMANDS=ON` (enabled by default in `CMakeLists.txt`) for `clang-tidy` to work correctly.

## Common Workflows

### Adding New Event Types
1. Update `Records.hpp` to include the new data structure.
2. Update the `Record` variant in `EventSource.hpp`.
3. Update `SimulatorObserver` to include a callback for the new event.
4. Update `Simulator::SimulateGame` to handle the new record type and notify the observer.
5. Add corresponding methods to `IGame` and `Game`.

### Writing Tests
- Use GoogleTest.
- Unit tests for logic should use mocks for `EventSource`, `IGame`, or `SimulatorObserver` where appropriate.
- See `tests/unit/SimulatorTests.cpp` for unit tests and `tests/integration/SimulatorTests.cpp` for end-to-end integration tests.

### Project Tools
- **`normalize_retrosheet`**: A utility tool that reads a Retrosheet file into a `Scorebook` and writes it back out, effectively normalizing the data using Chadwick's internal logic. Useful for verifying that wrappers correctly handle all records in a file.

## Domain Knowledge: Retrosheet
- **Event Files**: Retrosheet data is typically stored in event files with extensions like `.EVA`, `.EVN`, `.EVE`, `.EVO`, or `.EVS`. These files are plain text and contain a sequence of records.
- **Record Types**:
    - `id`: Unique identifier for the game.
    - `version`: Version of the Retrosheet format.
    - `info`: Metadata about the game (date, teams, location).
    - `start`: Initial lineups for both teams. Map to `StarterInfo`.
    - `sub`: Player substitutions. Map to `SubstitutionInfo`.
    - `play`: Main event records (hits, outs, etc.). Map to `PlayInfo`.
    - `badj`/`padj`: Batter/Pitcher adjustments (hand changes). Map to `BatterAdjustmentInfo`/`PitcherAdjustmentInfo`.
    - `radj`: Runner adjustments (e.g., placing a runner on base for extra innings). Map to `RunnerAdjustmentInfo`.
    - `com`: Comments, often used for extra-textual information or official rulings.
- **Play Record Details**: A `play` record includes inning, team (0=visitor, 1=home), batter ID, count at the time of the play, pitch sequence, and the play description.
- **Resources**: Detailed specifications for all Retrosheet record types can be found in the [Retrosheet Event File Documentation](https://www.retrosheet.org/eventfile.htm).
