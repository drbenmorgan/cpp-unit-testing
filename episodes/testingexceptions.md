---
title: "Testing exceptional behaviour"
teaching: 10 # teaching time in minutes
exercises: 2 # exercise time in minutes
---

:::::::::::::::::::::::::::::::::::::: questions 

- How do I verify that my code fails in the right way?
- What should happen when a function receives invalid input?

::::::::::::::::::::::::::::::::::::::::::::::::

::::::::::::::::::::::::::::::::::::: objectives
- Use EXPECT_THROW to assert that a specific exception type is raised
- Use EXPECT_NO_THROW to assert that valid input does not raise
- Write tests for the boundary conditions of invariant_mass(): negative energy, unphysical mass squared.
- Explain the difference between testing error handling and testing normal behaviour
- Explain why the choice of exception type matters and how to test for it specifically
::::::::::::::::::::::::::::::::::::::::::::::::


:::::::::::::::::::::::::::::::::::::: keypoints
- Testing what your code refuses to do is as important as testing what it does
- EXPECT_THROW checks both that an exception was raised and that it was the right type — the type is part of the contract
- Boundary conditions are where bugs most commonly live — zero, empty, equal, and just-outside-range inputs deserve explicit tests
- A function's error handling is part of its specification and should be documented and tested like any other behaviour
- With invariant_mass() now fully tested, we have seen the complete range of GoogleTest assertion types — the remaining episodes apply these tools to more complex code
::::::::::::::::::::::::::::::::::::::::::::::::
