SET(LIB_NAME ElevatorRoom)
SET(PLUGIN_HEADERS
    ElevatorRoom.h
)

ADD_LIBRARY(${LIB_NAME}
    "SHARED"
    ${PLUGIN_HEADERS}
    ElevatorRoom.cpp
)

FIND_PACKAGE(OASCLIENT)

INCLUDE_DIRECTORIES(${OSG_INCLUDE_DIR}
			${OASCLIENT_INCLUDE_DIR})
TARGET_LINK_LIBRARIES(${LIB_NAME} ${OSG_LIBRARIES}
			${OASCLIENT_LIBRARY})

ADD_CALVR_LIBRARIES(${LIB_NAME})

INSTALL(TARGETS ${LIB_NAME} DESTINATION lib/plugins)
