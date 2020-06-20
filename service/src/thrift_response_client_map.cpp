#include "thrift_response_client_map.h"

thrift_response_client_map::thrift_response_client_map() : clients_(0) {
}

void thrift_response_client_map::add_client(int64_t client_id, std::shared_ptr<thrift_response_client> client) {
  clients_.insert(client_id, client);
}

void thrift_response_client_map::remove_client(int64_t client_id) {
  clients_.erase(client_id);
}

void thrift_response_client_map::async_respond_client(const sequence_id &seq, int op_code, const std::vector<std::string> &result) {
  if (seq.client_id == -1)
    return;
  bool found = clients_.update_fn(seq.client_id, [&](std::shared_ptr<thrift_response_client> &client) {
    client->async_response(seq, op_code, result);
  });
  //if (!found)
  //  std::runtime_error("ERROR: client with corresponding id not found");
    //LOG(log_level::warn) << "Cannot respond to client since client id " << seq.client_id << " is not registered...";
}

void thrift_response_client_map::clear() {
  clients_.clear();
}

void thrift_response_client_map::send_failure() {
  sequence_id fail;
  fail.__set_client_id(-2);
  fail.__set_client_seq_no(-2);
  fail.__set_client_id(-2);
  for (const auto &x : clients_.lock_table()) {
  try {
      std::vector<std::string> _result;
      x.second->async_response(fail, 5, _result);
  } 
  catch (std::exception &e) {
    continue;
    }
  }
}