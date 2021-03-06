set(STORAGE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../libstorage/src)
set(CLIENT_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../libclient/src)
set(PROXY_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../proxy/src)
set(SERVICE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../service/src)
set(SERVER_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include_directories(${STORAGE_INCLUDE} ${CLIENT_INCLUDE} ${PROXY_INCLUDE} ${SERVICE_INCLUDE} ${SERVER_INCLUDE} )


add_executable(proxy_server
        src/proxy_server.cpp

        ../libclient/src/client.h
        ../libclient/src/proxy_client.cpp
        ../libclient/src/proxy_client.h

        ../service/src/thrift_server.cpp
        ../service/src/thrift_server.h
        ../service/src/thrift_handler_factory.cpp
        ../service/src/thrift_handler_factory.h
        ../service/src/thrift_handler.cpp
        ../service/src/thrift_handler.h
        ../service/src/thrift_response_client_map.cpp
        ../service/src/thrift_response_client_map.h
        ../service/src/thrift_response_client.cpp
        ../service/src/thrift_response_client.h
        ../service/src/thrift_response_service.cpp
        ../service/src/thrift_response_service.h
        ../service/src/thrift.cpp
        ../service/src/thrift.h
        ../service/src/proxy_types.cpp
        ../service/src/proxy_types.h
        ../service/src/proxy_constants.cpp
        ../service/src/proxy_constants.h

        ../proxy/src/pancake_proxy.cpp
        ../proxy/src/pancake_proxy.h
        ../proxy/src/proxy.h
        ../proxy/src/operation.h
        ../proxy/src/distribution.h
        ../proxy/src/distribution.cpp
        ../proxy/src/queue.h
        ../proxy/src/update_cache.h
        ../proxy/src/update_cache.cpp
        ../proxy/src/encryption_engine.h
        ../proxy/src/encryption_engine.cpp
        ../proxy/src/util.h
        ../proxy/src/util.cpp

        ../libstorage/src/storage_interface.h
        ../libstorage/src/redis.h
        ../libstorage/src/redis.cpp
        ../libstorage/src/rocksdb.cpp
        ../libstorage/src/rocksdb.h
        #../libstorage/src/memcached.cpp
        #../libstorage/src/memcached.h
        ../service/src/thrift_utils.h)
add_dependencies(proxy_server cpp_redis ssdb_rocks libcuckoo openssl_ep thrift_ep)
target_link_libraries(proxy_server
        Threads::Threads
        ${THRIFTNB_LIBRARY}
        ${THRIFT_LIBRARY}
        ${LIBEVENT_LIBRARY}
        ${CPP_REDIS_LIBRARIES}
        ${OPENSSL_LIBRARIES}
        ${SSDB_CLIENT_LIBRARY}
        ${LEVELDB_LIBRARY}
        ${SNAPPY_LIBRARY}
        ${JEMALLOC_LIBRARY}
        ${CMAKE_DL_LIBS}
        ${ZLIB_LIBRARIES}
        ${BZIP2_LIBRARIES}
        #${LIBMEMCACHED_LIBRARIES}
        ${PROXY_INCLUDE_LIBRARIES}
        ${STORAGE_INCLUDE_LIBRARIES}
        ${BENCHMARK_INCLUDE_LIBRARIES}
        ${SERVICE_INCLUDE_LIBRARIES}
        ${CLIENT_INCLUDE_LIBRARIES}
        Threads::Threads)
