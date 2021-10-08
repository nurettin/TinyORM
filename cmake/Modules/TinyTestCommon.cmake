# Configure passed auto test
function(tiny_configure_test name)

    set(options INCLUDE_MODELS)
    cmake_parse_arguments(PARSE_ARGV 1 TINY "${options}" "" "")

    target_precompile_headers(${name} PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_SOURCE_DIR}/include/pch.h">
    )

    if(NOT CMAKE_DISABLE_PRECOMPILE_HEADERS)
        target_compile_definitions(${name} PRIVATE USING_PCH)
    endif()

    set_target_properties(${name}
        PROPERTIES
            C_VISIBILITY_PRESET "hidden"
            CXX_VISIBILITY_PRESET "hidden"
            VISIBILITY_INLINES_HIDDEN YES
    )

    # Setup correct PATH env. variable, used by ctest command, needed to find TinyUtils
    # and TinyOrm libraries in the build tree
    set_property(TEST ${name} APPEND PROPERTY
        ENVIRONMENT "PATH=${TINY_TESTS_ENV}"
    )

    if(MINGW)
        target_compile_options(${name}
            PRIVATE
                $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:DEBUG>>:
                    -Wa,-mbig-obj
                    # Avoid string table overflow
                    -O1>
        )
    endif()

    target_compile_definitions(${name}
        PRIVATE
            PROJECT_TINYORM_TEST
            TINYORM_TESTS_CODE
    )

    target_include_directories(${name} PRIVATE
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
    )

    if(TINY_INCLUDE_MODELS)
        target_include_directories(${name} PRIVATE
            "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/tests/models>"
        )
    endif()

    target_link_libraries(${name}
        PRIVATE
            Qt${QT_VERSION_MAJOR}::Core
            Qt${QT_VERSION_MAJOR}::Sql
            Qt${QT_VERSION_MAJOR}::Test
            ${TinyOrm_ns}::${TinyUtils_target}
            ${TinyOrm_ns}::${TinyOrm_target}
    )

endfunction()
