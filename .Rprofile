source("renv/activate.R")

local({
	repos <- getOption("repos")
	repos[["carpentries"]] <- "https://carpentries.r-universe.dev"

	if (is.null(repos[["CRAN"]]) || identical(repos[["CRAN"]], "@CRAN@")) {
		repos[["CRAN"]] <- "https://packagemanager.posit.co/cran/latest"
	}

	options(repos = repos)
})

# Automatically restore project library from lockfile on startup
# (equivalent to 'bundle install' in Ruby Bundler)
# Skip in non-interactive CI environments to avoid redundant restores
if (Sys.getenv("CI") == "") {
  renv::restore(prompt = FALSE)
}
