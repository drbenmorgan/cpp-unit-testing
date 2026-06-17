#!/usr/bin/env Rscript

# Build zip archives for lesson starter templates in episodes/files.
# Each immediate subdirectory becomes <name>.zip alongside it.

source_dir <- file.path("episodes", "files")

if (!dir.exists(source_dir)) {
  stop("Template source directory not found: ", source_dir, call. = FALSE)
}

if (!requireNamespace("zip", quietly = TRUE)) {
  stop(
    "Package 'zip' is required. Install it with install.packages('zip').",
    call. = FALSE
  )
}

source_dir_abs <- normalizePath(source_dir, mustWork = TRUE)

template_dirs <- list.dirs(source_dir, recursive = FALSE, full.names = TRUE)

if (length(template_dirs) == 0) {
  message("No template directories found in ", source_dir)
  quit(save = "no", status = 0)
}

for (template_dir in template_dirs) {
  template_name <- basename(template_dir)
  zip_path <- file.path(source_dir_abs, paste0(template_name, ".zip"))

  if (file.exists(zip_path)) {
    unlink(zip_path)
  }

  zip::zipr(
    zipfile = zip_path,
    files = template_name,
    root = source_dir_abs
  )

  message("Created ", zip_path)
}
