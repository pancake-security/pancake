//
// Created by Lloyd Brown on 10/3/19.
//

#include "thrift_handler.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

thrift_handler::thrift_handler() {

}

thrift_handler::thrift_handler(std::shared_ptr<proxy> proxy, const std::string &proxy_type) {
    proxy_ = proxy;
    proxy_type_ = proxy_type;
}

void thrift_handler::get(std::string& _return, const std::string& key) {
    _return = proxy_->get(operation_count_++, key);
}

void thrift_handler::put(std::string& _return, const std::string& key, const std::string& value) {
    proxy_->put(operation_count_++, key, value);
}

void thrift_handler::get_batch(std::vector<std::string> & _return, const std::vector<std::string> &keys) {
    _return = proxy_->get_batch(operation_count_++, keys);
}

void thrift_handler::put_batch(std::string& _return, const std::vector<std::string> & keys, const std::vector<std::string> & values) {
    proxy_->put_batch(operation_count_++, keys, values);
}