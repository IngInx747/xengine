# Locate root dir
SET(ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}")

# Locate libraries dir
IF (CMAKE_CL_64)
	SET(LIBS "${ROOT_DIR}/lib/assimp-vc140-mt.lib")
ELSE (CMAKE_CL_64)
	SET(LIBS "${ROOT_DIR}/lib/assimp-vc140-mt.lib")
ENDIF (CMAKE_CL_64)

# Handle the QUIETLY and REQUIRED arguments and set XXX_FOUND to TRUE if all listed variables are TRUE.
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
	assimp DEFAULT_MSG
	ROOT_DIR
	LIBS)

SET(ASSIMP_LIBS ${LIBS})
SET(ASSIMP_INCLUDE_DIRS ${ROOT_DIR})

MARK_AS_ADVANCED(
    ASSIMP_LIBS
    ASSIMP_INCLUDE_DIRS)
