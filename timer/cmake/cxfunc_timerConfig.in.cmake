@PACKAGE_INIT@

include(CMakeFindDependencyMacro)

# Find optional dependencies for examples
find_dependency(ROOT QUIET)
find_dependency(Geant4 QUIET)
find_dependency(spdlog QUIET)

# Include the targets
include("${CMAKE_CURRENT_LIST_DIR}/cxfunc_timerTargets.cmake")

check_required_components(cxfunc_timer)
