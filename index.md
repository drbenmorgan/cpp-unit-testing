---
site: sandpaper::sandpaper_site
---

In science, we rely on establishing the behaviour and accuracy of experimental devices before
making actual measurements, for example through calibration. Equally, theoretical calculations
make predictions that inform experiment design and data analysis decisions, and thus we also rely
on establishing the correctness of these calculations. The same is true of the software we
use to simulate, take, and analyse the data collected through experiments - it is an theoretical 
calculation or experimental device just as much as any other. We must therefore throughly _test_ our software's accuracy  and reliability in order that the scientific results we derive from its use are not compromised.

In this lesson, we introduce methods and tooling for **unit testing** C++ code. Our "units"
here are the lower level components of the code, specifically individual functions and/or classes.
By defining _test cases_, _test suites_ and _test fixtures_, we'll see how we can integrate testing
as a standard part of the software development workflow. The [GoogleTest](https://google.github.io/googletest/) framework will be introduced to build these elements, a commonly used testing system
in both science and industry that saves us a lot of otherwise boilerplate coding. We'll finish by
looking at how _unit_ testing provides a foundation for higher level _integration_ and _regression_ 
testing, and other useful tooling for aspects unit testing cannot catch.
