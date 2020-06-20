
#include "thrift_response_client.h"

#include <thrift/protocol/TBinaryProtocol.h>
#include <iostream>

using namespace ::apache::thrift::protocol;

thrift_response_client::thrift_response_client(std::shared_ptr<TProtocol> protocol)
    : client_(std::make_shared<thrift_client>(protocol)) {}

void thrift_response_client::async_response(const sequence_id& seq_id, const int32_t op_code, const std::vector<std::string> & result) {
  client_->async_response(seq_id, op_code, result);
}