set(STORAGE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../libstorage)

include_directories(${STORAGE_INCLUDE})

set (CMAKE_C_COMPILER "/usr/bin/gcc-5")
set (CMAKE_CXX_COMPILER "/usr/bin/g++-5")

# Proxy server
add_executable(pancake_proxy
        src/pancake_proxy.cpp
        src/pancake_proxy.h
        src/operation.cpp
        src/operation.h
        src/proxy.h
        src/storage_interface.h
        src/redis.cpp
        src/redis.h
        src/rocksdb.cpp
        src/rocksdb.h
        src/memcached.cpp
        src/memcached.h
        src/distribution.h
        src/update_cache.h
        src/encryption_engine.h
        )
add_dependencies(pancake_proxy cpp_redis ssdb_rocks libcuckoo openssl_ep)
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

# nothriftORAM proxy
add_executable(path_oram
      src/path_oram.cpp
      src/proxy.cpp
      src/proxy.h
      src/storage_interface.cpp
      src/storage_interface.h
      src/redis.cpp
      src/redis.h
      src/rocksdb.cpp
      src/rocksdb.h
      src/memcached.cpp
      src/memcached.h
      src/rocksdb.cpp
      src/rocksdb.h
      src/PathORAM.cpp
      src/PathORAM.h
      src/Util.h
      )
add_dependencies(path_oram cpp_redis ssdb_rocks thrift libcuckoo)
target_link_libraries(path_oram
      #${CPP_REDIS_LIBRARIES}
      ${THRIFT_LIBRARIES}
      ${OPENSSL_LIBRARIES}
      ${SSDB_CLIENT_LIBRARY}
      ${LEVELDB_LIBRARY}
      ${SNAPPY_LIBRARY}
      ${JEMALLOC_LIBRARY}
      ${CMAKE_DL_LIBS}
      ${ZLIB_LIBRARIES}
      ${BZIP2_LIBRARIES}
      ${LIBMEMCACHED_LIBRARIES}
      Threads::Threads
       cryptopp)# nothriftORAM proxy