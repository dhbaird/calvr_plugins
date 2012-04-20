FIND_PATH(CALVR_DIR include/cvrKernel/CalVR.h
  PATHS
  $ENV{CALVR_HOME}
  NO_DEFAULT_PATH
)

FIND_PATH(CALVR_INCLUDE_DIR cvrKernel/CalVR.h
  PATHS
  ${CALVR_DIR}
  NO_DEFAULT_PATH
    PATH_SUFFIXES include
)

IF(WIN32)

FIND_LIBRARY(CALVR_KERNEL_LIBRARY 
  NAMES CalVRAll
  PATHS
  ${CALVR_DIR}
  NO_DEFAULT_PATH
  PATH_SUFFIXES lib64 lib
)

ELSE(WIN32)

FIND_LIBRARY(CALVR_COLLABORATIVE_LIBRARY 
  NAMES cvrCollaborative
  PATHS
  ${CALVR_DIR}
  NO_DEFAULT_PATH
  PATH_SUFFIXES lib64 lib
)

FIND_LIBRARY(CALVR_CONFIG_LIBRARY 
  NAMES cvrConfig
  PATHS
  ${CALVR_DIR}
  NO_DEFAULT_PATH
  PATH_SUFFIXES lib64 lib
)

FIND_LIBRARY(CALVR_INPUT_LIBRARY 
  NAMES cvrInput
  PATHS
  ${CALVR_DIR}
  NO_DEFAULT_PATH
  PATH_SUFFIXES lib64 lib
)

FIND_LIBRARY(CALVR_KERNEL_LIBRARY 
  NAMES cvrKernel
  PATHS
  ${CALVR_DIR}
  NO_DEFAULT_PATH
  PATH_SUFFIXES lib64 lib
)

FIND_LIBRARY(CALVR_MENU_LIBRARY 
  NAMES cvrMenu
  PATHS
  ${CALVR_DIR}
  NO_DEFAULT_PATH
  PATH_SUFFIXES lib64 lib
)

FIND_LIBRARY(CALVR_UTIL_LIBRARY 
  NAMES cvrUtil
  PATHS
  ${CALVR_DIR}
  NO_DEFAULT_PATH
  PATH_SUFFIXES lib64 lib
)

MARK_AS_ADVANCED(CALVR_COLLABORATIVE_LIBRARY)
MARK_AS_ADVANCED(CALVR_CONFIG_LIBRARY)
MARK_AS_ADVANCED(CALVR_INPUT_LIBRARY)
MARK_AS_ADVANCED(CALVR_KERNEL_LIBRARY)
MARK_AS_ADVANCED(CALVR_MENU_LIBRARY)
MARK_AS_ADVANCED(CALVR_UTIL_LIBRARY)

ENDIF(WIN32)

SET(CALVR_FOUND "NO")
IF(CALVR_INCLUDE_DIR AND CALVR_KERNEL_LIBRARY)
    SET(CALVR_FOUND "YES")
ENDIF(CALVR_INCLUDE_DIR AND CALVR_KERNEL_LIBRARY)
