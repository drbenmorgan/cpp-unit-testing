# The Carpentries Workbench Template Markdown Lesson

This lesson is a template lesson that uses [The Carpentries Workbench][workbench]. 

## Local development with renv

This repository uses a project-local `renv` environment so lesson build packages
do not leak into your global R library. `.Rprofile` automatically restores the
project library from `renv.lock` on startup, following standard `renv` practice.

### Local preview (Jekyll-like)

From the project root:

```sh
R -e 'sandpaper::serve()'
```

Packages are automatically restored on startup. The first time you run this in a
fresh checkout, it may take a moment to install dependencies.

**Useful alternatives:**

```sh
R -e 'sandpaper::check_lesson()'
R -e 'sandpaper::build_lesson()'
```

**Package cache setup (optional):** To enable the Carpentries lesson package cache
workflow (reproducible builds across machines), run once:

```sh
R -e 'sandpaper::use_package_cache()'
```

## Note about lesson life cycle stage
Although the `config.yaml` states the life cycle stage as pre-alpha, **the template is stable and ready to use**. The life cycle stage is preset to `"pre-alpha"` as this setting is appropriate for new lessons initialised using the template.


1. **Update this README with 
   [relevant information about your lesson](https://carpentries.github.io/lesson-development-training/collaborating-newcomers.html#readme)**
   and delete this section.

[cff-home]: https://citation-file-format.github.io/
[cff-sandpaper-docs]:  https://carpentries.github.io/sandpaper-docs/editing.html#making-your-lesson-citable
[cffinit]: https://citation-file-format.github.io/cff-initializer-javascript/
[workbench]: https://carpentries.github.io/sandpaper-docs/
