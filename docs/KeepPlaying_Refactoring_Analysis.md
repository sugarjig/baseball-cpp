# Analysis: Moving `KeepPlaying` Logic to `Simulator`

## Overview
This document analyzes the proposal to move the call to `KeepPlaying()` from `EventSource` implementations to the `Simulator::SimulateGame` method. Currently, most `EventSource` implementations check `state.KeepPlaying()` inside their `Next()` method to determine if they should signal the end of the event stream.

## Current Implementation
In `MatrixEventSource` and `RandomEventSource`:
```cpp
auto Next(const IGameState& state) -> std::optional<Event> {
    if (!state.KeepPlaying()) {
        return std::nullopt;
    }
    // ... generate event ...
}
```

In `Simulator`:
```cpp
void Simulator::SimulateGame(IGame& game) const {
    while (const auto event = eventSource->Next(game.GetGameState())) {
        // ... apply event ...
    }
}
```

## Proposed Change
In `Simulator`:
```cpp
void Simulator::SimulateGame(IGame& game) const {
    while (game.GetGameState().KeepPlaying()) {
        const auto event = eventSource->Next(game.GetGameState());
        if (!event) break;
        // ... apply event ...
    }
}
```

## Tradeoffs

### Pros
- **Separation of Concerns**: `EventSource` focuses on *providing* data, while `Simulator` focuses on *controlling* the simulation lifecycle.
- **Simplification of Generative Sources**: Sources like `RandomEventSource` or `MatrixEventSource` no longer need to be aware of game termination rules. This reduces duplication of the `KeepPlaying()` check across different sources.
- **Centralized Simulation Logic**: High-level rules about when a simulation should stop are consolidated in one place (`Simulator`), making it easier to modify or override (e.g., for "what-if" scenarios that continue past the 9th inning regardless of score).

### Cons
- **Potential Loss of Metadata (Static Sources)**: For file-based sources like Retrosheet, records often exist *after* the game technically ends (e.g., final comments, protests, or official attendance records). If the `Simulator` stops strictly when `KeepPlaying()` returns false, these records will be ignored.
- **Ambiguity in `Next()` return value**: `std::optional<Event>` would now exclusively mean "End of Data Stream" rather than "End of Game". While cleaner, it still requires `Simulator` to handle both the "Game Over" and "Out of Data" conditions.

### Pitfalls & Risks
- **Walk-off Timing**: In a walk-off scenario, the game ends *immediately* upon a certain action. If the `KeepPlaying()` check is performed at the start of the `while` loop, we must ensure the final walk-off event was already processed.
- **Post-Game Events**: As noted, Retrosheet `com` (comment) records are valuable. A strict `KeepPlaying()` check in the `Simulator` would skip them. `StaticEventSource` (Retrosheet) currently relies on `Next()` returning `std::nullopt` only when the file ends, effectively ignoring `KeepPlaying()` to ensure all records are read.

### Long-term Considerations
- **Simulation Policies**: Moving the logic to `Simulator` paves the way for injecting different "Simulation Policies". For example, a `NormalSeasonPolicy` might stop at the walk-off, while a `StatsGenerationPolicy` might always simulate exactly 9 innings for both teams to gather balanced data.
- **EventSource Interface**: The `Next()` method could be simplified to not require `IGameState` at all if the source doesn't need it for generation (though `MatrixEventSource` still needs it for state-based sampling).

## Recommendation
Move the `KeepPlaying()` check to `Simulator`, but provide a way to override it or ensure that "Static" sources can still drain their remaining records if they contain post-game metadata. For generative sources, the `Simulator` should definitely own the termination logic.
