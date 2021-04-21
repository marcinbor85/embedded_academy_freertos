project("embedded_academy_freertos" C) 

cmake_minimum_required(VERSION 3.0)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 -O0 -g")

set(OSAL_PORT freertos)
set(FREERTOS_MEMMANG heap_3)
set(FREERTOS_PORT ThirdParty/GCC/Posix)

add_subdirectory(FreeRTOS-Kernel)
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
add_subdirectory(09-message_buffers)
add_subdirectory(10-mutex_semphr)
add_subdirectory(11-deadlock_mutex)
add_subdirectory(12-trace_task)
