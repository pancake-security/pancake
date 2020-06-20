//
// Created by Lloyd Brown on 10/5/19.
//

#ifndef PANCAKE_THRIFT_SERVER_H
#define PANCAKE_THRIFT_SERVER_H

#include <thrift/server/TThreadedServer.h>
#include <thrift/server/TServer.h>
#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/PlatformThreadFactory.h>
#include <thrift/server/TNonblockingServer.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TNonblockingServerSocket.h>
#include <thrift/transport/TServerSocket.h>

#include "thrift_handler_factory.h"

class thrift_server {
    public:
        typedef std::shared_ptr<proxy> proxy_ptr;
        typedef std::shared_ptr<apache::thrift::server::TServer> server_ptr;
        static server_ptr create(proxy_ptr, const std::string &proxy_type, std::shared_ptr<thrift_response_client_map> id_to_client_, int port, size_t num_threads);
};

#endif //PANCAKE_THRIFT_SERVER_H
