---
title: "Code coverage"
teaching: 10 # teaching time in minutes
exercises: 2 # exercise time in minutes
---
:::::::::::::::::::::::::::::::::::::: questions 

- How do I know which parts of my code my tests actually exercise?
- What does test coverage tell me, and what doesn't it tell me?

::::::::::::::::::::::::::::::::::::::::::::::::

::::::::::::::::::::::::::::::::::::: objectives

- Build a project with `gcov/lcov` instrumentation .
- Run gcovr and interpret line and branch coverage reports
- Identify at least one untested branch in the Histogram implementation from the coverage report
- Write a test that increases branch coverage and verify the improvement in the report

- Explain the difference between line coverage and branch coverage
- Describe at least one class of bug that 100% line coverage would not catch

::::::::::::::::::::::::::::::::::::::::::::::::

## Test coverage

We've been adding tests for `histogram` and as this has progressed you might have been
asking "how do I know I've tested everything?".

The good news is that there are tools that can measure the **coverage** of our tests:
which lines of the code we are testing were _actually_ executed by tests.

As you might guess, this requires instrumenting the compiled code and we therefore need
to compile with flags that enable this. We should also turn optimization off, because this
process can throw off line matching. Both of these motivate the simplest way to enable
coverage in our tests: a separate CMake _build type_ like the `Release` and `Debug` we saw earlier
in which we can exactly specify the flags we want to use. Open up your `CMakeLists.txt` and all
we need to do here is add one line:

```cmake
# 2. C++ Standard setup
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 2a. Coverage Build Type flags for C++ 
set(CMAKE_CXX_FLAGS_COVERAGE "-O0 --coverage")
```

::::: callout
CMake organises compiler flags for C++ and each build type in the following _variables_:

- `CMAKE_CXX_FLAGS`: always applied compiler flags.
- `CMAKE_CXX_FLAGS_<TYPE>`: additional flags to `CMAKE_CXX_FLAGS` used in build type `<TYPE>`.

There are many other ways to set this up, but these are beyond the scope of this lesson.
:::::::::::::

We've explicitly turned off optimization (`-O0`) and enabled instrumentation for coverage
with `--coverage`. We can now configure, build and test using our new `Coverage` build type like as we did for `Release`:

```bash
(ccptepp-test) $ cmake -GNinja -DCMAKE_BUILD_TYPE=Coverage -S . -B build-coverage
```

::::: callout
Note that the argument to `CMAKE_BUILD_TYPE` is case-insensitive! We could equally have used
`coverage` or `CoVeRaGe`.
:::::::::::::

```bash
(ccptepp-test) $ cmake --build build-coverage
...
[1/4] /usr/bin/c++  -I/tmp/ccptepp-test/src -O0 --coverage -std=c++17 -arch arm64 -MD -MT CMakeFiles/ccptepp.dir/src/invariant_mass.cpp.o -MF CMakeFiles/ccptepp.dir/src/invariant_mass.cpp.o.d -o CMakeFiles/ccptepp.dir/src/invariant_mass.cpp.o -c /tmp/ccptepp-test/src/invariant_mass.cpp
...

(ccptepp-test) $ ctest --test-dir build-coverage

```

So far, so much the same, so what has changed? Compiling with coverage and then running the tests 
has actually generated two extra files per `.cpp` file. We can find these in `build-coverage` with:

```bash
(ccptepp-test) $ find build-coverage -name "*.gc*"
build-coverage/CMakeFiles/test_invariant_mass.dir/test/test_invariant_mass.cpp.gcno
build-coverage/CMakeFiles/test_invariant_mass.dir/test/test_invariant_mass.cpp.gcda
build-coverage/CMakeFiles/ccptepp.dir/src/invariant_mass.cpp.gcno
build-coverage/CMakeFiles/ccptepp.dir/src/invariant_mass.cpp.gcda
```

The `.gcno` files are output by the compiler when building, and the `.gcda` files when the programs
actually run. These contain all the information we need, but are in binary format so we need a
tool to help us make sense of them.


## Using `gcovr` to generate coverage reports

We're going to use the [`gcovr`](https://gcovr.com/en/stable/index.html) tool to help us
make sense of the coverage outputs as it's the simplest and easiest to use. It's preinstalled
in our Pixi development environment so we don't need any further setup to use it.

To produce a report, [`gcovr`] needs to know where to find both the `.cpp` files and the `.gcda` 
outputs generated for these. If we are running from `ccptepp-test/` as we've been doing so far,
we can thus run `gcovr` as:

```bash
(ccptepp-test) $ gcovr --root src/ build-coverage/
```

::::: callout
- `--root` tells `gcovr` where to look for source files.
- the locations for `.gcda` files are passed as additional paths at the end.

It will recurse into these directories, so we don't need to provide multiple nested paths.
:::::::::::::

By default, `gcovr` just produces text output to the terminal, so we'll see something like

```bash
(ccptepp-test) $ gcovr --root src/ build-coverage/


```

So we get some details, even which lines are missed.

Whilst complete, we can visualize this a bit more easily by getting `gcovr` to output
an HTML report:

```bash
(ccptepp-test) $ gcovr --root src/ build-coverage/ --html-details build-coverage/coverage.html
(INFO) Reading coverage data...
(INFO) Writing coverage report...
```

You can now open `build-coverage/coverage.html` in your browser of choice, and you should
see:

Let's focus on the 



::::::::::::::::::::::::::::::::::::: keypoints 

- Coverage measures which lines and branches were executed during testing — not whether they were tested correctly
- A line shown as covered means it ran; it does not mean the result was checked or that the test would catch a bug there
- Branch coverage is more informative than line coverage — a line can execute without all its branches being taken
- Coverage is a lower bound on thoroughness, not an upper bound — 100% coverage is necessary but not sufficient
- The coverage report is most useful as a guide to where tests are missing, not as a measure of test quality
- Beware of diminishing returns

::::::::::::::::::::::::::::::::::::::::::::::::
