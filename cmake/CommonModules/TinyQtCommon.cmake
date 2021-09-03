# Set common variables and create interface-only library target so all other targets
# will be able to link to, either directly or transitively, to consume common compile
# options/definitions
function(tiny_qt_common target alias)

    set(options EXPORT)
    set(single_args NAMESPACE NAME)
    cmake_parse_arguments(PARSE_ARGV 1 TINY "${options}" "${single_args}" "")

    add_library(${target} INTERFACE)
    add_library(${TINY_NAMESPACE}::${TINY_NAME} ALIAS ${target})

    if(TINY_EXPORT)
        set_target_properties(${target} PROPERTIES EXPORT_NAME ${TINY_NAME})
    endif()

    # Full C++ 20 support is required
    target_compile_features(${target} INTERFACE cxx_std_20)

    # Qt defines
    # ---

    target_compile_definitions(${target} INTERFACE
        # You can also make your code fail to compile if it uses deprecated APIs.
        # In order to do so, uncomment the following line.
        # You can also select to disable deprecated APIs only up to a certain version of Qt.
        # Disables all the APIs deprecated before Qt 6.0.0
        QT_DISABLE_DEPRECATED_BEFORE=0x060000

        #QT_NO_CAST_FROM_ASCII
        #QT_RESTRICTED_CAST_FROM_ASCII
        QT_NO_CAST_TO_ASCII
        QT_NO_CAST_FROM_BYTEARRAY
        QT_USE_QSTRINGBUILDER
        QT_STRICT_ITERATORS

        # Disable debug output in release mode
        $<$<NOT:$<CONFIG:Debug>>:QT_NO_DEBUG_OUTPUT>
    )

    # Platform specific configurations
    # ---

    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        target_compile_definitions(${target} INTERFACE
            # Windows 10 1903 "19H1" - 0x0A000007
            NTDDI_VERSION=0x0A000007
            # Windows 10 - 0x0A00
            _WIN32_WINNT=0x0A00
            _WIN32_IE=0x0A00
            UNICODE _UNICODE
            # Exclude unneeded header files
            WIN32_LEAN_AND_MEAN
            NOMINMAX
        )
    endif()

    # Compiler and Linker options
    # ---

    if(MSVC)
        target_compile_options(${target} INTERFACE
            # Suppress banner and info messages
            /nologo
            /guard:cf
            /utf-8
            # TODO This is by default for msvc c++20, solve when will do clang/gcc build silverqx
            /permissive-
            /bigobj
            # Has to be enabled explicitly
            # https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/
            /Zc:__cplusplus
            # Standards-conforming behavior
            /Zc:wchar_t,rvalueCast,inline,strictStrings
            /Zc:throwingNew,referenceBinding,ternary
#            /external:anglebrackets /external:W0 /external:templates-
#            /external:anglebrackets /external:W0
#            /Wall
#            /W4 /wd4127
            /WX /W3 /w34100 /w34189 /w44996 /w44456 /w44457 /w44458 /wd4577 /wd4467
        )

        target_link_options(${target} INTERFACE
            /guard:cf
            $<$<NOT:$<CONFIG:Debug>>:/OPT:REF,ICF=5>
            # /OPT:REF,ICF does not support incremental linking
            $<$<CONFIG:RelWithDebInfo>:/INCREMENTAL:NO>
            /WX
        )
    endif()

    # Is safer to provide this explicitly, qmake do it for msvc too
    # -fexceptions for linux is not needed, it is on by default
    if(MSVC)
        target_compile_options(${target} INTERFACE /EHsc)
    endif(MSVC)

    # TODO verify silverqx
    if(MINGW)
        target_link_options(${target} INTERFACE
            $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:LINKER:--dynamicbase>
        )
    endif()

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
            OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang"
            OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang"
    )
        target_compile_options(${target} INTERFACE
            -Wall
            -Wextra
            -Wcast-qual
            -Wcast-align
            -Woverloaded-virtual
            -Wold-style-cast
            -Wnon-virtual-dtor
            -Wshadow
            -Wundef
            -Wfloat-equal
            -Wformat-security
            -Wdouble-promotion
            -Wconversion
            -Wzero-as-null-pointer-constant
            -pedantic
            -pedantic-errors
            # Reduce I/O operations
            -pipe
        )

        # Clang 11 still doesn't support -Wstrict-null-sentinel
        include(CheckCXXCompilerFlag)
        check_cxx_compiler_flag(-Wstrict-null-sentinel SNS_SUPPORT)
        if(SNS_SUPPORT)
            target_compile_options(${target} INTERFACE -Wstrict-null-sentinel)
        endif()
    endif()

    # Use 64-bit off_t on 32-bit Linux, ensure 64bit offsets are used for filesystem
    # accesses for 32bit compilation
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_SIZEOF_VOID_P EQUAL 4)
      target_compile_definitions(${target} INTERFACE -D_FILE_OFFSET_BITS=64)
    endif()

endfunction()
