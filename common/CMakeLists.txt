set(TARGET common)

add_library(
    ${TARGET}
    STATIC
    assert.c
    print.c
)

target_include_directories(${TARGET} PUBLIC ${FREERTOS_INC})
