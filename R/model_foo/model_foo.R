## Do we need to explicitly 
setup <- function() {
  # do some stuff like compile hooks, install additional packages etc. 
}

params <- function() {
  # all values should have defaults
  # orchestration script will only pass keys that are defined here.
  return(list(
    FOO_BAR=123,
    MODE="PROD",
    DRY_RUN = FALSE
  ))
}

generate_sim_dimensions <- function() {
  tbl <- tibble::tribble(~Country, ~Market, ~Date,
                         "USA", "EQ", "2021",
                         "CAD", "FX", "2022")
  return(tbl)
}

run <- function(params, country, market, simdate) {
  record_results()
  return(c(params, country, market, simdate))
}
