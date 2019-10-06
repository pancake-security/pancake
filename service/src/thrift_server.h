//
// Created by Lloyd Brown on 10/5/19.
//

#ifndef PANCAKE_THRIFT_SERVER_H
#define PANCAKE_THRIFT_SERVER_H

#include <thrift/server/TThreadedServer.h>
#include <thrift/server/TServer.h>
#include <thrift/concurrency/ThreadManager.h>
#include <thrift/server/TNonblockingServer.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/transport/TNonblockingServerSocket.h>

#include "thrift_handler_factory.h"

class thrift_server {
    public:
        typedef std::shared_ptr<proxy> proxy_ptr;
        typedef std::shared_ptr<apache::thrift::server::TServer> server_ptr;
        static server_ptr create(proxy_ptr, const std::string &proxy_type, int port, size_t num_threads = 1);
};

#endif //PANCAKE_THRIFT_SERVER_H
