# TODO

## Simulator
- [x] Use transition matrix for each starting state
- [ ] Consider batter and pitcher handedness in matrices
- [ ] Consider ballpark in matrices
- [ ] Refactor UpdateState (call UpdateState from Add methods, manage iterator in GameState?)
- [ ] Move KeepPlaying out of Chadwick GameState (into Simulator?)
- [ ] Add abstraction over Retrosheet records
- [ ] Simulate pitch sequences
- [ ] Create transition matrices for batters, pitchers, fielders, umpires

## Code Quality
- [ ] Ask Junie for a code review
- [ ] Add a README
- [ ] Audit C++ usage for possible 26 improvements
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
