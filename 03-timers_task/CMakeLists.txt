set(TARGET 03-timers_task)

add_executable(
    ${TARGET}
    ${FREERTOS_SRC}
    main.c
    FreeRTOSConfig.h
)

target_include_directories(
    ${TARGET}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${TARGET} PUBLIC common pthread)
target_include_directories(common PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
