add_static_variants(SOURCE grid_walker.cc
                    INIFILE grid_walker.ini
                    BASENAME statictest
                    CREATED_TARGETS testtools_targets
                    ${DEBUG_MACRO_TESTS})
# append generated targets to stuff internal list for link magic etc
set(testnames ${testnames} ${testtools_targets})
