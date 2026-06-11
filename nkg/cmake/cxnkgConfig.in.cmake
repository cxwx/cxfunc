@PACKAGE_INIT@

include(CMakeFindDependencyMacro)

# Find ROOT dependency
find_dependency(ROOT)

# Include the targets
include("${CMAKE_CURRENT_LIST_DIR}/cxnkgTargets.cmake")

check_required_components(cxnkg)
