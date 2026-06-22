---
title: Setup
---

## Software installation

These instructions set out how to obtain and install the software on Linux or macOS. It is
assumed that you have:

- access to the Bash or Zsh shell on a fairly modern Linux or macOS system
- a text editor such as Vim or Emacs, or an IDE such as VSCode
- sufficient disk space (~250MB) to store the software and outputs

You **do not** need root/administrator access.

<!-- ## Data Sets -->
<!--
FIXME: place any data you want learners to use in `episodes/data` and then use
       a relative link ( [data zip file](data/lesson-data.zip) ) to provide a
       link to it, replacing the example.com link.
-->
<!--Download the [data zip file](https://example.com/FIXME) and unzip it to your Desktop -->

### C++ Compiler

As we will be coding in C++ (and the C++17 standard), we will need a compiler.
We'll take this from the system for reliability, so it can be installed through
the appropriate package management system.

:::::::::::::::: spoiler

### Linux

Install GCC through your system's package manager.

::::::::::::::::::::::::

:::::::::::::::: spoiler

### MacOS

Install Xcode from the App Store.

::::::::::::::::::::::::

:::::::::::::::: spoiler

### Windows

This lesson has not been designed to run on Windows. We would recommend using the [Windows Subsystem for Linux](https://learn.microsoft.com/en-us/windows/wsl/), and following the instructions for Linux
in the rest of the lesson.

::::::::::::::::::::::::


### Pixi

As we move through the course, we will need additional tools to help us write,
build, and run our tests efficiently. To do this, we will use the [Pixi] package manager to provide an isolated development environment with
the software we need.


:::::::::::::::: spoiler

### Linux

Open a terminal and run:

```bash
curl -fsSL https://pixi.sh/install.sh | sh
```

or, if you don't have `curl` available:

```bash
wget -qO- https://pixi.sh/install.sh | sh
```

More details on what these do are on [Pixi's installation page](https://pixi.prefix.dev/latest/installation/).

::::::::::::::::::::::::

:::::::::::::::: spoiler

### MacOS

If you have [Homebrew](https://brew.sh) installed, then it's recommended to install Pixi
with this. Open a terminal and run:

```bash
brew install pixi
```

Otherwise, you should install Pixi directly:

```bash
curl -fsSL https://pixi.sh/install.sh | sh
```

More details on what these do are on [Pixi's installation page](https://pixi.prefix.dev/latest/installation/).

::::::::::::::::::::::::

::::::::::::::::::::::::::::::::::::::: discussion

### Why Pixi?

If you're familiar with installing software, you might be asking why we're using Pixi,
another package manager, on top of the system ones (or similar tools like Homebrew).
The answer is in two parts:

1. Consistency: we can lock in the versions of the tools used to avoid issues in the exercises
2. Simplified workflow: We'll use Pixi's [tasks](https://pixi.prefix.dev/latest/workspace/advanced_tasks/) to simplify the configure/build/test workflow so we can concentrate on
the testing concepts rather the tooling.

It is also very similar in concept to (and uses packages from) Conda which you may be
familiar with already.

:::::::::::::::::::::::::::::::::::::::::::::::::::

## Software Setup

Open a terminal and check that you have the following commands available:

:::::::::::::::: spoiler

### Linux

```bash
g++ --version
```

and

```bash
pixi --version
```

::::::::::::::::::::::::

:::::::::::::::: spoiler

### MacOS

```bash
clang++ --version
```

and

```bash
pixi --version
```
::::::::::::::::::::::::


## Starter Project

Download the [project zip file](files/ccptepp-test.zip) and unzip it somewhere suitable.
More detailed setup information will be provided in each exercise as needed, though in all
cases we will be working in the directory `ccptepp-test/` which the zipfile unzips to.
