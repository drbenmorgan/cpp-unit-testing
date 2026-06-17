---
title: "Introducing GoogleTest"
teaching: 10 # teaching time in minutes
exercises: 2 # exercise time in minutes
---
:::::::::::::::::::::::::::::::::::::: questions 

- How do I write a unit test using GoogleTest?

::::::::::::::::::::::::::::::::::::::::::::::::

::::::::::::::::::::::::::::::::::::: objectives

- Write a simple unit test using GoogleTest

::::::::::::::::::::::::::::::::::::::::::::::::

1. Pixi setup
2. Add `find_package` for GTest
3. Write test
4. Build/integrate with CTest

Writing tests clearly involves a lot of boilerplate coding. Since it's such a common
need there are a lot of C++ packages out there that provide all of this for us.
We’ve chosen the [GoogleTest] unit testing framework for this course because of its general
wide use (and author familiarity), but others are available, e.g.

- [Catch2] is probably the most popular after GoogleTest
- [Boost.Test](https://www.boost.org/library/latest/test/) is part of the widely used, but
  heavyweight, Boost libraries
- [doctest](https://github.com/doctest/doctest) lightweight, header only, allows "tests alongside
  code" model used in languages like Rust

All have similar concepts, so adapting what you learn here to projects that use a
different framework is generally just a dialect/terminology difference.


::::::::::::::::::::::::::::::::::::: keypoints 

- Always use an existing testing framework to reduce complexity and maintenance 
- A good framework like GoogleTest makes it easy to write and structure tests

::::::::::::::::::::::::::::::::::::::::::::::::
