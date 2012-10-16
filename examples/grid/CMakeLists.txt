ADD_EXECUTABLE( providers "providers.cc" ${COMMON_HEADERS} )
TARGET_LINK_LIBRARIES( providers "dunegrid" "dunecommon"  ${CCGNU_LIBRARIES} "boost_filesystem" "boost_system")
