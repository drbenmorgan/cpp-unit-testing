---
title: "Organizing code to enable unit testing"
teaching: 10 # teaching time in minutes
exercises: 2 # exercise time in minutes
---
::::::::::::::::::::::::::::::::::::: questions 

- How should we structure C++ code to assist unit testing?
- What makes a function easy or hard to test?

::::::::::::::::::::::::::::::::::::::::::::::::

::::::::::::::::::::::::::::::::::::: objectives
- Split a single-file C++ program into a header, an implementation file, and a separate test file
- Explain why separating test code from production code matters
- Identify properties of a function that make it easy to test: clear inputs, clear outputs, no hidden dependencies
- Identify at least three structural problems in a given function that make it difficult to test
- Propose a refactoring of a function with testability problems into smaller, testable units
- Explain why the question "how would I ensure this refactoring does not change behaviour?" motivates writing tests before refactoring

::::::::::::::::::::::::::::::::::::::::::::::::

## C++ Package Organization

At present, we have both the unit of code we want to test and the test code in a single file.
Practically, the `invariant_mass` function is more likely to be part of a larger C++ _project/package_  
that compiles a large set of functions and classes into an end-user program or a _library_ of
reusable, pre-compiled code. 

In terms of testing, this means that we want to separate the program/library _interface and implementation_ code
from that which _tests_ it. Unlike some languages, the ISO C++ Standard does _not_ enforce 
or require a specific directory layout of package implementation and testing code, leaving this
up to the package maintainers. For this lesson, we will organise our code into the following
directories:

```
+- ccptepp-test/
   +- src/
      ... headers declaring interfaces and implementation files defining them ...
   +- test/
      ... unit tests for the interfaces declared in src/ ...
```

## Splitting `test_invariant_mass` into a header, implementation, and test program

Let's start by splitting the `invariant_mass` function out from the test program.
Open a new _header_ file `invariant_mass.hpp` in `src/` and move the function from `test/test_invariant_mass.cpp` into it:

```cpp
//! \file invariant_mass.hpp
#pragma once // header guard

#include <cmath>
#include <stdexcept>

// 1. Return invariant mass $m = sqrt(E^2 - p^2) in natural units
// 2. throws std::domain_error if E < 0
// 3. throws std::domain_error if E^2 - p^2 < 0
double invariant_mass(double energy, double momentum)
{
   if (energy < 0)
   {
      throw std::domain_error("unphysical negative energy");
   }
   double mass_squared = energy*energy - momentum*momentum;
   if (mass_squared < 0)
   {
      throw std::domain_error("unphysical mass^2");  
   }
   return std::sqrt(mass_squared);
}
```

We can now modify `test_invariant_mass.cpp` to simply include this header to provide the
function interface.

```cpp
//! \file test_invariant_mass.cpp
#include "invariant_mass.hpp" // Include the interface for what we're testing

// Run the tests
int main()
{
   double photon_mass = invariant_mass(100,100);
}
```

We now need to tell the compiler where to find the new header using `-I` to specify
where it should look, but otherwise everything is as before:

:::::::::::::::: spoiler
### Linux

```bash
g++ -std=c++17 -I src/ test/test_invariant_mass.cpp -o test_invariant_mass    
./test_invariant_mass
```
::::::::::::::::::::::::

:::::::::::::::: spoiler
### MacOS

```bash
g++ -std=c++17 -I src/ test/test_invariant_mass.cpp -o test_invariant_mass    
./test_invariant_mass
```
::::::::::::::::::::::::

Since `invariant_mass` is so simple, we _could_ leave the implementation inline in the
header, but most code separates the _interface_ from the _implementation_:

1. Users of the code are only interested in the interface, not the details of the implementation.
2. Compiled code _may_ be faster.

Start by providing a _declaration_ for `invariant_mass` in `invariant_mass.hpp`:

```cpp
//! \file invariant_mass.hpp
#pragma once // header guard

#include <cmath>
#include <stdexcept>

// declaration
double invariant_mass(double energy, double momentum);

// implementation (or "definition")
double invariant_mass(double energy, double momentum)
{
   if (energy < 0)
   {
      throw std::domain_error("unphysical negative energy");
   }
   double mass_squared = energy*energy - momentum*momentum;
   if (mass_squared < 0)
   {
      throw std::domain_error("unphysical mass^2");  
   }
   return std::sqrt(mass_squared);
}
```

Now create a file `src/invariant_mass.cpp` and move the _definition_ of `invariant_mass` into it:

```cpp
//! \file invariant_mass.cpp
// Our declaration
#include "invariant_mass.hpp"

#include <cmath>

// implementation (or "definition")
double invariant_mass(double energy, double momentum)
{
   if (energy < 0)
   {
      throw std::domain_error("unphysical negative energy");
   }
   double mass_squared = energy*energy - momentum*momentum;
   if (mass_squared < 0)
   {
      throw std::domain_error("unphysical mass^2");  
   }
   return std::sqrt(mass_squared);
}
```

We then clean up the header to:

```cpp
//! \file invariant_mass.hpp
#pragma once // header guard

#include <stdexcept>

double invariant_mass(double energy, double momentum);
```

We now need to tell the compiler to _also_ compile `invariant_mass.cpp` when it builds
`test_invariant_mass`:

:::::::::::::::: spoiler
### Linux

```bash
g++ -std=c++17 -I src/ src/invariant_mass.cpp test/test_invariant_mass.cpp -o test_invariant_mass    
./test_invariant_mass
```
::::::::::::::::::::::::

:::::::::::::::: spoiler
### MacOS

```bash
g++ -std=c++17 -I src/ src/invariant_mass.cpp test/test_invariant_mass.cpp -o test_invariant_mass    
./test_invariant_mass
```
::::::::::::::::::::::::


Overall, this isn't much different from what we already have, but we have
decoupled _what_ we test from _how_ we test it. The price of this has been a more
complex compilation command, which we will address in a later episode.


## C++ Design to Assist Unit Testing

- pure functions
- separation of concerns (e.g. don't mix I/O and math)
- avoid mega-functions/loops.
  - basic factoring.
- avoid global state

::::: challenge
```cpp
#include <iostream>
#include <fstream>
#include <cmath>
#include "invariant_mass.hpp"

double g_energy_scale = 1.0;

void process_candidates(const std::string& filename) {

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return;
    }

    int    n_candidates = 0;
    int    n_physical   = 0;
    double sum_mass     = 0.0;

    double energy, px, py, pz;
    while (file >> energy >> px >> py >> pz) {
        ++n_candidates;

        energy *= g_energy_scale;

        double momentum = std::sqrt(px*px + py*py + pz*pz);

        try {
            double mass = invariant_mass(energy, momentum);
            ++n_physical;
            sum_mass += mass;
            if (mass > 70.0 && mass < 110.0) {
                std::cout << "Z candidate found with mass "
                          << mass << " GeV" << std::endl;
            }
        } catch (const std::invalid_argument&) {
            std::cout << "Unphysical candidate, skipping." << std::endl;
        }
    }

    if (n_physical > 0) {
        std::cout << "Mean mass: " << sum_mass / n_physical
                  << " GeV" << std::endl;
    }
    std::cout << "Processed " << n_candidates << " candidates, "
              << n_physical   << " physical." << std::endl;
}
```


Part 1 — Identify the problems
"For each of the following properties, decide whether process_candidates() has it and explain in one sentence why it matters for testing:"

Does the function depend only on its explicit parameters?
Does it separate mathematical computation from file I/O and output?
Does it do one thing, or several?
Does it depend on any state defined outside the function?
Are all the values that control its behaviour visible in its signature?

Instructor notes / model answers
Does it depend only on its explicit parameters?

No. The result depends on g_energy_scale, which is not a parameter. A test cannot control or predict the output without also setting the global, and any other code that modifies the global between tests will silently change the result.
Does it separate computation from I/O?

No. File reading, arithmetic, and printing are all interleaved in the same loop. To test the mass calculation you must provide a real or carefully constructed file, and to check the result you must capture stdout — neither of which is straightforward.
Does it do one thing?

No. It reads a file, applies an energy correction, computes momenta, calls invariant_mass(), applies a mass window cut, accumulates statistics, and prints a summary. Each of these is a candidate for an independent unit.
Does it depend on external state?

Yes — g_energy_scale. See above.
Are all controlling values visible in the signature?

No. The mass window bounds 70.0 and 110.0 are hardcoded in the body. A test cannot vary them without editing the source, and a reader of the function signature has no indication they exist.

Part 2 — Consequences for testing
"For each problem you identified, describe a concrete testing difficulty it causes. Try to be specific: what test would you want to write, and why can you not write it cleanly against the current code?"
Model answers

Global state: "I want to test the effect of applying a scale factor of 1.1 to the energy. I cannot do this without setting g_energy_scale = 1.1 before the call and resetting it afterwards — and if two tests run concurrently, or another function modifies it, my test result is unreliable."
File I/O entangled with computation: "I want to test that a particle with energy 100 GeV and momentum 50 GeV produces a mass of approximately 86.6 GeV. To do this I must write those values to a temporary file, pass the filename to the function, and parse stdout to check the result. This is fragile, slow, and tests far more than the mass calculation."
Mega-function: "I want to test the Z candidate selection independently — specifically, that a mass of 69.9 GeV is not selected and 70.1 GeV is. There is no way to call just that logic; I must run the entire pipeline to exercise it."
Magic numbers: "I want to test the mass window boundary conditions. The values 70.0 and 110.0 are buried in the source — I cannot pass different values in a test without editing the code, which means I am testing a different program than the one in production."


Part 3 — Propose a restructuring
"Sketch a set of smaller functions that together reproduce the behaviour of process_candidates(), but where each part can be tested independently. Function signatures and a one-sentence description of what you would test for each are sufficient — you do not need to write the implementations."
Model answer

```cpp
cpp// Pure mathematical unit — already extracted in the lesson
// double invariant_mass(double energy, double momentum);

// Pure mathematical unit: magnitude of 3-momentum
// Test: momentum_magnitude(3.0, 4.0, 0.0) == 5.0 (Pythagorean triple)
// Test: momentum_magnitude(0.0, 0.0, 0.0) == 0.0
double momentum_magnitude(double px, double py, double pz);

// Pure function: apply a multiplicative scale to an energy value
// Test: apply_energy_scale(100.0, 1.1) == 110.0
// Test: apply_energy_scale(100.0, 1.0) == 100.0 (identity)
double apply_energy_scale(double energy, double scale);

// Pure function: test whether a mass falls within a window
// Test: is_z_candidate(91.2, 70.0, 110.0) == true
// Test: is_z_candidate(69.9, 70.0, 110.0) == false  (boundary)
// Test: is_z_candidate(110.0, 70.0, 110.0) == false (upper boundary exclusive?)
bool is_z_candidate(double mass, double mass_min, double mass_max);

// Operates on data already in memory; returns results as values not printout.
// energy_scale passed explicitly — no global state.
// Test: empty vectors return n_candidates == 0, n_physical == 0
// Test: one physical candidate returns correct mean mass
// Test: one unphysical candidate (E^2 < p^2) is counted but excluded from mean
struct CandidateSummary {
    int                 n_candidates;
    int                 n_physical;
    double              mean_mass;
    std::vector<double> z_candidate_masses;
};

CandidateSummary analyse_candidates(const std::vector<double>& energies,
                                    const std::vector<double>& px,
                                    const std::vector<double>& py,
                                    const std::vector<double>& pz,
                                    double energy_scale,
                                    double mass_min,
                                    double mass_max);

// I/O boundary: reads file, calls analyse_candidates, prints summary.
// Not directly unit tested — but now thin enough that there is little
// logic here to get wrong.
void process_candidates(const std::string& filename,
                        double energy_scale,
                        double mass_min,
                        double mass_max);
```

Points worth drawing out in discussion:

momentum_magnitude() is worth extracting even though it is a single line — it has a name, a clear contract, and can be tested with exact Pythagorean triples
is_z_candidate() makes the boundary conditions explicit and testable; students should notice the question mark in the comment about whether the upper bound is inclusive or exclusive, and recognise this as a specification decision that needs to be made and documented
analyse_candidates() now takes all its inputs as parameters and returns all its outputs as values — it can be tested without any files or output capture
process_candidates() still exists but is now just a thin I/O wrapper; the principle is not to eliminate I/O but to push it to the boundary

Part 4 — Preserving behaviour
"If you refactored process_candidates() into the functions you proposed, how would you verify that the refactoring did not change the behaviour of the program? What would you want to have in place before you started, and what would you check at each step?"
Model answer
Before starting: characterise the existing behaviour with at least one end-to-end check — run process_candidates() on a known input file and record the output. This becomes the reference to check against after each refactoring step.
During refactoring: extract one function at a time, keeping the overall program runnable after each extraction. Check after each step that the end-to-end output is unchanged.
After refactoring: the new unit tests for the extracted functions verify correctness at the unit level; the end-to-end check verifies that composition of the units produces the same overall behaviour as the original.
The honest observation to make here: if process_candidates() had no tests before the refactoring, you are in a difficult position. The end-to-end check helps, but it only covers the cases you thought to include in your reference file. This is why it is easier to write testable code from the start than to recover testability from legacy code — and why the lesson is teaching these habits now rather than after students have written ten thousand lines of analysis code.


"Look at what process_candidates() has become after the refactoring. We have unit tests for momentum_magnitude(), apply_energy_scale(), invariant_mass(), and is_z_candidate() — each in isolation. But we do not have a unit test for process_candidates() itself, and that is intentional. Its job is no longer to contain logic; it is to connect units together and talk to the file system. A test that calls process_candidates() with a real input file and checks the output is not a unit test — it is an integration test. It is checking that the units compose correctly, and that the I/O boundary works as expected. Both kinds of test are valuable, but they are answering different questions. Unit tests tell you which component is broken; integration tests tell you that the components work together. If your integration test fails but all your unit tests pass, the bug is almost certainly in the way the units are connected — which is a much smaller place to look."


::::: solution
::::::::::::::
:::::::::::::::


::::: challenge
```cpp
cpp#include <cmath>
#include <random>
#include "invariant_mass.h"

/* Estimate the invariant mass resolution by smearing true quantities
   with Gaussian detector resolution */
double estimate_mass_resolution(double true_energy,
                                double true_momentum,
                                double resolution = 0.05,
                                int    n_trials   = 10000) {

    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> smear(0.0, resolution);

    double sum_sq = 0.0;
    for (int i = 0; i < n_trials; ++i) {
        double smeared_energy   = true_energy   * (1.0 + smear(gen));
        double smeared_momentum = true_momentum * (1.0 + smear(gen));
        double mass = invariant_mass(smeared_energy, smeared_momentum);
        sum_sq += mass * mass;
    }
    return std::sqrt(sum_sq / n_trials);
}
```

Questions
"This function does not share the structural problems of process_candidates() — it takes all its inputs as parameters, does no I/O, and returns a value. But it still has a testability problem. Can you identify it?"
"What would happen if you wrote assert(estimate_mass_resolution(91.2, 0.0) == X) for some value X you computed by hand? Run the function twice and compare the results."
"How would you restructure the function so that a test could produce a reproducible result? What is the minimal change needed?"
"Even with that fix, what would your test actually be checking? Is that sufficient?"

Model answers
Identify the problem:

std::random_device seeds the Mersenne Twister from a hardware entropy source, so the sequence of random numbers is different on every call. Two calls to estimate_mass_resolution() with identical arguments will return different values. No fixed expected value exists to assert against.
What would happen with assert:

The assertion would pass or fail unpredictably depending on the random seed. Worse, it might pass nine times out of ten and fail occasionally — the hardest kind of bug to diagnose, because the failure is not reproducible.
Minimal fix — accept the generator as a parameter:

```cpp
double estimate_mass_resolution(double true_energy,
                                double true_momentum,
                                std::mt19937& gen,
                                double resolution = 0.05,
                                int    n_trials   = 10000);
```


A test can now pass a generator seeded with a fixed value and get a deterministic result:

```cpp
std::mt19937 gen(42);  // fixed seed
double result = estimate_mass_resolution(91.2, 0.0, gen);
// result is now the same on every run
```

The production caller constructs its generator however it likes — from std::random_device, from a run number, from a command-line argument — and passes it in. The function no longer makes that decision for its caller.
What does the test actually check:

With a fixed seed, the test checks that the function produces a specific numerical result for that seed. It does not check that the result is statistically correct — for that you would need to verify that the distribution of outputs over many seeds has the right mean and width, which is a different and harder kind of test. The honest answer is that testing stochastic functions thoroughly is genuinely difficult, and fixing the seed is a pragmatic first step that at least guarantees reproducibility. Students who notice this tension are thinking carefully — it is worth acknowledging rather than glossing over.

Closing observation for the instructor
The two snippets together make a useful point to draw out explicitly at the end of the exercise: process_candidates() and estimate_mass_resolution() have different kinds of testability problem, and the fix in each case is different. But both fixes follow the same underlying principle — make all dependencies explicit in the function signature. Global state, file handles, and random number generators are all the same kind of problem: hidden inputs that the function's caller cannot see or control. A function whose entire input is visible in its signature is a function you can reason about, test, and trust.


:::::::::::::::




::::::::::::::::::::::::::::::::::::: keypoints 

- Tests live in their own file and are compiled separately from the code under test
- A function is easy to test if it takes all its inputs as parameters and returns its output as a value
- Global state, side effects, hidden dependencies, and mixed concerns make functions harder to test and harder to reason about
- Writing testable code and writing maintainable code are largely the same discipline
- Refactoring untested code safely requires characterising its existing behaviour first — which requires tests you do not yet have
::::::::::::::::::::::::::::::::::::::::::::::::
