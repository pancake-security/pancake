//
// Created by Lloyd Brown on 10/6/19.
//

#include <unordered_map>
#include <iostream>
#include <thread>
#include "assert.h"

#include "distribution.h"
#include "pancake_proxy.h"
#include "thrift_server.h"
#include "proxy_client.h"
#include "thrift_utils.h"

using namespace ::apache::thrift::transport;

#define HOST "127.0.0.1"
#define PROXY_PORT 9090

void flush_thread(bool &done, std::shared_ptr<proxy> proxy){
    while (!done){
        sleep(5);
        dynamic_cast<pancake_proxy&>(*proxy).flush();
    }
    std::cout << "Quitting spammer" << std::endl;
}

int main(){
    bool done = false;
    std::shared_ptr<proxy> proxy_ = std::make_shared<pancake_proxy>();
    std::vector<std::string> keys;
    std::string dummy(dynamic_cast<pancake_proxy&>(*proxy_).object_size_, '0');
    for (int i = 1000; i < 2000; i++){
        keys.push_back(std::to_string(i));
    }
    std::vector<std::string> values = std::vector<std::string>(keys.size(), dummy);
    auto id_to_client = std::make_shared<thrift_response_client_map>();
    void *arguments[4];
    distribution distribution_(keys, std::vector<double>(keys.size(), 1.0/keys.size()));
    arguments[0] = &distribution_;
    double alpha = 1.0 / keys.size();
    double delta = 1.0 / (2 * keys.size()) * 1 / alpha;
    arguments[1] = &alpha;
    arguments[2] = &delta;
    arguments[3] = &id_to_client;

    dynamic_cast<pancake_proxy&>(*proxy_).init(keys, values, arguments);
    std::cout << "Initialized pancake" << std::endl;
    auto proxy_server = thrift_server::create(proxy_, "pancake", id_to_client, PROXY_PORT, 1);
    std::thread proxy_serve_thread([&proxy_server] { proxy_server->serve(); });
    wait_for_server_start(HOST, PROXY_PORT);
    std::cout << "Proxy server is reachable" << std::endl;
    proxy_client client;
    client.init(HOST, PROXY_PORT);

    std::thread flusher(flush_thread, std::ref(done), proxy_);


    for (std::size_t i = 1000; i < 2000; ++i) {
        client.put(std::to_string(i), std::to_string(i));
    }
    std::cout << "Passed tests" << std::endl;
    for (std::size_t i = 1000; i < 2000; ++i) {
        assert(client.get(std::to_string(i)) != "");
    }
    std::cout << "Passed tests get" << std::endl;

    for (std::size_t i = 5000; i < 6000; ++i) {
        try {
            client.get(std::to_string(i));
            assert(false);
        }
        catch (apache::thrift::TApplicationException e) {
            assert(true);
        }
    }
    std::cout << "Passed tests" << std::endl;
    done = true;
    flusher.join();
    proxy_->close();
    proxy_server->stop();
    if (proxy_serve_thread.joinable()) {
        proxy_serve_thread.join();
    }
}
