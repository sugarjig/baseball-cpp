# Baseball-CPP Agent Guidelines

## Project Overview
`baseball-cpp` is a C++26 framework designed for baseball simulation and Retrosheet data processing. It serves as a modern C++ wrapper around the [Chadwick](https://github.com/chadwickbureau/chadwick) C library, providing a high-level, object-oriented interface for game simulation.

## Technical Architecture

### Core Components
- **`Simulator`**: The central engine that drives the simulation by pulling records from an `EventSource` and applying them to an `IGame` object.
- **`EventSource`**: An interface for providing a stream of baseball events (plays, substitutions, comments). `StaticEventSource` is a concrete implementation used for testing and fixed scenarios.
- **`IGame` / `Game`**: `IGame` defines the interface for a baseball game that can receive events. `Game` is the primary implementation that wraps Chadwick's `CWGame` structures.
- **`GameState`**: A read-only view of the current state of a game (inning, score, outs, etc.), backed by Chadwick's `cw_game_state`.
- **`SimulatorObserver`**: An optional observer that can hook into simulation events (pre-event, post-event, specific events).

### Chadwick Integration
The project wraps the Chadwick C library. Key integration points:
- Chadwick headers are included via `extern "C"`.
- `Game` class manages the lifecycle of `CWGame` and `CWGameIterator` using RAII (manual cleanup in destructor).
- Raw pointers from Chadwick are generally kept private within wrapper classes.

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
- **RAII**: Always use destructors or smart pointers to manage C resources from Chadwick to prevent memory leaks.
- **Interfaces**: Use abstract base classes (prefixed with `I`) for components that need decoupling or mocking (e.g., `IGame`, `EventSource`).

## Common Workflows

### Adding New Event Types
1. Update `Records.hpp` to include the new data structure.
2. Update the `Record` variant in `EventSource.hpp`.
3. Update `Simulator::SimulateGame` to handle the new record type.
4. Add corresponding methods to `IGame` and `Game`.

### Writing Tests
- Use GoogleTest.
- Unit tests for logic should use mocks for `EventSource` or `IGame` where appropriate.
- See `tests/SimulatorTests.cpp` for examples.

## Domain Knowledge: Retrosheet
- The simulation logic is heavily based on Retrosheet event files (`.EVA`, `.EVN`, etc.).
- A `PlayInfo` record typically includes inning, team (0=visitor, 1=home), batter ID, pitch count, pitch sequence, and the play description string.
- Detailed play specifications can be found in the [Retrosheet documentation](https://www.retrosheet.org/eventfile.htm).
