
setup <- function() {
  # do some stuff like compile hooks, install additional packages etc. 
}

generate_sim_dimensions <- function() {
  tbl <- tibble::tribble(~Country, ~Market, ~Date, "USA", "EQ", "2021", "CAD", "FX", "2022")
  return(tbl)
}

run <- function(country, market, simdate) {
  
}
