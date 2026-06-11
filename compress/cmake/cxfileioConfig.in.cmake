@PACKAGE_INIT@

include(CMakeFindDependencyMacro)

# Find dependencies
find_dependency(Boost 1.71 REQUIRED COMPONENTS iostreams)
find_dependency(ROOT)

# Include the targets
include("${CMAKE_CURRENT_LIST_DIR}/cxfileioTargets.cmake")

check_required_components(cxfileio)
