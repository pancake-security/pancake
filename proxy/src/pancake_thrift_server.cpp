//
// Created by Lloyd Brown on 10/3/19.
//

#include "pancake_thrift_server.h"

#include "../../gen-cpp/pancake_thrift.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TNonblockingServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

    pancake_thriftHandler::pancake_thriftHandler() {

    }

    pancake_thriftHandler::pancake_thriftHandler(distribution dist) {
        distribution_ = dist;
    }

    void pancake_thriftHandler::get(std::string& _return, const std::string& key) {
        if (proxy_type_ == "pancake")
            _return = dynamic_cast<pancake_proxy *>(proxy_)->get(operation_count_++, key, _return);
        else
            _return = proxy_->get(operation_count_++, key);
    }

    void pancake_thriftHandler::put(std::string& _return, const std::string& key, const std::string& value) {
        if (proxy_type_ == "pancake")
            dynamic_cast<pancake_proxy *>(proxy_)->put(operation_count_++, key, value, _return);
        else
            proxy_->put(operation_count_++, key, value);
    }

    void pancake_thriftHandler::get_batch(std::vector<std::string> & _return, const std::vector<std::string> & keys) {
        if (proxy_type_ == "pancake")
            _return = dynamic_cast<pancake_proxy *>(proxy_)->get_batch(operation_count_++, &keys, _return);
        else
            _return = proxy_->get_batch(operation_count_++, &keys);
    }

    void pancake_thriftHandler::put_batch(std::string& _return, const std::vector<std::string> & keys, const std::vector<std::string> & values) {
        if (proxy_type_ == "pancake")
            dynamic_cast<pancake_proxy *>(proxy_)->put_batch(operation_count_++, &keys, &values, _return);
        else
            proxy_->put_batch(operation_count_++, &keys, &values);
    }

    void pancake_thriftHandler::usage() {
        std::cout << "Pancake proxy: frequency flattening kvs\n";
        // Network Parameters
        std::cout << "\t -h: Storage server host name\n";
        std::cout << "\t -p: Storage server port\n";
        std::cout << "\t -s: Storage server type (redis, rocksdb, memcached)\n";
        std::cout << "\t -n: Storage server count\n";
        // Workload parameters
        std::cout << "\t -w: Clients' first workload file\n";
        std::cout << "\t -l: key size\n";
        std::cout << "\t -v: Value size\n";
        std::cout << "\t -b: Security batch size\n";
        std::cout << "\t -c: Storage batch size\n";
        std::cout << "\t -t: Number of worker threads for cpp_redis\n";
        // Other parameters
        std::cout << "\t -o: Output location for sizing thread\n";
        std::cout << "\t -d: Core to run on\n";
    };

class proxy_server_processor_factory : public TProcessorFactory {
public:
    proxy_server_processor_factory() {
    }

    ::apache::thrift::stdcxx::shared_ptr<TProcessor> getProcessor(const TConnectionInfo&) {
        ::apache::thrift::stdcxx::shared_ptr<pancake_thriftHandler> handler(
                new pancake_thriftHandler());
        ::apache::thrift::stdcxx::shared_ptr<TProcessor> handlerProcessor(
                new pancake_thriftProcessor(handler));
        return handlerProcessor;
    }
};
    
    int pancake_thriftHandler::main(int argc, char **argv) {
        int port = 9090;

        //::apache::thrift::stdcxx::shared_ptr<pancake_thriftHandler> handler(new pancake_thriftHandler());
        ::apache::thrift::stdcxx::shared_ptr<TProcessorFactory> processorFactory(new proxy_server_processor_factory());
        ::apache::thrift::stdcxx::shared_ptr<TNonblockingServerSocket> socket(new TNonblockingServerSocket(port));
        //::apache::thrift::stdcxx::shared_ptr<TNonblockingServerTransport> transport(new TFramedTransport());
        //::apache::thrift::stdcxx::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

        TNonblockingServer server(processorFactory, socket);

        //           auto clone_factory = std::make_shared<block_request_handler_factory>(blocks);
        //           auto proc_factory = std::make_shared<block_request_serviceProcessorFactory>(clone_factory);
        //           auto socket = std::make_shared<TNonblockingServerSocket>(port);
        //           auto server = std::make_shared<TNonblockingServer>(proc_factory, socket);

        //TNonblockingServer server(processor, socket, transportFactory, protocolFactory);
    
        auto proxy = new pancake_proxy();
    
        int o;
        while ((o = getopt(argc, argv, "a:p:s:n:w:v:b:c:p:o:d:t:x:f:")) != -1) {
            switch (o) {
                case 'h':
                    proxy->server_host_name_ = std::string(optarg);
                    break;
                case 'p':
                    proxy->server_port_ = std::atoi(optarg);
                    break;
                case 's':
                    proxy->server_type_ = std::string(optarg);
                    break;
                case 'n':
                    proxy->server_count_ = std::atoi(optarg);
                    break;
                case 'w':
                    proxy->workload_file_ = std::string(optarg);
                    break;
                case 'l':
                    proxy->key_size_ = std::atoi(optarg);
                    break;
                case 'v':
                    proxy->object_size_ = std::atoi(optarg);
                    break;
                case 'b':
                    proxy->security_batch_size_ = std::atoi(optarg);
                    break;
                case 'c':
                    proxy->storage_batch_size_ = std::atoi(optarg);
                    break;
                case 't':
                    proxy->pthreads_ = std::atoi(optarg);
                    break;
                case 'o':
                    proxy->output_location_ = std::string(optarg);
                    break;
                case 'd':
                    proxy->core_ = std::atoi(optarg) - 1;
                    break;
                default:
                    usage();
                    exit(-1);
            }
        }

        std::vector<std::string> keys;
        void * arguments [3];
        arguments[0] = malloc(sizeof(distribution *));
        arguments[1] = malloc(sizeof(double*));
        arguments[2] = malloc(sizeof(double*));

        arguments[0] = &distribution_;
        double alpha = 1.0/distribution_.get_items_and_frequencies().first->size();
        double delta = 1.0/(2*distribution_.get_items_and_frequencies().first->size())*1/alpha;
        arguments[1] = &alpha;
        arguments[2] = &delta;
        proxy->init(&keys, &keys, arguments);
        proxy_ = proxy;
    
    
        server.serve();
        return 0;
    }