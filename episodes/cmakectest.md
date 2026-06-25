---
title: "Integrating unit tests in development workflows"
teaching: 10 # teaching time in minutes
exercises: 2 # exercise time in minutes
---

:::::::::::::::::::::::::::::::::::::: questions 

- How do I automate the build and run of my tests?

::::::::::::::::::::::::::::::::::::::::::::::::

::::::::::::::::::::::::::::::::::::: objectives

- Use CMake and CTest to build and run `test_invariant_mass`.

::::::::::::::::::::::::::::::::::::::::::::::::

## Why automate test build and running?

We've naturally used a very simple code to begin learning about unit testing, but
practical projects will be composed of many functions and classes (our units), each
of which will have its own unit test program. Our current manual "compile the test
program, run it" won't scale as we move to more complex tasks, and this is where
a good _buildsystem_ can help us. These are essentially _workflow managers_ for the
specific task of "configuring, compiling, and testing software". We specify the _workflow_
in terms of _what_ we want to build and run in a script, and the buildsystem works out
the details of compiler configuration and _dependencies_ for us. We've essentially been
doing this worflow manually already:

1. Use the flag `-std=c++17` on every compile
2. Recompile `test_invariant_mass` using `test_invariant_mass.cpp` when ever this file changes.
3. Run `test_invariant_mass` and confirm it runs successfully.

Buildsystems help us make this process automated and most importantly _reproducible_,
as their scripts become part of our codebase.

## Introducing CMake and CTest

Whilst there are many buildsystems out there, [CMake](https://cmake.org) has become
the primary go-to system for C++ software (it can also compile C, Fortran, CUDA and HIP).
CMake is actually a _metabuildsystem_ in that it doesn't actually implement the full
workflow management, but rather generates scripts for existing tools like Make, Ninja,
Xcode and Visual Studio. We won't need to worry about this in this lesson, and the
`cmake` program will take care on running these tools for us. 

::::: prereq
The exercises in this episode require the `pixi` package which you installed in the setup.
::::::::::::::::::

From now on, we'll be working in a _development environment_ setup for us by the `pixi`
tool. This will ensure all of the software we need is present (except for the C++ compiler,
which we take from the system) and setup for immediate use. To do this, make sure you're in
the `ccptepp-test/` directory and run:

```bash
$ pixi shell
```

::::: callout
For clarity, we will now _always_ prefix terminal commands with the `$` prompt to distinguish these
from outputs. You don't need to type the `$`! Your terminal may look different depending on what you
use for the prompt.
:::::::::::::

This should drop you into a shell with the development environment setup with a name to distinguish this:

```bash
(ccptepp-test) $
```

::::: callout
You can exit this environment at any time by typing `exit`. You can re-enter it at any point by
running `pixi shell` again, but remember you need to be in the `ccptepp-test/` directory to do this! 
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

## Building `test_invariant_mass` with CMake

To build `test_invariant_mass` with CMake, we need to write a `CMakeLists.txt` script
that will tell CMake how to do this. Open the file `CMakeLists.txt` in `ccptepp-test`
and add the following lines:

```cmake
# 1. CMake setup
cmake_minimum_required(VERSION 3.26...4.2)
project(CCPTEPPTest)

# 2. C++ Standard setup
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 3. Build test_invariant_mass from the source file
add_executable(test_invariant_mass test/test_invariant_mass.cpp)
```

::::: checklist
### Key points about this file
1. The file is named `CMakeLists.txt` with capital `C` and `L` and the `.txt` extension.
2. Comments in CMake scripts begin with a `#`.
3. Relative paths like `test/test_invariant_mass.cpp` are relative the directory
   of the `CMakeLists.txt` file.
4. CMake scripting is command-based, and [full documentation is available](https://cmake.org/cmake/help/v4.2/manual/cmake-commands.7.html)
:::::::::::::::

The first two lines are doing the main heavy lifting: first to configure CMake to support the
range of versions we specify, second to set up internal variables and check we having working
C/C++ compilers available. If the CMake we run with is _less_ than the minimum version we
specify, we will get an error. The maximum version is just an indication that "we haven't tried
versions beyond this yet" (CMake is generally good with backward compatibility).

The `CMAKE_CXX_...` are _variables_ that tell CMake how to configure the C++ compiler so
that it uses the C++17 standard throughout, that the compiler _must_ support this standard,
and that it should not use any compiler extensions to the language.

Finally, we actually declare the build of the `test_invariant_mass` program from its source file.

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
Having isolated build directories is general good practice as it mitigates the risk of comitting
binary/generated files to your VCS. Of course, a full project should also implement a full
`.gitignore` file too!
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
[1/2] /usr/bin/c++   -std=c++17 -arch arm64 -MD -MT CMakeFiles/test_invariant_mass.dir/test/test_invariant_mass.cpp.o -MF CMakeFiles/test_invariant_mass.dir/test/test_invariant_mass.cpp.o.d -o CMakeFiles/test_invariant_mass.dir/test/test_invariant_mass.cpp.o -c /tmp/ccptepp-test/test/test_invariant_mass.cpp
[2/2] : && /usr/bin/c++ -arch arm64 -Wl,-search_paths_first -Wl,-headerpad_max_install_names CMakeFiles/test_invariant_mass.dir/test/test_invariant_mass.cpp.o -o test_invariant_mass   && :
```

which shows that `test_invariant_mass` has been compiled using the right flags and should be present
at `build/test_invariant_mass`.

::::: challenge

1. Check that you can indeed run `build/test_invariant_mass` as you did before.
2. Try running `cmake --build ./build` again. What do you notice?

::::: solution
1. It should run fine - at least it should pass/fail as you left it from the last episode!
2. You should see the output `ninja: no work to do.`. Buildsystems won't needlessly recompile
   if none of the inputs (_dependencies_) have changed.
::::::::::::::
:::::::::::::::


## Running `test_invariant_mass` with CTest

CMake provides scripting commands and a dedicated program, `ctest`, that can help us
automate running tests after we've built them. We can add this very simply to our
`CMakeLists.txt`:

```cmake
# ...

# 3. Build test_invariant_mass from the source file
add_executable(test_invariant_mass test/test_invariant_mass.cpp)

# 4. Setup CTest
enable_testing()

# 5. Declare tests
add_test(NAME TestInvariantMass COMMAND test_invariant_mass)
```

::::: checklist
### Key points about these commands
1. The [`enable_testing()`](https://cmake.org/cmake/help/v4.2/command/enable_testing.html) command sets up CMake to generate scripts for CTest to run.
2. The `add_test(https://cmake.org/cmake/help/v4.2/command/add_test.html#command:add_test)` command declares a test to CMake/CTest
:::::::::::::::

The `COMMAND` argument in `add_test` is "what to run", and note CMake is being quite clever here.
We are actually telling it to _"run the executable that corresponds to the **target** named `test_invariant_mass` declared elsewhere"_. Here our _target_ name is exactly the same as the
resulting _executable_, but this isn't always the case (e.g. Windows might use the `.exe` extension).
By using target names, we don't have to worry about this detail or where, exactly, the executable was
output to on disk.

We _could_ now run `cmake` again to configure, but as we have already done that once, all we need
to do now is run

```bash
(ccptepp-test) $ cmake --build ./build
[0/1] Re-running CMake...
-- Configuring done (0.1s)
-- Generating done (0.0s)
-- Build files have been written to: /tmp/ccptepp-test/build
ninja: no work to do.
```

CMake builds _dependencies on its own inputs_ into the workflow, so you don't need to start
from scratch reconfiguring everytime - simply rebuild! However, we do still need to run the test, and for this we switch to use the `ctest` program.

::::: callout
CMake doesn't natively provide a `--test` argument like build for some reason! We'll solve this
awkwardness with Pixi later.
:::::::::::::

We run this very much like `cmake`:

```bash
(ccptepp-test) $ ctest --test-dir ./build
```

Here we use `--test-dir` to tell CTest where to find the tests it should run. As we have `test_invariant_mass` failing from the last episode, we should see output:

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

By default, CTest _does not_ report any output created by either failing or passing tests.
That might not seem helpful, but many projects have hundreds of unit test programs, so seeing
a high level overview of passes/failures as the default is not unreasonable.

::::: challenge
1. Run `ctest -V --test-dir ./build` and compare the output to our initial run
2. Run `ctest --output-on-failure --test-dir ./build` and compare the output to `-V`

Which of the three verbosities do you think is most useful for general development work?

::::: solution

Usually `--output-on-failure` is the best compromise as you obviously _hope_ that tests pass,
so you won't get any output _unless_ something fails. The normal use case for `-V` is debugging
tests, for example you've written a test case you expect to fail, but it isn't. It's generally
too verbose in other situations.

In more advanced work, `--output-on-failure` is great for continuous integration systems like
GitHub Actions so that failure reports appear in your logs.

::::::::::::::
:::::::::::::::


## Using Pixi as a helper

The advantages of CMake/CTest might not seem obvious at this point 


::::::::::::::::::::::::::::::::::::: keypoints 

- Automate the build and run of your tests within your development workflows
- CMake and CTest provide a simple system to do this for C++ across all platforms

::::::::::::::::::::::::::::::::::::::::::::::::
