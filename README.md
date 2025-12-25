# ReversreOscilloscopy-SBM
README.md (usage + CLI commands)
# ReverseOscillasopy-SBM — Readable Traces & Silent-Bug Matrix Exercises

This repository provides two pedagogical areas:

1. Human-readable ASCII "oscilloscope-like" traces for logic, memory, and bug/event overlays (examples in `examples/`).
2. A focused lab on silent numerical bugs using three intentionally broken matrix-multiply implementations:
   - `matmul_broken_A` — compact style (missing final write; silent omission).
   - `matmul_broken_B` — exaggerated spacing and a subtle indexing/transposition bug.
   - `matmul_broken_C` — normal spacing but uses single-precision accumulation (precision-losing accumulation).

Each broken implementation is intentionally designed to be "silent": the program runs, but numerical outputs differ from a correct reference. The included test harness helps learners detect, quantify, and reason about these differences.

Getting started (recommended)
- Clone the repository (or switch to the `SilentBugMatrix` branch if provided).
- Build one or more test binaries using the Makefile.
- Run the harness and examine the summary metrics: max absolute difference, sum absolute difference, and sample matrix entries.

Example — Build & run the broken matrix multiplication implementations:

1. Build and run version A (compact, missing write)
```bash
# From repository root
make test_run_A
./test_run_A            # default: small deterministic test

# or with custom size and scenario:
./test_run_A 8 identity # N=8, identity input scenario
```

2. Build and run version B (exaggerated spacing, indexing bug)
```bash
make test_run_B
./test_run_B
```

3. Build and run version C (float accumulation precision loss)
```bash
make test_run_C
./test_run_C
```

4. Build and run all tests (convenience)
```bash
make test_run_all
```

CLI usage (test_run_X)
```
Usage:
  ./test_run_X [N] [scenario] [tolerance]

Arguments:
  N         : matrix dimension (default 6)
  scenario  : one of: increment, identity, random, pattern (default: increment)
  tolerance : floating threshold for "acceptable" max-difference (default 1e-12)

Exit codes:
  0 : no difference detected within tolerance
  2 : difference detected (silent bug manifested)
  1 : internal error (allocation / invalid args)
```

Scenarios
- increment : deterministic integer-valued matrices with small values, useful to reveal many structural bugs.
- identity  : B = identity, so result should equal A (great to detect row/col transposition bugs).
- random    : pseudo-random values (reproducible deterministic seed).
- pattern   : small pre-defined pattern useful for visual inspection.

What to look for
- Max absolute difference significantly > tolerance indicates the silent bug impacted numerical results.
- Check sample entries to see patterns (all zeros, transposed pattern, small systematic errors).
- For `matmul_broken_C`, expect small but non-zero differences due to precision loss; increase `N` to amplify accumulation errors.

Pedagogy & exercises
- Ask learners to:
  - Run each broken version and explain the observed mismatch pattern.
  - Fix one implementation and re-run the harness; add unit tests that assert max difference <= tolerance.
  - Add a CI job that runs `make test_run_all` and fails when the exit code indicates a mismatch.

If you want additional deliverables:
- A correct reference file `src/matmul_ref.c` to link instead of computing the reference in the harness.
- Larger test matrices and automated CSV/JSON output for data-driven analysis.
- A short lab worksheet including expected outputs and discussion prompts.
