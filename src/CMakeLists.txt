cmake_minimum_required(VERSION 3.0)
project(qruov)
enable_language(C ASM)
set(CMAKE_C_STANDARD 99)
set(CMAKE_VERBOSE_MAKEFILE ON)

enable_testing()
find_package(OpenSSL)

set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# needed for nvim
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
if(CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES
          ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

set(ALLOWED_WARNINGS " -Wno-type-limits -DQRUOV_HASH_LEGACY ")
set(CMAKE_C_FLAGS_DEBUG "-march=native -mavx2 -mavx -DDEBUG -O0 -ggdb -Wall -Wextra -Wpedantic -fsanitize=address ${ALLOWED_WARNINGS}")
set(CMAKE_C_FLAGS_RELEASE "-DNDEBUG -O3 -Wall -Wextra -Wpedantic -mavx2 -mavx -march=native -mtune=native -ftree-vectorize -flto  -fomit-frame-pointer -fwrapv -fPIC -fPIE ${ALLOWED_WARNINGS}")


set(SOURCES
    ${PROJECT_SOURCE_DIR}/avx2/Fql.c
    ${PROJECT_SOURCE_DIR}/avx2/mgf.c
    ${PROJECT_SOURCE_DIR}/avx2/qruov.c
    ${PROJECT_SOURCE_DIR}/avx2/rng.c
    ${PROJECT_SOURCE_DIR}/avx2/sign.c
    ${PROJECT_SOURCE_DIR}/avx2/matrix.c
)

include_directories(avx)

set(TARGET_BINARY_NAME qruov_nist)
add_executable(${TARGET_BINARY_NAME} ${SOURCES} ${PROJECT_SOURCE_DIR}/avx2/PQCgenKAT_sign.c)
target_link_libraries(${TARGET_BINARY_NAME} crypto m)
set_property(TARGET ${TARGET_BINARY_NAME} APPEND PROPERTY COMPILE_FLAGS "-DQRUOV_q=127 -DQRUOV_L=3 -DQRUOV_v=156 -DQRUOV_m=54 -DQRUOV_fc=1 -DQRUOV_fe=1 -DQRUOV_fc0=1 -DQRUOV_PLATFORM=avx2a -DQRUOV_security_strength_category=1")
add_test(${TARGET_BINARY_NAME} ${TARGET_BINARY_NAME})


#foreach(category "252" "400" "548")
#  if(${category} EQUAL "252")
#    set(PARAM_TARGETS "192" "68" "45")
#  elseif(${category} EQUAL "400")
#    set(PARAM_TARGETS "220" "102")
#  elseif(${category} EQUAL "548")
#    set(PARAM_TARGETS "345" "137")
#  endif()
#
#  foreach(optimize_target ${PARAM_TARGETS})
#    # settings for benchmarking binary
#
#    # settings for unit tests binary
#    set(TARGET_BINARY_NAME LESS_test_cat_${category}_${optimize_target})
#    add_executable(${TARGET_BINARY_NAME} ${HEADERS} ${SOURCES} ${PROJECT_SOURCE_DIR}/lib/test/less_test.c)
#    set_property(TARGET ${TARGET_BINARY_NAME} APPEND PROPERTY COMPILE_FLAGS "-DCATEGORY=${category} -DTARGET=${optimize_target} ${KECCAK_EXTERNAL_ENABLE} ")
#    target_include_directories(${TARGET_BINARY_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/test)
#    add_test(${TARGET_BINARY_NAME} ${TARGET_BINARY_NAME})
#
#    # KATS generation
#    set(TARGET_BINARY_NAME LESS_nist_cat_${category}_${optimize_target})
#    add_executable(${TARGET_BINARY_NAME} ${HEADERS} ${SOURCES}  ${PROJECT_SOURCE_DIR}/lib/nist/KAT_NIST_rng.c ${PROJECT_SOURCE_DIR}/lib/nist/PQCgenKAT_sign.c)
#    target_include_directories(${TARGET_BINARY_NAME} PRIVATE ${OPENSSL_INCLUDE_DIR})
#    target_link_libraries(${TARGET_BINARY_NAME} PRIVATE OpenSSL::Crypto)
#    set_property(TARGET ${TARGET_BINARY_NAME} APPEND PROPERTY COMPILE_FLAGS "-DCATEGORY=${category} -DTARGET=${optimize_target} ${KECCAK_EXTERNAL_ENABLE} ")
#    add_test(${TARGET_BINARY_NAME} ${TARGET_BINARY_NAME})
#  endforeach(optimize_target)
#endforeach(category)
