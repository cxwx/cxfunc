@PACKAGE_INIT@

include(CMakeFindDependencyMacro)

# Find ROOT dependency
find_dependency(ROOT)

# Include the targets
include("${CMAKE_CURRENT_LIST_DIR}/cxfunc_pauseTargets.cmake")

check_required_components(cxfunc_pause)
