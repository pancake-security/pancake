
#include "thrift_response_client.h"

#include <thrift/protocol/TBinaryProtocol.h>
#include <iostream>

using namespace ::apache::thrift::protocol;

thrift_response_client::thrift_response_client(std::shared_ptr<TProtocol> protocol)
    : client_(std::make_shared<thrift_client>(protocol)) {}

void thrift_response_client::async_get_response(const sequence_id &seq, const std::string &result) {
  client_->async_get_response(seq, result);
}

void thrift_response_client::async_put_response(const sequence_id &seq, const std::string &result) {
  client_->async_get_response(seq, result);
}

void thrift_response_client::async_get_batch_response(const sequence_id &seq, const std::vector<std::string> &result) {
  client_->async_get_batch_response(seq, result);
}

void thrift_response_client::async_put_batch_response(const sequence_id &seq, const std::string &result) {
  client_->async_put_batch_response(seq, result);
}