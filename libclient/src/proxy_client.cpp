//
// Created by Lloyd Brown on 10/1/19.
//

#include "proxy_client.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;

void proxy_client::init(const std::string &host_name, int port) {
    auto socket = std::make_shared<TSocket>(host_name, port);;
    auto transport = std::shared_ptr<TTransport>(new TFramedTransport(socket));
    auto protocol = std::shared_ptr<TProtocol>(new TBinaryProtocol(transport));
    client_ = std::make_shared<pancake_thriftClient>(protocol);
}

std::string proxy_client::get(const std::string &key) {
    std::string _return;
    client_->get(_return, key);
    return _return;
}

void proxy_client::put(const std::string &key, const std::string &value) {
    std::string _return;
    client_->put(_return, key, value);
}

std::vector<std::string> proxy_client::get_batch(const std::vector<std::string> &keys) {
    std::vector<std::string> _return;
    client_->get_batch(_return, keys);
    return _return;
}

void proxy_client::put_batch(const std::vector<std::string> keys, const std::vector<std::string> values) {
    std::string _return;
    client_->put_batch(_return, keys, values);
}
