project("embedded_academy_freertos" C) 

cmake_minimum_required(VERSION 3.0)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 -O0 -g")

set(FREERTOS_DIR ${CMAKE_SOURCE_DIR}/FreeRTOS-Kernel)

set(
    FREERTOS_INC
    ${FREERTOS_DIR}/include
    ${FREERTOS_DIR}/portable/ThirdParty/GCC/Posix
    ${FREERTOS_DIR}/portable/ThirdParty/GCC/Posix/utils
)

set(
    FREERTOS_SRC
    ${FREERTOS_DIR}/tasks.c
    ${FREERTOS_DIR}/timers.c
    ${FREERTOS_DIR}/stream_buffer.c
    ${FREERTOS_DIR}/queue.c
    ${FREERTOS_DIR}/list.c
    ${FREERTOS_DIR}/event_groups.c
    ${FREERTOS_DIR}/croutine.c
    ${FREERTOS_DIR}/portable/MemMang/heap_3.c
    ${FREERTOS_DIR}/portable/ThirdParty/GCC/Posix/port.c    
    ${FREERTOS_DIR}/portable/ThirdParty/GCC/Posix/utils/wait_for_event.c    
)

add_subdirectory(common)
add_subdirectory(osal)

function(add_example)

    set(TARGET ${ARGV0})

    add_executable(
        ${TARGET}
    )

    target_sources(
        ${TARGET}
        PRIVATE
        ${FREERTOS_SRC}
        ${COMMON_SRC}
        ${OSAL_SRC}
        main.c
    )

    target_include_directories(
        ${TARGET}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${COMMON_INC}
        ${FREERTOS_INC}
        ${OSAL_INC}
    )

    target_link_libraries(${TARGET} PUBLIC pthread)

endfunction()

add_subdirectory(01-hello_freertos)
add_subdirectory(02-delay_until)
add_subdirectory(03-timers_task)
add_subdirectory(04-system_malloc)
add_subdirectory(05-polling_events)
add_subdirectory(06-shared_data)
add_subdirectory(07-osal_basics)
add_subdirectory(08-queue_pointers)
