include(ExternalProject)

set(LIBEVENT_VERSION "2.1.8")
set(THRIFT_VERSION "0.11.0")
set(ZLIB_VERSION "1.2.11")
set(OPENSSL_VERSION "1.1.1-pre7")
set(CURL_VERSION "7.60.0")
set(CATCH_VERSION "2.2.1")

# Prefer static to dynamic libraries
set(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

string(TOUPPER "${CMAKE_BUILD_TYPE}" UPPERCASE_BUILD_TYPE)
set(EXTERNAL_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_${UPPERCASE_BUILD_TYPE}}")
set(EXTERNAL_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_${UPPERCASE_BUILD_TYPE}}")

string(STRIP ${EXTERNAL_CXX_FLAGS} EXTERNAL_CXX_FLAGS)
string(STRIP ${EXTERNAL_C_FLAGS} EXTERNAL_C_FLAGS)

message(STATUS "External CXX flags: ${EXTERNAL_CXX_FLAGS}")
message(STATUS "External C flags: ${EXTERNAL_C_FLAGS}")

set(OPENSSL_CXX_FLAGS "${EXTERNAL_CXX_FLAGS}")
set(OPENSSL_C_FLAGS "${EXTERNAL_C_FLAGS}")
set(OPENSSL_PREFIX "${PROJECT_BINARY_DIR}/external/openssl")
set(OPENSSL_INCLUDE_DIR "${OPENSSL_PREFIX}/include")
set(OPENSSL_STATIC_LIB_NAME "${CMAKE_STATIC_LIBRARY_PREFIX}ssl")
set(CRYPTO_STATIC_LIB_NAME "${CMAKE_STATIC_LIBRARY_PREFIX}crypto")
set(OPENSSL_LIBRARIES "${OPENSSL_PREFIX}/lib/${OPENSSL_STATIC_LIB_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}"
        "${OPENSSL_PREFIX}/lib/${CRYPTO_STATIC_LIB_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}")
ExternalProject_Add(openssl_ep
        URL https://www.openssl.org/source/openssl-${OPENSSL_VERSION}.tar.gz
        BUILD_IN_SOURCE 1
        CONFIGURE_COMMAND ./config -fPIC --prefix=${OPENSSL_PREFIX} no-comp no-shared no-tests CXX=${CMAKE_CXX_COMPILER} CC=${CMAKE_C_COMPILER}
        BUILD_COMMAND "$(MAKE)"
        INSTALL_COMMAND "$(MAKE)" install
        LOG_DOWNLOAD ON
        LOG_CONFIGURE ON
        LOG_BUILD ON
        LOG_INSTALL ON)

include_directories(SYSTEM ${OPENSSL_INCLUDE_DIR})
message(STATUS "OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
message(STATUS "OpenSSL static libraries: ${OPENSSL_LIBRARIES}")

install(FILES ${OPENSSL_LIBRARIES} DESTINATION lib)
install(DIRECTORY ${OPENSSL_INCLUDE_DIR}/openssl DESTINATION include)

set(LIBCUCKOO_CXX_FLAGS "${EXTERNAL_CXX_FLAGS}")
set(LIBCUCKOO_C_FLAGS "${EXTERNAL_C_FLAGS}")
set(LIBCUCKOO_PREFIX "${PROJECT_BINARY_DIR}/external/libcuckoo")
set(LIBCUCKOO_CMAKE_ARGS "-Wno-dev"
        "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
        "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}"
        "-DCMAKE_CXX_FLAGS=${LIBCUCKOO_CXX_FLAGS}"
        "-DCMAKE_C_FLAGS=${LIBCUCKOO_C_FLAGS}"
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        "-DCMAKE_INSTALL_PREFIX=${LIBCUCKOO_PREFIX}"
        "-DBUILD_EXAMPLES=OFF"
        "-DBUILD_TESTS=OFF"
        "-DBUILD_SHARED_LIBS=OFF")
set(LIBCUCKOO_INCLUDE_DIR "${LIBCUCKOO_PREFIX}/include")
set(LIBCUCKOO_STATIC_LIB_NAME "${CMAKE_STATIC_LIBRARY_PREFIX}libcuckoo")
set(LIBCUCKOO_LIBRARY "${LIBCUCKOO_PREFIX}/lib/${LIBCUCKOO_STATIC_LIB_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}")
set(LIBCUCKOO_LIBRARIES ${LIBCUCKOO_LIBRARY})

ExternalProject_Add(libcuckoo
        GIT_REPOSITORY https://github.com/efficient/libcuckoo.git
        GIT_TAG 8785773896d74f72b6224e59d37f5f8c3c1e022a
        GIT_SUBMODULES
        LIST_SEPARATOR |
        CMAKE_ARGS ${LIBCUCKOO_CMAKE_ARGS}
        LOG_DOWNLOAD ON
        LOG_CONFIGURE ON
        LOG_BUILD ON
        LOG_INSTALL ON)

include_directories(SYSTEM ${LIBCUCKOO_INCLUDE_DIR})
message(STATUS "Libcuckoo include dir: ${LIBCUCKOO_INCLUDE_DIR}")
#
#set(LIBMEMCACHED_CXX_FLAGS "${EXTERNAL_CXX_FLAGS}")
#set(LIBMEMCACHED_C_FLAGS "${EXTERNAL_C_FLAGS}")
#set(LIBMEMCACHED_PREFIX "${PROJECT_BINARY_DIR}/external/libmemcached")
##set(LIBMEMCACHED_CMAKE_ARGS "-Wno-dev"
##        "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
##        "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}"
##        "-DCMAKE_CXX_FLAGS=${LIBCUCKOO_CXX_FLAGS}"
##        "-DCMAKE_C_FLAGS=${LIBCUCKOO_C_FLAGS}"
##        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
##        "-DCMAKE_INSTALL_PREFIX=${LIBCUCKOO_PREFIX}"
##        "-DBUILD_EXAMPLES=OFF"
##        "-DBUILD_TESTS=OFF"
##        "-DBUILD_SHARED_LIBS=OFF")
#set(LIBMEMCACHED_INCLUDE_DIR "${LIBCUCKOO_PREFIX}")
#set(LIBMEMCACHED_STATIC_LIB_NAME "${CMAKE_STATIC_LIBRARY_PREFIX}libmemcached")
#set(LIBMEMCACHED_LIBRARY "${LIBCUCKOO_PREFIX}")
##set(LIBMEMCACHED_LIBRARIES )
#
#ExternalProject_Add(libmemcached
#        GIT_REPOSITORY https://github.com/memcached/memcached.git
#        GIT_SUBMODULES
#        LIST_SEPARATOR |
#        LOG_DOWNLOAD ON
#        LOG_CONFIGURE ON
#        LOG_BUILD ON
#        LOG_INSTALL ON)
#
#include_directories(SYSTEM ${LIBMEMCACHED_INCLUDE_DIR})
#message(STATUS "Libcuckoo include dir: ${LIBMEMCACHED_INCLUDE_DIR}")

set(Boost_VERSION 1.69.0)
set(Boost_BUILD ON)
set(Boost_COMPONENTS "program_options")

if (DEFINED ENV{Boost_ROOT} AND EXISTS $ENV{Boost_ROOT})
    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_ROOT "$ENV{Boost_ROOT}")
    find_package(Boost ${Boost_VERSION} COMPONENTS ${Boost_COMPONENTS})
    if (Boost_FOUND)
        set(Boost_BUILD OFF)
        set(Boost_INCLUDE_DIR ${Boost_INCLUDE_DIRS})
        get_filename_component(Boost_HOME ${Boost_INCLUDE_DIR} DIRECTORY)
        add_custom_target(boost_ep)
    endif (Boost_FOUND)
endif ()

if (Boost_BUILD)
    string(REGEX REPLACE "\\." "_" Boost_VERSION_STR ${Boost_VERSION})
    set(Boost_PREFIX "${PROJECT_BINARY_DIR}/external/boost_ep")
    set(Boost_HOME ${Boost_PREFIX})
    set(Boost_INCLUDE_DIRS "${Boost_PREFIX}/include")
    set(Boost_INCLUDE_DIR "${Boost_INCLUDE_DIRS}")
    foreach (component ${Boost_COMPONENTS})
        list(APPEND Boost_COMPONENT_FLAGS --with-${component})
    endforeach ()
    ExternalProject_Add(boost_ep
            URL https://downloads.sourceforge.net/project/boost/boost/${Boost_VERSION}/boost_${Boost_VERSION_STR}.tar.bz2
            UPDATE_COMMAND ""
            CONFIGURE_COMMAND ./bootstrap.sh --prefix=${Boost_PREFIX}
            BUILD_COMMAND ./b2 link=static variant=release cxxflags=-fPIC cflags=-fPIC --prefix=${Boost_PREFIX} ${Boost_COMPONENT_FLAGS} install
            BUILD_IN_SOURCE 1
            INSTALL_COMMAND ""
            INSTALL_DIR ${Boost_PREFIX}
            LOG_DOWNLOAD ON
            LOG_CONFIGURE ON
            LOG_BUILD ON
            LOG_INSTALL ON)

    macro(libraries_to_fullpath out)
        set(${out})
        foreach (comp ${Boost_COMPONENTS})
            string(TOUPPER ${comp} comp_upper)
            set(Boost_${comp_upper}_LIBRARY ${Boost_PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}boost_${comp}${CMAKE_STATIC_LIBRARY_SUFFIX})
            list(APPEND ${out} Boost_${comp_upper}_LIBRARY)
        endforeach ()
    endmacro()
    libraries_to_fullpath(Boost_LIBRARIES)
endif ()

include_directories(SYSTEM ${Boost_INCLUDE_DIR})
message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIR}")
message(STATUS "Boost program options library: ${Boost_PROGRAM_OPTIONS_LIBRARY}")

set(LIBEVENT_VERSION "2.1.8")
set(LIBEVENT_BUILD ON)

if (DEFINED ENV{LIBEVENT_ROOT} AND EXISTS $ENV{LIBEVENT_ROOT})
    set(LIBEVENT_ROOT "$ENV{LIBEVENT_ROOT}")
    find_package(LIBEVENT ${LIBEVENT_VERSION})
    if (LIBEVENT_FOUND)
        set(LIBEVENT_BUILD OFF)
        set(LIBEVENT_INCLUDE_DIR ${LIBEVENT_INCLUDE_DIRS})
        set(LIBEVENT_LIBRARY ${LIBEVENT_LIBRARIES})
        get_filename_component(LIBEVENT_HOME ${LIBEVENT_INCLUDE_DIR} DIRECTORY)
        add_custom_target(libevent_ep)
    endif (LIBEVENT_FOUND)
endif ()

if (LIBEVENT_BUILD)
    set(LIBEVENT_PREFIX "${PROJECT_BINARY_DIR}/external/libevent_ep")
    set(LIBEVENT_HOME "${LIBEVENT_PREFIX}")
    set(LIBEVENT_INCLUDE_DIR "${LIBEVENT_PREFIX}/include")
    set(LIBEVENT_STATIC_LIB_NAME "${CMAKE_STATIC_LIBRARY_PREFIX}event")
    set(LIBEVENT_CORE_STATIC_LIB_NAME "${CMAKE_STATIC_LIBRARY_PREFIX}event_core")
    set(LIBEVENT_EXTRA_STATIC_LIB_NAME "${CMAKE_STATIC_LIBRARY_PREFIX}event_extra")
    set(LIBEVENT_LIBRARY "${LIBEVENT_PREFIX}/lib/${LIBEVENT_STATIC_LIB_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}")
    set(LIBEVENT_CMAKE_ARGS "-Wno-dev"
            "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
            "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}"
            "-DCMAKE_INSTALL_PREFIX=${LIBEVENT_PREFIX}"
            "-DENABLE_TESTING=OFF"
            "-DBUILD_SHARED_LIBS=OFF"
            "-DEVENT__DISABLE_OPENSSL=ON"
            "-DEVENT__DISABLE_BENCHMARK=ON"
            "-DEVENT__DISABLE_TESTS=ON")
    ExternalProject_Add(libevent_ep
            URL https://github.com/nmathewson/Libevent/archive/release-${LIBEVENT_VERSION}-stable.tar.gz
            CMAKE_ARGS ${LIBEVENT_CMAKE_ARGS}
            LOG_DOWNLOAD ON
            LOG_CONFIGURE ON
            LOG_BUILD ON
            LOG_INSTALL ON)
endif ()

include_directories(SYSTEM ${LIBEVENT_INCLUDE_DIR})
message(STATUS "Libevent include dir: ${LIBEVENT_INCLUDE_DIR}")
message(STATUS "Libevent static library: ${LIBEVENT_LIBRARY}")

set(THRIFT_VERSION "0.12.0")
set(THRIFT_BUILD ON)

if (DEFINED ENV{THRIFT_ROOT} AND EXISTS $ENV{THRIFT_ROOT})
    set(THRIFT_ROOT "$ENV{THRIFT_ROOT}")
    find_package(THRIFT ${THRIFT_VERSION})
    if (THRIFT_FOUND)
        set(THRIFT_BUILD OFF)
        add_custom_target(thrift_ep)
    endif (THRIFT_FOUND)
endif ()

if (THRIFT_BUILD)
    set(THRIFT_PREFIX "${PROJECT_BINARY_DIR}/external/thrift_ep")
    set(THRIFT_HOME "${THRIFT_PREFIX}")
    set(THRIFT_INCLUDE_DIR "${THRIFT_PREFIX}/include")
    set(THRIFT_CMAKE_ARGS "-Wno-dev"
            "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
            "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}"
            "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
            "-DCMAKE_INSTALL_PREFIX=${THRIFT_PREFIX}"
            "-DCMAKE_INSTALL_RPATH=${THRIFT_PREFIX}/lib"
            "-DBUILD_COMPILER=${GENERATE_THRIFT}"
            "-DBUILD_TESTING=OFF"
            "-DWITH_SHARED_LIB=OFF"
            "-DWITH_QT4=OFF"
            "-DWITH_QT5=OFF"
            "-DWITH_C_GLIB=OFF"
            "-DWITH_HASKELL=OFF"
            "-DWITH_LIBEVENT=ON"
            "-DWITH_ZLIB=OFF"
            "-DWITH_JAVA=OFF"
            "-DWITH_PYTHON=OFF"
            "-DWITH_CPP=ON"
            "-DWITH_STDTHREADS=OFF"
            "-DWITH_BOOSTTHREADS=OFF"
            "-DWITH_STATIC_LIB=ON"
            "-DCMAKE_PREFIX_PATH=${OPENSSL_HOME}|${Boost_HOME}|${LIBEVENT_HOME}")

    set(THRIFT_STATIC_LIB_NAME "${CMAKE_STATIC_LIBRARY_PREFIX}thrift")
    #if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    #    set(THRIFT_STATIC_LIB_NAME "${THRIFT_STATIC_LIB_NAME}d")
    #endif ()
    set(THRIFT_LIBRARY "${THRIFT_PREFIX}/lib/${THRIFT_STATIC_LIB_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}")
    set(THRIFTNB_STATIC_LIB_NAME "${CMAKE_STATIC_LIBRARY_PREFIX}thriftnb")
    #if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    #    set(THRIFTNB_STATIC_LIB_NAME "${THRIFTNB_STATIC_LIB_NAME}d")
    #endif ()
    set(THRIFTNB_LIBRARY "${THRIFT_PREFIX}/lib/${THRIFTNB_STATIC_LIB_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}")
    if (GENERATE_THRIFT)
        set(THRIFT_COMPILER "${THRIFT_PREFIX}/bin/thrift")
    endif ()
    ExternalProject_Add(thrift_ep
            URL "http://archive.apache.org/dist/thrift/${THRIFT_VERSION}/thrift-${THRIFT_VERSION}.tar.gz"
            DEPENDS boost_ep openssl_ep libevent_ep
            CMAKE_ARGS ${THRIFT_CMAKE_ARGS}
            LIST_SEPARATOR |
            LOG_DOWNLOAD ON
            LOG_CONFIGURE ON
            LOG_BUILD ON
            LOG_INSTALL ON)
endif ()

include_directories(SYSTEM ${THRIFT_INCLUDE_DIR})
message(STATUS "Thrift include dir: ${THRIFT_INCLUDE_DIR}")
message(STATUS "Thrift static library: ${THRIFT_LIBRARY}")

if (GENERATE_THRIFT)
    message(STATUS "Thrift compiler: ${THRIFT_COMPILER}")
    add_executable(thriftcompiler IMPORTED GLOBAL)
    set_target_properties(thriftcompiler PROPERTIES IMPORTED_LOCATION ${THRIFT_COMPILER})
    add_dependencies(thriftcompiler thrift)
endif ()

set(CPP_REDIS_CXX_FLAGS "${EXTERNAL_CXX_FLAGS}")
set(CPP_REDIS_C_FLAGS "${EXTERNAL_C_FLAGS}")
set(CPP_REDIS_PREFIX "${PROJECT_BINARY_DIR}/external/cpp_redis")
set(CPP_REDIS_HOME "${CPP_REDIS_PREFIX}")
set(CPP_REDIS_INCLUDE_DIR "${CPP_REDIS_PREFIX}/include")
set(CPP_REDIS_CMAKE_ARGS "-Wno-dev"
        "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
        "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}"
        "-DCMAKE_CXX_FLAGS=${CPP_REDIS_CXX_FLAGS}"
        "-DCMAKE_C_FLAGS=${CPP_REDIS_C_FLAGS}"
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        "-DCMAKE_INSTALL_PREFIX=${CPP_REDIS_PREFIX}"
        "-DBUILD_EXAMPLES=OFF"
        "-DBUILD_TESTS=OFF"
        "-DBUILD_SHARED_LIBS=OFF")
set(CPP_REDIS_STATIC_LIB_NAME "${CMAKE_STATIC_LIBRARY_PREFIX}cpp_redis")
set(CPP_REDIS_TACOPIE_STATIC_LIB_NAME "${CMAKE_STATIC_LIBRARY_PREFIX}tacopie")
set(CPP_REDIS_LIBRARY "${CPP_REDIS_PREFIX}/lib/${CPP_REDIS_STATIC_LIB_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}")
set(CPP_REDIS_TACOPIE_LIBRARY "${CPP_REDIS_PREFIX}/lib/${CPP_REDIS_TACOPIE_STATIC_LIB_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}")
set(CPP_REDIS_LIBRARIES ${CPP_REDIS_LIBRARY} ${CPP_REDIS_TACOPIE_LIBRARY})
ExternalProject_Add(cpp_redis
        GIT_REPOSITORY https://github.com/Cylix/cpp_redis.git
        GIT_TAG ${CPP_REDIS_VERSION}
        GIT_SUBMODULES
        LIST_SEPARATOR |
        CMAKE_ARGS ${CPP_REDIS_CMAKE_ARGS}
        LOG_DOWNLOAD ON
        LOG_CONFIGURE ON
        LOG_BUILD ON
        LOG_INSTALL ON)
include_directories(SYSTEM ${CPP_REDIS_INCLUDE_DIR})
message(STATUS "cpp_redis include dir: ${CPP_REDIS_INCLUDE_DIR}")
message(STATUS "cpp_redis static library: ${CPP_REDIS_LIBRARIES}")

set(SSDB_ROCKS_PREFIX "${PROJECT_BINARY_DIR}/external/ssdb_rocks")
set(SSDB_ROCKS_HOME "${SSDB_ROCKS_PREFIX}/src/ssdb_rocks")
set(LEVELDB_PATH "${SSDB_ROCKS_HOME}/deps/rocksdb-master")
set(JEMALLOC_PATH "${SSDB_ROCKS_HOME}/deps/jemalloc-3.3.1")
set(SNAPPY_PATH "${SSDB_ROCKS_HOME}/deps/snappy-1.1.0")

set(LEVELDB_INCLUDE_DIR "${LEVELDB_PATH}/include")
set(JEMALLOC_INCLUDE_DIR "${JEMALLOC_PATH}/include")

set(LEVELDB_LIBRARY "${LEVELDB_PATH}/librocksdb.a")
set(SNAPPY_LIBRARY "${SNAPPY_PATH}/.libs/libsnappy.a")
set(JEMALLOC_LIBRARY "${JEMALLOC_PATH}/lib/libjemalloc.a")

set(SSDB_CLIENT_INCLUDE_DIR "${SSDB_ROCKS_HOME}/api/cpp")
set(SSDB_CLIENT_LIBRARY "${SSDB_ROCKS_HOME}/api/cpp/libssdb.a")

ExternalProject_Add(ssdb_rocks
        GIT_REPOSITORY "https://github.com/anuragkh/ssdb-rocks.git"
        PREFIX ${SSDB_ROCKS_PREFIX}
        BUILD_IN_SOURCE 1
        CONFIGURE_COMMAND ./build.sh
        BUILD_COMMAND "$(MAKE)"
        INSTALL_COMMAND ""
        LOG_DOWNLOAD ON
        LOG_CONFIGURE ON
        LOG_BUILD ON
        LOG_INSTALL ON)

include_directories(SYSTEM ${LEVELDB_INCLUDE_DIR})
include_directories(SYSTEM ${JEMALLOC_INCLUDE_DIR})
include_directories(SYSTEM ${SSDB_CLIENT_INCLUDE_DIR})

message(STATUS "LevelDB include dir: ${LEVELDB_INCLUDE_DIR}")
message(STATUS "LevelDB library: ${LEVELDB_LIBRARY}")

message(STATUS "Jemalloc include dir: ${JEMALLOC_INCLUDE_DIR}")
message(STATUS "Jemalloc library: ${JEMALLOC_LIBRARY}")

message(STATUS "Snappy library: ${SNAPPY_LIBRARY}")

message(STATUS "SSDB Client include dir: ${SSDB_CLIENT_INCLUDE_DIR}")
message(STATUS "SSDB Client library: ${SSDB_CLIENT_LIBRARY}")

find_package(ZLIB REQUIRED)
include_directories(SYSTEM ${ZLIB_INCLUDE_DIRS})
find_package(BZip2 REQUIRED)
include_directories(SYSTEM ${BZIP2_INCLUDE_DIR})



# - Try to find the libmemcached library # Once done this will define #
#  LIBMEMCACHED_FOUND - system has the libmemcached library
#  LIBMEMCACHED_INCLUDE_DIR - the libmemcached include directory(s)
#  LIBMEMCACHED_LIBRARIES - The libraries needed to use libmemcached

#  If the memcached libraries are found on the system, we assume they exist natively and dependencies
#  can be handled through package management.  If the libraries are not found, and if
#  MEMCACHED_USE_EXTERNAL_LIBRARY is ON, we will fetch, build, and include a copy of the neccessary
#  Libraries.

option(MEMCACHED_USE_EXTERNAL_LIBRARY "Pull and build source from external location if local is not found" NO)

# Search for native library to build against
if(WIN32)
    set(libmemcached_lib "libmemcached")
    set(libmemcachedUtil_lib "libmemcachedutil")
else()
    set(libmemcached_lib "memcached")
    set(libmemcachedUtil_lib "memcachedutil")
endif()

find_path(LIBMEMCACHED_INCLUDE_DIR libmemcached/memcached.hpp PATHS /usr/include /usr/share/include /usr/local/include PATH_SUFFIXES libmemcached)
find_library(LIBMEMCACHEDCORE_LIBRARY NAMES ${libmemcached_lib} PATHS /usr/lib usr/lib/libmemcached /usr/share /usr/lib64 /usr/local/lib /usr/local/lib64)
find_library(LIBMEMCACHEDUTIL_LIBRARY NAMES ${libmemcachedUtil_lib} PATHS /usr/lib /usr/share /usr/lib64 /usr/local/lib /usr/local/lib64)

set(LIBMEMCACHED_LIBRARIES ${LIBMEMCACHEDCORE_LIBRARY} ${LIBMEMCACHEDUTIL_LIBRARY})

if(LIBMEMCACHED_INCLUDE_DIR)
    if(EXISTS "${LIBMEMCACHED_INCLUDE_DIR}/libmemcached-1.0/configure.h")
        file(STRINGS "${LIBMEMCACHED_INCLUDE_DIR}/libmemcached-1.0/configure.h" version REGEX "#define LIBMEMCACHED_VERSION_STRING")
        string(REGEX REPLACE "#define LIBMEMCACHED_VERSION_STRING " "" version "${version}")
        string(REGEX REPLACE "\"" "" version "${version}")
        set(LIBMEMCACHED_VERSION_STRING ${version})
        if("${LIBMEMCACHED_VERSION_STRING}" VERSION_EQUAL "${LIBMEMCACHED_FIND_VERSION}" OR "${LIBMEMCACHED_VERSION_STRING}" VERSION_GREATER "${LIBMEMCACHED_FIND_VERSION}")
            set(LIBMEMCACHED_VERSION_OK 1)
            set(MSG "${DEFAULT_MSG}")
        else()
            set(LIBMEMCACHED_VERSION_OK 0)
            set(MSG "libmemcached version '${LIBMEMCACHED_VERSION_STRING}' incompatible with min version>=${LIBMEMCACHED_FIND_VERSION}")
        endif()
    endif()
endif()

if((LIBMEMCACHEDCORE_LIBRARY STREQUAL "LIBMEMCACHEDCORE_LIBRARY-NOTFOUND"
    OR LIBMEMCACHEDUTIL_LIBRARY STREQUAL "LIBMEMCACHEDUTIL_LIBRARY-NOTFOUND"
    OR LIBMEMCACHED_INCLUDE_DIR STREQUAL "LIBMEMCACHED_INCLUDE_DIR-NOTFOUND"
    OR NOT LIBMEMCACHED_VERSION_OK)
    AND MEMCACHED_USE_EXTERNAL_LIBRARY)
    # Currently libmemcached versions are not sufficient on ubuntu 12.04 and 14.04 LTS
    # until then, we build the required libraries from source
    if(NOT TARGET generate-libmemcached)
        #set(LIBMEMCACHED_URL https://launchpad.net/libmemcached/1.0/${LIBMEMCACHED_FIND_VERSION}/+download/libmemcached-${LIBMEMCACHED_FIND_VERSION}.tar.gz)
        set(LIBMEMCACHED_URL https://launchpad.net/libmemcached/1.0/1.0.18/+download/libmemcached-1.0.18.tar.gz)
        include(ExternalProject)
        ExternalProject_Add(
            generate-libmemcached
            URL ${LIBMEMCACHED_URL}
            DOWNLOAD_NO_PROGRESS 1
            TIMEOUT 15
            DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
            SOURCE_DIR ${CMAKE_BINARY_DIR}/downloads/libmemcached
            CONFIGURE_COMMAND "${CMAKE_BINARY_DIR}/downloads/libmemcached/configure" --prefix=${INSTALL_DIR} LDFLAGS=-L${LIB_PATH}
            BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} LDFLAGS=-Wl,-rpath-link,${LIB_PATH}
            BINARY_DIR ${CMAKE_BINARY_DIR}/build-libmemcached
            INSTALL_COMMAND "")
        add_library(libmemcached SHARED IMPORTED GLOBAL)
        add_library(libmemcachedutil SHARED IMPORTED GLOBAL)
        set_property(TARGET libmemcached
            PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/build-libmemcached/libmemcached/.libs/libmemcached.so.11.0.0)
        set_property(TARGET libmemcachedutil
            PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/build-libmemcached/libmemcached/.libs/libmemcachedutil.so.2.0.0)
        set_property(TARGET libmemcached
            PROPERTY IMPORTED_LINK_DEPENDENT_LIBRARIES libmemcachedutil)
        add_dependencies(libmemcached generate-libmemcached)
        add_dependencies(libmemcachedutil generate-libmemcached)

        if(PLATFORM)
            install(CODE "set(ENV{LD_LIBRARY_PATH} \"\$ENV{LD_LIBRARY_PATH}:${CMAKE_BINARY_DIR}:${CMAKE_BINARY_DIR}/build-libmemcached/libmemcached/.libs\")")
            install(PROGRAMS
                ${CMAKE_BINARY_DIR}/build-libmemcached/libmemcached/.libs/libmemcached.so
                ${CMAKE_BINARY_DIR}/build-libmemcached/libmemcached/.libs/libmemcached.so.11
                ${CMAKE_BINARY_DIR}/build-libmemcached/libmemcached/.libs/libmemcached.so.11.0.0
                ${CMAKE_BINARY_DIR}/build-libmemcached/libmemcached/.libs/libmemcachedutil.so
                ${CMAKE_BINARY_DIR}/build-libmemcached/libmemcached/.libs/libmemcachedutil.so.2
                ${CMAKE_BINARY_DIR}/build-libmemcached/libmemcached/.libs/libmemcachedutil.so.2.0.0
                DESTINATION lib)
        endif()
    endif()

    set(LIBMEMCACHEDCORE_LIBRARY $<TARGET_FILE:libmemcached>)
    set(LIBMEMCACHEDUTIL_LIBRARY $<TARGET_FILE:libmemcachedutil>)
    set(LIBMEMCACHED_LIBRARIES $<TARGET_FILE:libmemcached> $<TARGET_FILE:libmemcachedutil>)
    set(LIBMEMCACHED_INCLUDE_DIR ${CMAKE_BINARY_DIR}/downloads/libmemcached)
    # always assumed to be ok
    set(LIBMEMCACHED_VERSION_OK 1)
else()
    set(MEMCACHED_USE_EXTERNAL_LIBRARY OFF)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    LIBMEMCACHED DEFAULT_MSG
    LIBMEMCACHEDCORE_LIBRARY
    LIBMEMCACHEDUTIL_LIBRARY
    LIBMEMCACHED_INCLUDE_DIR
    LIBMEMCACHED_VERSION_OK)
mark_as_advanced(LIBMEMCACHED_INCLUDE_DIR LIBMEMCACHED_LIBRARIES LIBMEMCACHEDCORE_LIBRARY LIBMEMCACHEDUTIL_LIBRARY)
