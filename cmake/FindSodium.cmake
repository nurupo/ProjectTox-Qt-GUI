# - Try to find Sodium
# Once done this will define
#  SODIUM_FOUND - System has Sodium
#  SODIUM_INCLUDE_DIRS - The Sodium include directories
#  SODIUM_LIBRARIES - The libraries needed to use Sodium

find_path(SODIUM_INCLUDE_DIR sodium.h)

find_library(SODIUM_LIBRARY NAMES sodium libsodium)

set(SODIUM_LIBRARIES ${SODIUM_LIBRARY} )
set(SODIUM_INCLUDE_DIRS ${SODIUM_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set SODIUM_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Sodium  DEFAULT_MSG
                                  SODIUM_LIBRARY SODIUM_INCLUDE_DIR)

mark_as_advanced(SODIUM_INCLUDE_DIR SODIUM_LIBRARY )
