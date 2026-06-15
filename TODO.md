# TODO

## Simulator
- [ ] Add a random event source
- [ ] Determine probability of state transitions instead of outcomes (S, D, T, HR, etc.)
- [ ] Refactor UpdateState (call UpdateState from Add methods, manage iterator in GameState?)

## Code Quality
- [ ] Ask Junie for a code review
- [ ] Add a README
- [ ] Audit C++ usage for possible 26 improvements
- [ ] Use -Wall -Werror flags (and possibly -Wextra and -Wpedantic)
- [ ] Automatically format code with clang-format before committing
- [ ] Audit AGENTS.md

## Testing
- [ ] More unit test coverage
- [ ] Separate unit tests from integration tests

## Tooling
- [ ] Structured logging
- [ ] Use [diffing library](https://github.com/sisong/HDiffPatch) in integration test
- [ ] Use [CSV parser](https://github.com/vincentlaucsb/csv-parser) in integration test
- [ ] Use [run-vcpkg](https://github.com/lukka/run-vcpkg) in GitHub Actions to cache packages
  - [ ] Or use [handrolled solution](https://learn.microsoft.com/en-us/vcpkg/consume/binary-caching-github-packages?pivots=windows-runner)
- [ ] Publish Chadwick to vcpkg
  - https://learn.microsoft.com/en-us/vcpkg/get_started/get-started-packaging?pivots=shell-bash
  - https://learn.microsoft.com/en-us/vcpkg/get_started/get-started-adding-to-registry?pivots=shell-powershell

## Agent Usage
- [ ] Include Retrosheet documentation as context
