add_library(sources)

target_sources(sources PRIVATE Game.cpp)

target_link_libraries(sources PUBLIC dependencies)
