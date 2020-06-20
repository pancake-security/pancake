//
// Created by Lloyd Brown on 10/5/19.
//

#include "thrift_server.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace ::apache::thrift::concurrency;

std::shared_ptr<TServer> thrift_server::create(std::shared_ptr<proxy> proxy_ptr, const std::string &proxy_type, std::shared_ptr<thrift_response_client_map> id_to_client, int port, size_t num_threads) {
    auto clone_factory = std::make_shared<thrift_handler_factory>(proxy_ptr, proxy_type, id_to_client);
    auto proc_factory = std::make_shared<pancake_thriftProcessorFactory>(clone_factory);
    auto socket = std::make_shared<TNonblockingServerSocket>(port);
    //auto socket = std::make_shared<TServerSocket>(port);
    //auto server = std::make_shared<TNonblockingServer>(proc_factory, socket);

    std::shared_ptr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(15);
    std::shared_ptr<PosixThreadFactory> threadFactory = std::shared_ptr<PosixThreadFactory>(new PosixThreadFactory());
    threadManager->threadFactory(threadFactory);
    threadManager->start();
    auto server = std::make_shared<TNonblockingServer>(proc_factory, std::make_shared<TBinaryProtocolFactory>(), socket, threadManager);

    //server->setUseHighPriorityIOThreads(true);
    server->setNumIOThreads(num_threads);
    /*  auto server = std::make_shared<TThreadedServer>(proc_factory,
                                                    socket,  
                                                    std::make_shared<TBufferedTransportFactory>(), 
                                                    std::make_shared<TBinaryProtocolFactory>(), 
                                                    std::make_shared<PosixThreadFactory>());
    */
    return server;
}
