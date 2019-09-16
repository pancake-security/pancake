set(STORAGE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../libstorage/src)

message(${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${STORAGE_INCLUDE})

set (CMAKE_C_COMPILER "/usr/bin/clang")
set (CMAKE_CXX_COMPILER "/usr/bin/clang++")
#set (CMAKE_C_COMPILER "/usr/local/bin/gcc-5")
#set (CMAKE_CXX_COMPILER "/usr/local/bin/g++-5")

# Proxy server
add_executable(storage_interface_test
        src/storage_interface_test.cpp
        ../libstorage/src/storage_interface.h
        ../libstorage/src/redis.cpp
        ../libstorage/src/redis.h
        ../libstorage/src/rocksdb.cpp
        ../libstorage/src/rocksdb.h
        #../libstorage/src/src/memcached.cpp
        #../libstorage/src/src/memcached.h
        )
add_dependencies(storage_interface_test cpp_redis ssdb_rocks)
target_link_libraries(storage_interface_test
        #${THRIFT_LIBRARIES}
        ${CPP_REDIS_LIBRARIES}
        ${OPENSSL_LIBRARIES}
        ${SSDB_CLIENT_LIBRARY}
        ${LEVELDB_LIBRARY}
        ${SNAPPY_LIBRARY}
        ${JEMALLOC_LIBRARY}
        ${CMAKE_DL_LIBS}
        ${ZLIB_LIBRARIES}
        ${BZIP2_LIBRARIES}
        ${STORAGE_INCLUDE}
        #${LIBCUCKOO_LIBRARIES}
        #${LIBMEMCACHED_LIBRARIES}
        Threads::Threads)
