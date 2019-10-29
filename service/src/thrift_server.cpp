//
// Created by Lloyd Brown on 10/5/19.
//

#include "thrift_server.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace ::apache::thrift::concurrency;

std::shared_ptr<TServer> thrift_server::create(std::shared_ptr<proxy> proxy_ptr, const std::string &proxy_type, int port, size_t num_threads) {
    auto clone_factory = std::make_shared<thrift_handler_factory>(proxy_ptr, proxy_type);
    auto proc_factory = std::make_shared<pancake_thriftProcessorFactory>(clone_factory);
    auto socket = std::make_shared<TNonblockingServerSocket>(port);
    auto server = std::make_shared<TNonblockingServer>(proc_factory, socket);
    server->setUseHighPriorityIOThreads(true);
    server->setNumIOThreads(num_threads);
    return server;
}