//
// Created by Lloyd Brown on 10/5/19.
//

#include "proxy.h"
#include "thrift_server.h"

void proxy_server::usage() {
    std::cout << "Pancake proxy: frequency flattening kvs\n";
    // Network Parameters
    std::cout << "\t -h: Storage server host name\n";
    std::cout << "\t -p: Storage server port\n";
    std::cout << "\t -s: Storage server type (redis, rocksdb, memcached)\n";
    std::cout << "\t -n: Storage server count\n";
    std::cout << "\t -z: Proxy server type\n";
    // Workload parameters
    std::cout << "\t -w: Clients' workload file\n";
    std::cout << "\t -l: key size\n";
    std::cout << "\t -v: Value size\n";
    std::cout << "\t -b: Security batch size\n";
    std::cout << "\t -c: Storage batch size\n";
    std::cout << "\t -t: Number of worker threads for cpp_redis\n";
    // Other parameters
    std::cout << "\t -o: Output location for sizing thread\n";
    std::cout << "\t -d: Core to run on\n";
};

int proxy_server::main(int argc, char **argv) {
    int port = 9090;
    auto proxy = std::make_shared(new pancake_proxy());
    int o;
    std::string proxy_type = "pancake"
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
            case 'z':
                proxy_type = std::string(optarg);
            default:
                usage();
                exit(-1);
        }
    }

    std::vector<std::string> keys;
    void *arguments[3];
    arguments[0] = malloc(sizeof(distribution * ));
    arguments[1] = malloc(sizeof(double *));
    arguments[2] = malloc(sizeof(double *));

    arguments[0] = &distribution_;
    double alpha = 1.0 / distribution_.get_items_and_frequencies().first->size();
    double delta = 1.0 / (2 * distribution_.get_items_and_frequencies().first->size()) * 1 / alpha;
    arguments[1] = &alpha;
    arguments[2] = &delta;
    proxy->init(&keys, &keys, arguments);
    proxy_ = proxy;

    auto server = create(proxy, proxy_type);
    server->serve()
    return 0;
}