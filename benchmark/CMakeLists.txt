set(STORAGE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../libstorage/src)
set(STORAGE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../libclient/src)
set(PROXY_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../proxy/src)
set(BENCHMARK_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories(${STORAGE_INCLUDE} ${PROXY_INCLUDE})

# Proxy server
add_executable(pancake_benchmark
        src/pancake_benchmark.cpp
        src/pancake_benchmark.h
        ../proxy/src/pancake_thrift_server.h
        ../proxy/src/pancake_thrift_server.cpp
        ../proxy/src/pancake_proxy.cpp
        ../proxy/src/pancake_proxy.h
        ../proxy/src/operation.h
        ../proxy/src/proxy.h
        ../libstorage/src/storage_interface.h
        ../libstorage/src/redis.cpp
        ../libstorage/src/redis.h
        ../libstorage/src/rocksdb.cpp
        ../libstorage/src/rocksdb.h
        #../libstorage/src/memcached.cpp
        #../libstorage/src/memcached.h
        ../proxy/src/distribution.h
        ../proxy/src/update_cache.h
        ../proxy/src/encryption_engine.h
        src/timer.h
        ../libclient/src/client.h
        ../libclient/src/proxy_client.h
        ../libclient/src/proxy_client.cpp
        )
add_dependencies(pancake_benchmark cpp_redis ssdb_rocks libcuckoo openssl_ep)
target_link_libraries(pancake_benchmark
        ${THRIFT_LIBRARIES}
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
        #${LIBMEMCACHED_LIBRARIES}
        ${PROXY_INCLUDE_LIBRARIES}
        ${STORAGE_INCLUDE_LIBRARIES}
        ${BENCHMARK_INCLUDE_LIBRARIES}
        Threads::Threads)