set(STORAGE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../libstorage/src)
set(PROXY_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../proxy/src)

message(${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${STORAGE_INCLUDE})

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
        ${CPP_REDIS_LIBRARIES}
        ${SSDB_CLIENT_LIBRARY}
        ${LEVELDB_LIBRARY}
        ${SNAPPY_LIBRARY}
        ${JEMALLOC_LIBRARY}
        ${CMAKE_DL_LIBS}
        ${ZLIB_LIBRARIES}
        ${BZIP2_LIBRARIES}
        ${STORAGE_INCLUDE_LIBRARIES}
        #${LIBMEMCACHED_LIBRARIES}
        )

include_directories(${PROXY_INCLUDE})

add_executable(distribution_test
        src/distribution_test.cpp
        ../proxy/src/distribution.h
        ../proxy/src/distribution.cpp
        )
target_link_libraries(distribution_test
        ${PROXY_INCLUDE_LIBRARIES}
        )

add_executable(encryption_test
        src/encryption_test.cpp
        ../proxy/src/encryption_engine.h
        ../proxy/src/encryption_engine.cpp
        ../proxy/src/util.h
        ../proxy/src/util.cpp
        )
target_link_libraries(encryption_test
        ${PROXY_INCLUDE_LIBRARIES}
        ${OPENSSL_LIBRARIES}
        )

add_executable(update_cache_test
        src/update_cache_test.cpp
        ../proxy/src/update_cache.h
        ../proxy/src/update_cache.cpp
        )
target_link_libraries(update_cache_test
        ${PROXY_INCLUDE_LIBRARIES}
        )
