---
title: "Integrating tests into a build system"
teaching: 10 # teaching time in minutes
exercises: 2 # exercise time in minutes
---

:::::::::::::::::::::::::::::::::::::: questions 

- How do build and run my tests automatically?
- How does a build system benefit testing as a project grows?

::::::::::::::::::::::::::::::::::::::::::::::::

::::::::::::::::::::::::::::::::::::: objectives

- Understand the friction of manual compilation as the number of test files grows.
- Write a `CMakeLists.txt` that builds a test executable and registers it with CTest.
- Run tests using `ctest` with `-V` and `--output-on-failure` to analyse test failure outputs.
- Understand the limitation of `assert()` in release builds.
- Explain why automating the build and run of tests reduces the barrier to running them easiely and frequently.

::::::::::::::::::::::::::::::::::::::::::::::::

## Why automate test build and running?

We've naturally used a very simple code to begin learning about unit testing, but
practical projects will be composed of many functions and classes (our units), each
of which will have its own unit test program. Even with our simple code, the compilation
command is already quite complex, and different on different platforms:

```bash
# ... or clang++ ...
g++ -std=c++17 -I src/ src/invariant_mass.cpp test/test_invariant_mass.cpp -o test_invariant_mass 
```

Imagine that we add more functions and these start to use (i.e. _depend_ on) each other, and we
have test programs for each of these. Our current manual "compile the test program, run it" won't
scale here, and is also mistake prone. We could easily forget to recompile something that we are
testing, or something that what we are testing depends on - the tests would then still pass but 
this wouldn't be testing the _current_ state of the code. Furthermore, the barrier to building
and running tests is high, even for ourselves, and we want testing to be _frequently run_ (ideally after every recompile!) and thus it needs to be _easy to build and run_.

This is where a good **buildsystem** can help us. These are essentially _workflow managers_ for the
specific task of "configuring, building (i.e. compiling), and testing software". We specify the 
_workflow_ in terms of _what_ we want to build and run in a script, and the buildsystem works out
the details of compiler configuration and _dependencies_ for us. We've essentially been
doing this scripting and workflow manually already:

1. Use the flag `-std=c++17` on every compile
2. Use `-I src/` to declare the location of the `invariant_mass.hpp` header.
2. Recompile `test_invariant_mass` from `test_invariant_mass.cpp` `invariant_mass.hpp` and `invariant_mass.cpp` when ever one or more of these files changes.
3. Run `test_invariant_mass` and confirm it runs successfully.

Buildsystems help us make this process automated, portable, and most importantly _reproducible_, 
as their scripts become part of our codebase and thus version control (e.g. Git).

## Introducing CMake and CTest

Whilst there are many buildsystems out there, [CMake](https://cmake.org) has become
the primary go-to system for C++ software (it can also compile C, Fortran, CUDA and HIP).
CMake is actually a _metabuildsystem_ in that it doesn't actually implement the full
workflow management itself, but _generates_ scripts for [existing tools like Make, Ninja,
Xcode and Visual Studio](https://cmake.org/cmake/help/v4.2/manual/cmake-generators.7.html). We won't need to worry about this in this lesson, as the
`cmake` program will take care on running these tools for us. 

::::: prereq
The exercises in this episode require the `pixi` package which you installed in the setup.
::::::::::::::::::

From now on, we'll be working in a _development environment_ setup for us by the `pixi`
tool. This will ensure all of the software we need for the remainder of the episodes is 
present (except for the C++ compiler, which we take from the system) and setup for immediate use.
To do this, make sure you're in the `ccptepp-test/` directory and run:

```bash
$ pixi shell
```

::::: callout
For clarity, we will now _always_ prefix terminal commands with the `$` prompt to distinguish these
from outputs. You don't need to type the `$`! Your terminal may look different depending on what you
use for the prompt.
:::::::::::::

This should drop you into a shell with the development environment setup with a prefix to the 
prompt to distinguish it from the base environment:

```bash
(ccptepp-test) $
```

::::: callout
- You can exit this environment at any time by typing `exit`. 
- You can re-enter it at any point by running `pixi shell` again, but remember you need to be in the `ccptepp-test/` directory to do this! 
:::::::::::::

Let's check we have `cmake` available:

```bash
(ccptepp-test) $ cmake --version
```

we should get

```bash
cmake version 4.2.0

CMake suite maintained and supported by Kitware (kitware.com/cmake).
```

Like all good programs, you can get help on running CMake either directly on the command line
with:

```bash
(ccptepp-test) $ cmake --help
```

or from its [comprehensive documentation](https://cmake.org/cmake/help/v4.2/).


## Building `test_invariant_mass` with CMake

To build `test_invariant_mass` with CMake, we need to write a `CMakeLists.txt` script
to tell CMake how to do this. Open the file `CMakeLists.txt` in `ccptepp-test`
and add the following lines:

```cmake
# - CMake setup
cmake_minimum_required(VERSION 3.26...4.2)
project(CCPTEPPTest)

# - C++ Standard setup
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# - Build a library
add_library(ccptepp src/invariant_mass.cpp)
target_include_directories(ccptepp PUBLIC src/)

# - Build test_invariant_mass
add_executable(test_invariant_mass test/test_invariant_mass.cpp)
target_link_libraries(test_invariant_mass ccptepp)
```

::::: checklist
### Key points about this file
1. The file is named `CMakeLists.txt` with capital `C` and `L`, plural `s`, and the `.txt` extension.
2. Comments in CMake scripts begin with a `#`.
3. Relative paths like `src/invariant_mass.cpp` are relative the directory
   of the `CMakeLists.txt` file.
4. CMake scripting is command-based, and [full documentation on all commands is available](https://cmake.org/cmake/help/v4.2/manual/cmake-commands.7.html)
:::::::::::::::

The first two lines are doing the main heavy lifting: first to configure CMake to support the
range of versions we specify, second to set up internal variables and check we having working
C/C++ compilers available. If the CMake we run with is _less_ than the minimum version we
specify, we will get an error. The maximum version is just an indication that "we haven't tried
versions beyond this yet" (CMake is generally good with backward compatibility).

The `CMAKE_CXX_...` are _variables_, in this case that tell CMake how to configure the
C++ compiler so that it uses the C++17 standard throughout, that the compiler _must_ support this standard, and that it should not use any compiler extensions to the language. CMake
variables are defined and manipulated with the `set()` command, and _reserved_ variables used
by CMake are [listed in its documentation](https://cmake.org/cmake/help/v4.2/manual/cmake-variables.7.html).

We then move on to the actual build, starting by building a _library_ for `invariant_mass`:

- `add_library()` declares a _library_ called `ccptepp` and lists the sources to build it from.
  - Building a library can be thought of as the _binary_ companion to the _source_ division
    we did to `invariant_mass` and `test_invariant_mass`.
  - It means we compile `invariant_mass.cpp` only once, with any code needing `invariant_mass` only
    needing to link to the _library_.
- `target_include_directories()` is CMake's equivalent to the `-I` flag we used when manually compiling.
   - It is simply declaring to CMake that _"any compilation of files for `ccptepp` needs to have the following paths added as `-I` flags"_.
   - The `PUBLIC` qualifier means that any compilation/link operation that _uses_ `ccptepp` should
     also have these same flags used.

We then complete the build of `test_invariant_mass`:

- `add_executable()` declares a _program_ called `test_invariant_mass` and lists the sources to build it from.
- `target_link_libraries()` declares that `test_invariant_mass` _links to_ the `ccptepp` library.
  - This ensures that compilation finds the `invariant_mass.hpp` header, and the final executable
    will have the binary code for the `invariant_mass` function.

To actually get CMake to build `test_invariant_mass` for us we first need to _configure_
the project. This is done by running:

```bash
(ccptepp-test) $ cmake -G Ninja -S . -B build
```

Here we use `-G` to specify the _buildsystem backend_ we want use. We've chosen the
[Ninja](https://ninja-build.org) tool here as it's generally much faster than others like [Make](https://en.wikipedia.org/wiki/Make_(software)). It's provided in the `pixi` environment for you.
We also specify the _source directory_ (where the `CMakeLists.txt` for the project is) 
with `-S`, and the _build directory_ (where we want CMake to output everything) with `-B`.
As we're running in `ccptepp-test/` we can use the current directory for `-S`. A dedicated,
separate build directory is used so we don't mix up _source code_ from _binary/generated code_.

::::: callout
- Having isolated build directories is general good practice as it mitigates the risk of comitting
binary/generated files to your VCS.
- Of course, a full project should also implement a full `.gitignore` file too!
:::::::::::::

On running we should get output similar too

```bash
(ccptepp-test) $ cmake -G Ninja -S . -B build
-- The C compiler identification is AppleClang 17.0.0.17000604
-- The CXX compiler identification is AppleClang 17.0.0.17000604
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /usr/bin/cc - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done (0.8s)
-- Generating done (0.0s)
-- Build files have been written to: /tmp/ccptepp-test/build
```

Of course, your compiler identification and where the build files are written will differ,
but you shouldn't see any warnings or errors. All that CMake has done at this step is
_generate_ the scripts needed to do the build, not the build itself. To do that, run:


```bash
(ccptepp-test) $ cmake --build ./build --verbose
```

The `--verbose` flag has been added here so we can see the full output:

```bash
Change Dir: '/tmp/ccptepp-test/build'

Run Build Command(s): /tmp/ccptepp-test/.pixi/envs/default/bin/ninja -v
[1/4] /usr/bin/c++  -I/tmp/ccptepp-test/src -std=c++17 -arch arm64 -MD -MT CMakeFiles/ccptepp.dir/src/invariant_mass.cpp.o -MF CMakeFiles/ccptepp.dir/src/invariant_mass.cpp.o.d -o CMakeFiles/ccptepp.dir/src/invariant_mass.cpp.o -c /tmp/ccptepp-test/src/invariant_mass.cpp
[2/4] : && /tmp/ccptepp-test/.pixi/envs/default/bin/cmake -E rm -f libccptepp.a && /usr/bin/ar qc libccptepp.a  CMakeFiles/ccptepp.dir/src/invariant_mass.cpp.o && /usr/bin/ranlib libccptepp.a && /tmp/ccptepp-test/.pixi/envs/default/bin/cmake -E touch libccptepp.a && :
[3/4] /usr/bin/c++  -I/tmp/ccptepp-test/src -std=c++17 -arch arm64 -MD -MT CMakeFiles/test_invariant_mass.dir/test/test_invariant_mass.cpp.o -MF CMakeFiles/test_invariant_mass.dir/test/test_invariant_mass.cpp.o.d -o CMakeFiles/test_invariant_mass.dir/test/test_invariant_mass.cpp.o -c /tmp/ccptepp-test/test/test_invariant_mass.cpp
[4/4] : && /usr/bin/c++ -arch arm64 -Wl,-search_paths_first -Wl,-headerpad_max_install_names CMakeFiles/test_invariant_mass.dir/test/test_invariant_mass.cpp.o -o test_invariant_mass  libccptepp.a && :
```

which shows that `test_invariant_mass` has been compiled using the right flags and should be present
at `build/test_invariant_mass`. CMake has essentially replicated what we were doing manually, but
we have now written it down clearly in a script that will replicate it.

::::: callout

You generally don't need to run with `--verbose` unless you have to debug issues. We're showing
the output here for academic interest, and even without verbosity CMake/Ninja
will always output warning/error messages for compile/link problems.

:::::::::::::

::::: challenge

1. Check that you can indeed run `build/test_invariant_mass` as you did before.
2. Try running `cmake --build ./build --verbose` again. What do you notice?
3. Add one blank line to `test/test_invariant_mass.cpp` and run `cmake --build ./build --verbose` again. What do you see this time?
4. Repeat 3, but this time add a blank line somewhere in `src/invariant_mass.cpp` and rebuild. What do you see this time?

::::: solution
1. It should run fine - at least it should pass/fail as you left it from the last episode!
2. You should see the output `ninja: no work to do.`. Buildsystems won't needlessly recompile
   if none of the inputs (_dependencies_) have changed.
3. You should see that it recompiles _only_ `test_invariant_mass.cpp`:

   ```bash
   Change Dir: '/Users/benmorgan/tmp/pix/ccptepp-test/build'

   Run Build Command(s): /tmp/ccptepp-test/.pixi/envs/default/bin/ninja -v
   [1/2] /usr/bin/c++  -I/tmp/ccptepp-test/src -std=c++17 -arch arm64 -MD -MT CMakeFiles/test_invariant_mass.dir/test/test_invariant_mass.cpp.o -MF CMakeFiles/test_invariant_mass.dir/test/test_invariant_mass.cpp.o.d -o CMakeFiles/test_invariant_mass.dir/test/test_invariant_mass.cpp.o -c /tmp/ccptepp-test/test/test_invariant_mass.cpp
   [2/2] : && /usr/bin/c++ -arch arm64 -Wl,-search_paths_first -Wl,-headerpad_max_install_names CMakeFiles/test_invariant_mass.dir/test/test_invariant_mass.cpp.o -o test_invariant_mass  libccptepp.a && :
   ```

   It hasn't had to recompile the library because nothing changed there.

4. You should see that it recompiles _only_ `invariant_mass.cpp`, but recreates the library and
   _relinks_ it to `test_invariant_mass`

   ```bash
   Change Dir: '/tmp/ccptepp-test/build'

   Run Build Command(s): /tmp/ccptepp-test/.pixi/envs/default/bin/ninja -v
   [1/3] /usr/bin/c++  -I/tmp/ccptepp-test/src -std=c++17 -arch arm64 -MD -MT CMakeFiles/ccptepp.dir/src/invariant_mass.cpp.o -MF CMakeFiles/ccptepp.dir/src/invariant_mass.cpp.o.d -o CMakeFiles/ccptepp.dir/src/invariant_mass.cpp.o -c /tmp/ccptepp-test/src/invariant_mass.cpp
   [2/3] : && /tmp/ccptepp-test/.pixi/envs/default/bin/cmake -E rm -f libccptepp.a && /usr/bin/ar qc libccptepp.a  CMakeFiles/ccptepp.dir/src/invariant_mass.cpp.o && /usr/bin/ranlib libccptepp.a && /tmp/ccptepp-test/.pixi/envs/default/bin/cmake -E touch libccptepp.a && :
   [3/3] : && /usr/bin/c++ -arch arm64 -Wl,-search_paths_first -Wl,-headerpad_max_install_names CMakeFiles/test_invariant_mass.dir/test/test_invariant_mass.cpp.o -o test_invariant_mass  libccptepp.a && :
   ```

   Thus if we make a change to the code were _testing_, CMake is ensuring that the rebuild
   updates the _program that tests it_ (strictly "depends on it") automatically.


Use of CMake might have seemed overkill for our case, but you can see that it's actually doing
a lot more checks and balances that our manual approach is not capable of. Plus, we no
longer have to worry about whether we're running on macOS, Linux, or any other system.
::::::::::::::
:::::::::::::::


## Running `test_invariant_mass` with CTest

We've seen we have `test_invariant_mass` available to run directly. For one test that's
simple enough, we could continue to run it manually, but as a project grows with multiple tests, we want to automate this:

- so we don't forget to run them ourselves,
- so others can run them easily.

CMake comes with scripting commands and a dedicated program, `ctest`, that provide this
capability so we don't need to write our own scripts here. We can support for CTest and automatic running very simply to our `CMakeLists.txt`:

```cmake
# ...

# - Build test_invariant_mass
add_executable(test_invariant_mass test/test_invariant_mass.cpp)
target_link_libraries(test_invariant_mass ccptepp)

# - Setup CTest
enable_testing()

# - Declare tests
add_test(NAME TestInvariantMass COMMAND test_invariant_mass)
```

::::: checklist
### Key points about these commands
1. The [`enable_testing()`](https://cmake.org/cmake/help/v4.2/command/enable_testing.html) command sets up CMake to generate scripts for CTest to run.
2. The [`add_test()`](https://cmake.org/cmake/help/v4.2/command/add_test.html#command:add_test) command declares a test to CMake/CTest
:::::::::::::::

The `COMMAND` argument in `add_test` is "what to run", and note CMake is being quite clever here.
We are actually telling it to _"run the executable that corresponds to the **target** named `test_invariant_mass` declared elsewhere"_. Here our _target_ name is exactly the same as the
resulting _executable_, but this isn't always the case (e.g. Windows might use the `.exe` extension).
By using target names, we don't have to worry about this detail or where, exactly, the executable was
output to on disk.

The `NAME` argument is just a label to identify the test in CTest's outputs. It's not just the
command name, as we might have the case that we run the same test executable in more than one
way, e.g.

```cmake
add_test(NAME TestLowEnergy  COMMAND test_beam --lowenergy)
add_test(NAME TestHighEnergy COMMAND test_beam --highenergy)
```

::::: callout
This shows that `COMMAND` is basically written like any terminal command, so your tests
can take command line arguments if needed.
:::::::::::::

We _could_ now run `cmake` again to configure, but as we have already done that once, all we need
to do is run

```bash
(ccptepp-test) $ cmake --build ./build
[0/1] Re-running CMake...
-- Configuring done (0.1s)
-- Generating done (0.0s)
-- Build files have been written to: /tmp/ccptepp-test/build
ninja: no work to do.
```

CMake builds _dependencies on its own inputs into the workflow_ just as it does for C++ files,
so you don't need to start from scratch reconfiguring everytime - simply rebuild! However, we do
still need to run the test, and for this we have to switch to use the `ctest` program.

::::: callout
CMake doesn't natively provide a `--test` argument like `--build` for some reason!
:::::::::::::

We run this very much like `cmake`:

```bash
(ccptepp-test) $ ctest --test-dir ./build
```

Here we use `--test-dir` to tell CTest where to find the tests it should run. As we left `test_invariant_mass` failing from the last episode, we should see output:

```bash
Test project /tmp/ccptepp-test/build
    Start 1: TestInvariantMass
1/1 Test #1: TestInvariantMass ................Subprocess aborted***Exception:   0.25 sec

0% tests passed, 1 tests failed out of 1

Total Test time (real) =   0.26 sec

The following tests FAILED:
	  1 - TestInvariantMass (Subprocess aborted)
Errors while running CTest
Output from these tests are in: /tmp/ccptepp-test/build/Testing/Temporary/LastTest.log
Use "--rerun-failed --output-on-failure" to re-run the failed cases verbosely.
```

Here the benefit of having test programs that return a non-zero exit code to indicate failure
comes in - this enables CTest to detect that a failure has happened!
However, by default CTest _does not_ report any _output_ created by either failing or passing tests.
That might not seem helpful, but many projects have hundreds of unit test programs, so seeing
a high level overview of passes/failures as the default is not unreasonable.

::::: challenge
## Getting more information from CTest

1. Run `ctest -V --test-dir ./build` and compare the output to our initial run
2. Run `ctest --output-on-failure --test-dir ./build` and compare the output to `-V`

Which of the three verbosities (none, `-V`, and `--output-on-failure` do you think is
most useful for general development work?

::::: solution

Usually `--output-on-failure` is the best compromise as you obviously _hope_ that tests pass,
so you won't get any output _unless_ something fails. The normal use case for `-V` is debugging
tests or CTest itself, for example you've written a test case you expect to fail, but it isn't.
It's generally too verbose in other situations.

In more advanced work, `--output-on-failure` is great for continuous integration systems like
GitHub Actions so that outputs from failing tests appear in your logs without the clutter of `-V`.
::::::::::::::
:::::::::::::::


## Build modes and testing with `assert()`

So far we've been building everything without any _optimization_ or other compiler flags.
We might want to check whether our tests pass at the higher optimization levels we'll use
in production, and CMake helps us here by defining build "types":

- `None` (Empty): default, no optimization
- `Debug`: instruments code for debugging in tools like `gdb`
- `RelWithDebInfo`: instruments code for debugging plus moderate optimization.
- `Release`: no debugging instrumentation, high optimization.

To activate these, we can configuring a fresh build using `CMAKE_BUILD_TYPE` to specify the
one we want:

```bash
(ccptepp-test) $ cmake -GNinja -DCMAKE_BUILD_TYPE=Debug -S . -B build-debug
```

We create a separate build directory for this because the code will be compiled differently.
Building is no different to before, but we can see the extra flags applied if we run with `--verbose`:

```bash
(ccptepp-test) $ cmake --build build-debug --verbose
...
[1/4] /usr/bin/c++  -I/tmp/ccptepp-test/src -g -std=c++17 -arch arm64 -MD -MT CMakeFiles/ccptepp.dir/src/invariant_mass.cpp.o -MF CMakeFiles/ccptepp.dir/src/invariant_mass.cpp.o.d -o CMakeFiles/ccptepp.dir/src/invariant_mass.cpp.o -c /tmp/ccptepp-test/src/invariant_mass.cpp
...
```

Note that `-g` has been added here - the flag to enable debugging instrumentation.

Running tests is also the same, and we should still see our failure:

```bash
(ccptepp-test) $ ctest --test-dir ./build-debug
...
Test project /tmp/ccptepp-test/build-debug
    Start 1: TestInvariantMass
1/1 Test #1: TestInvariantMass ................Subprocess aborted***Exception:   0.24 sec
...
```

::::: challenge

## Testing in `Release` builds

Try repeating the above exercise of configuring, building and running tests for a `Release` build.

1. What flags do you see added?
2. What do you notice about the test, and can you explain what is happening?

::::: solution

1. We should see that release adds `-O3 -DNDEBUG`

   ```bash
   (ccptepp-test) $ cmake -GNinja -DCMAKE_BUILD_TYPE=Release -S . -B build-release
   ...
   (ccptepp-test) $ cmake --build build-release --verbose
   ...
   [1/4] /usr/bin/c++  -I/tmp/ccptepp-test/src -O3 -DNDEBUG -std=c++17 -arch arm64 -MD -MT CMakeFiles/ccptepp.dir/src/invariant_mass.cpp.o -MF CMakeFiles/ccptepp.dir/src/invariant_mass.cpp.o.d -o CMakeFiles/ccptepp.dir/src/invariant_mass.cpp.o -c /tmp/ccptepp-test/src/invariant_mass.cpp
   ...
   ```

   The `-O3` is the highest optimization level (out of 0, 1, 2, and 3). What about `-DNDEBUG` though?

2. We'll find that the test actually _passes_:

   ```bash
   (ccptepp-test) $ ctest --test-dir build-release
   Test project /tmp/ccptepp-test/build-release
       Start 1: TestInvariantMass
   1/1 Test #1: TestInvariantMass ................   Passed    0.24 sec

   100% tests passed, 0 tests failed out of 1

   Total Test time (real) =   0.25 sec
   ```

   The key is the `-DNDEBUG` flag we saw in the solution to part 1. As documented on
   the [cppreference for `assert`](https://en.cppreference.com/cpp/error/assert):

   > If `NDEBUG` is defined as a macro name at the point in the source code where `<cassert>` or `<assert.h>` is included, the assertion is disabled: assert does nothing.

   This is not a disaster for use of `assert`, but we need to be aware of this when
   using it to test, or as a tool for defensive programming.

::::::::::::::
:::::::::::::::

## What have we gained?
This might have seemed like a long episode for not much gain, but we've actually simplified
_building and running our tests_ quite a bit. Whether we are on Linux, macOS or something else
all we now need to do in our development and testing workflow is:

1. Run `cmake -GNinja -S. -B build` one to set things up.
2. Run `cmake --build build` to compile everything.
3. Run `ctest --test-dir test` to test everything.
4. Edit/modify code.
5. Goto 2.


::::::::::::::::::::::::::::::::::::: keypoints 

- A build system like CMake ensures tests are always compiled against the current code 
  before they are run.
- CTest is a test runner — it does not care how tests are written, only whether the executable exits cleanly
- Tests you have to run manually are tests you will forget to run — automation removes that risk
- Keeping the barrier to running tests low is as important as writing the tests themselves
- assert() is disabled when NDEBUG is defined — in a CMake release build your entire test suite silently disappears

::::::::::::::::::::::::::::::::::::::::::::::::
