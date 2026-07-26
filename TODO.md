# TODO

## Simulator
- [x] Use transition matrix for each starting state
- [x] Refactor UpdateState (call UpdateState from Add methods)
- [x] Rename AddEvent and OnEvent to AddPlay and OnPlay
- [x] Add OnEvent method to SimulatorObserver (replace calls to individual On* calls)
- [x] Rename Record to Event
- [x] Move Record/Event struct to EventSource.hpp
- [x] Remove Starter from EventType
- [x] Move record processing from Simulator to Game (AddEvent)
- [x] Create GameIter class to mirror Chadwick structure (manage Chadwick GameState in GameIter)
- [ ] Make UpdateState private (move explaining comment in Game.cpp somewhere else)
- [ ] Move KeepPlaying calls out of event source implementations (into Simulator?)
- [ ] Add abstraction over Retrosheet records
- [ ] Simulate pitch sequences

## Matrix Event Source
- [ ] Consider batter and pitcher handedness in matrices
- [ ] Consider ballpark in matrices
- [ ] Create transition matrices for individual batters, pitchers, fielders, runners, umpires
- [ ] Fix matrix simulator bugs
  - [ ] Error advancements not handled correctly
  - [ ] Error and fielder's choice codes

## Code Quality
- [ ] Ask Junie for a code review
- [ ] Add a README
- [ ] Audit C++ usage for possible 26 improvements
- [ ] Audit CMake usage
- [ ] Use -Wall -Werror flags (and possibly -Wextra and -Wpedantic)
- [ ] Audit AGENTS.md

## Testing
- [ ] More unit test coverage

## Tooling
- [ ] Structured logging
- [ ] Use [diffing library](https://github.com/sisong/HDiffPatch) in integration test
- [ ] Use [CSV parser](https://github.com/vincentlaucsb/csv-parser) in an integration test
- [ ] Use [run-vcpkg](https://github.com/lukka/run-vcpkg) in GitHub Actions to cache packages
  - [ ] Or use [handrolled solution](https://learn.microsoft.com/en-us/vcpkg/consume/binary-caching-github-packages?pivots=windows-runner)
- [ ] Publish Chadwick to vcpkg
  - https://learn.microsoft.com/en-us/vcpkg/get_started/get-started-packaging?pivots=shell-bash
  - https://learn.microsoft.com/en-us/vcpkg/get_started/get-started-adding-to-registry?pivots=shell-powershell

## Agent Usage
- [ ] Include Retrosheet documentation as context
