set(STORAGE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../libstorage)
set(PROXY_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../proxy)

include_directories(${STORAGE_INCLUDE} ${PROXY_INCLUDE})

set (CMAKE_C_COMPILER "/usr/bin/gcc-5")
set (CMAKE_CXX_COMPILER "/usr/bin/g++-5")

# Proxy server
add_executable(pancake_benchmark
        src/pancake_benchmark.h
        src/pancake_benchmark.cpp
        src/pancake_proxy.cpp
        src/pancake_proxy.h
        src/operation.cpp
        src/operation.h
        src/proxy.h
        src/storage_interface.cpp
        src/storage_interface.h
        src/redis.cpp
        src/redis.h
        src/rocksdb.cpp
        src/rocksdb.h
        src/memcached.cpp
        src/memcached.h
        src/distribution.h
        src/standard_distribution.h
        src/update_cache.h
        src/cuckoo_update_cache.h
        src/timer.h
        )
add_dependencies(pancake_benchmark cpp_redis ssdb_rocks libcuckoo openssl_ep)
target_link_libraries(pancake_proxy
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
        ${LIBCUCKOO_LIBRARIES}
        ${LIBMEMCACHED_LIBRARIES}
        Threads::Threads)