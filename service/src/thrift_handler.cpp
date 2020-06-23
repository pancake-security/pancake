//
// Created by Lloyd Brown on 10/3/19.
//

#include "thrift_handler.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

// thrift_handler::thrift_handler() {
//     client_id_gen_(client_id_gen);
// }

// thrift_handler::thrift_handler(std::shared_ptr<proxy> proxy, const std::string &proxy_type) {
//     proxy_ = proxy;
//     proxy_type_ = proxy_type;
//     std::atomic<int64_t> client_id_gen_;
// }

thrift_handler::thrift_handler(std::shared_ptr<proxy> proxy, const std::string &proxy_type,
                               std::atomic<int64_t> &client_id_gen,
                               std::shared_ptr<::apache::thrift::protocol::TProtocol> prot,
                               std::shared_ptr<thrift_response_client_map> &id_to_client)
    : prot_(std::move(prot)),
      client_(std::make_shared<thrift_response_client>(prot_)),
      registered_client_id_(-1),
      client_id_gen_(client_id_gen),
      id_to_client_(id_to_client)
{
    proxy_ = proxy;
    proxy_type_ = proxy_type;
}

int64_t thrift_handler::thrift_handler::get_client_id() {
    return client_id_gen_.fetch_add(1L);    
}

void thrift_handler::register_client_id(const int32_t block_id, const int64_t client_id) {
    registered_client_id_ = client_id;
    id_to_client_->add_client(client_id, client_);
}

void thrift_handler::async_get(const sequence_id& seq_id, const std::string& key) {
    proxy_->async_get(seq_id, operation_count_++, key);
}

void thrift_handler::async_put(const sequence_id& seq_id, const std::string& key, const std::string& value) {
    proxy_->async_put(seq_id, operation_count_++, key, value);
}

void thrift_handler::async_get_batch(const sequence_id& seq_id, const std::vector<std::string> & keys) {
    proxy_->async_get_batch(seq_id, operation_count_++, keys);
}

void thrift_handler::async_put_batch(const sequence_id& seq_id, const std::vector<std::string> & keys, const std::vector<std::string> & values) {
    proxy_->async_put_batch(seq_id, operation_count_++, keys, values);
}


void thrift_handler::get(std::string& _return, const std::string& key) {
    _return = proxy_->get(operation_count_++, key);
}

void thrift_handler::put(const std::string& key, const std::string& value) {
    proxy_->put(operation_count_++, key, value);
}

void thrift_handler::get_batch(std::vector<std::string> & _return, const std::vector<std::string> &keys) {
    _return = proxy_->get_batch(operation_count_++, keys);
}

void thrift_handler::put_batch(const std::vector<std::string> & keys, const std::vector<std::string> & values) {
    proxy_->put_batch(operation_count_++, keys, values);
}
