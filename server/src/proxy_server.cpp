//
// Created by Lloyd Brown on 10/5/19.
//

#include "proxy.h"
#include "thrift_server.h"

void usage() {
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

int main(int argc, char **argv) {
    int port = 9090;
    std::shared_ptr<proxy> proxy_ = std::make_shared<pancake_proxy>();
    int o;
    std::string proxy_type_ = "pancake";
    while ((o = getopt(argc, argv, "a:p:s:n:w:v:b:c:p:o:d:t:x:f:z:")) != -1) {
        switch (o) {
            case 'h':
                dynamic_cast<pancake_proxy&>(*proxy_).server_host_name_ = std::string(optarg);
                break;
            case 'p':
                dynamic_cast<pancake_proxy&>(*proxy_).server_port_ = std::atoi(optarg);
                break;
            case 's':
                dynamic_cast<pancake_proxy&>(*proxy_).server_type_ = std::string(optarg);
                break;
            case 'n':
                dynamic_cast<pancake_proxy&>(*proxy_).server_count_ = std::atoi(optarg);
                break;
            case 'w':
                dynamic_cast<pancake_proxy&>(*proxy_).workload_file_ = std::string(optarg);
                break;
            case 'l':
                dynamic_cast<pancake_proxy&>(*proxy_).key_size_ = std::atoi(optarg);
                break;
            case 'v':
                dynamic_cast<pancake_proxy&>(*proxy_).object_size_ = std::atoi(optarg);
                break;
            case 'b':
                dynamic_cast<pancake_proxy&>(*proxy_).security_batch_size_ = std::atoi(optarg);
                break;
            case 'c':
                dynamic_cast<pancake_proxy&>(*proxy_).storage_batch_size_ = std::atoi(optarg);
                break;
            case 't':
                dynamic_cast<pancake_proxy&>(*proxy_).p_threads_ = std::atoi(optarg);
                break;
            case 'o':
                dynamic_cast<pancake_proxy&>(*proxy_).output_location_ = std::string(optarg);
                break;
            case 'd':
                dynamic_cast<pancake_proxy&>(*proxy_).core_ = std::atoi(optarg) - 1;
                break;
            case 'z':
                proxy_type_ = std::string(optarg);
            default:
                usage();
                exit(-1);
        }
    }

    std::vector<std::string> keys;
    std::string dummy(dynamic_cast<pancake_proxy&>(*proxy_).object_size_, '0');
    std::vector<std::string> values = std::vector<std::string>(1000, dummy);
    for (int i = 0; i < 1000; i++){
        keys.push_back(std::to_string(i));
    }
    void *arguments[3];
    arguments[0] = malloc(sizeof(distribution *));
    arguments[1] = malloc(sizeof(double *));
    arguments[2] = malloc(sizeof(double *));

    distribution distribution_(keys, std::vector<double>(1000, 1.0/keys.size()));
    arguments[0] = &distribution_;
    double alpha = 1.0 / keys.size();
    double delta = 1.0 / (2 * keys.size()) * 1 / alpha;
    arguments[1] = &alpha;
    arguments[2] = &delta;
    dynamic_cast<pancake_proxy&>(*proxy_).init(keys, values, arguments);

    auto server = thrift_server::create(proxy_, proxy_type_, port, 1);
    server->serve();
    dynamic_cast<pancake_proxy&>(*proxy_).close();
    return 0;
    // TODO: Perform waiting
}