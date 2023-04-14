target_sources(tests PRIVATE Level.cpp)
target_link_libraries(tests test_dependencies sources)
target_include_directories(tests PRIVATE ../src)
