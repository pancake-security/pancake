set(STORAGE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../libstorage)
set(PROXY_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories(${STORAGE_INCLUDE})

# Proxy server
add_executable(pancake_proxy
        src/pancake_proxy.cpp
        src/pancake_proxy.h
        src/proxy.h
        src/operation.h
        ../libstorage/src/storage_interface.h
        #../libstorage/src/redis.cpp
        #../libstorage/src/redis.h
        #../libstorage/src/rocksdb.cpp
        #../libstorage/src/rocksdb.h
        #../libstorage/src/memcached.cpp
        #../libstorage/src/memcached.h
        src/distribution.h
        src/distribution.cpp
        src/queue.h
        src/queue.cpp
        src/update_cache.h
        src/update_cache.cpp
        src/encryption_engine.h
        src/encryption_engine.cpp
        src/util.h
        src/util.cpp
        )
#add_dependencies(pancake_proxy cpp_redis ssdb_rocks libcuckoo openssl_ep)
add_dependencies(pancake_proxy ssdb_rocks libcuckoo openssl_ep)
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
        ${STORAGE_INCLUDE_LIBRARIES}
        #${LIBMEMCACHED_LIBRARIES}
        Threads::Threads)

## nothriftORAM proxy
#add_executable(path_oram
#      src/path_oram.cpp
#      src/proxy.cpp
#      src/proxy.h
#      src/storage_interface.cpp
#      src/storage_interface.h
#      src/redis.cpp
#      src/redis.h
#      src/rocksdb.cpp
#      src/rocksdb.h
#      src/memcached.cpp
#      src/memcached.h
#      src/rocksdb.cpp
#      src/rocksdb.h
#      src/PathORAM.cpp
#      src/PathORAM.h
#      src/Util.h
#      )
#add_dependencies(path_oram cpp_redis ssdb_rocks thrift libcuckoo)
#target_link_libraries(path_oram
#      #${CPP_REDIS_LIBRARIES}
#      #${THRIFT_LIBRARIES}
#      ${OPENSSL_LIBRARIES}
#      ${SSDB_CLIENT_LIBRARY}
#      ${LEVELDB_LIBRARY}
#      ${SNAPPY_LIBRARY}
#      ${JEMALLOC_LIBRARY}
#      ${CMAKE_DL_LIBS}
#      ${ZLIB_LIBRARIES}
#      ${BZIP2_LIBRARIES}
#      ${LIBMEMCACHED_LIBRARIES}
#      Threads::Threads
#       cryptopp)# nothriftORAM proxy