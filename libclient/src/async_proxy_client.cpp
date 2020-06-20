//
// Created by Lloyd Brown on 10/1/19.
//

#include "async_proxy_client.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;

void async_proxy_client::init(const std::string &host_name, int port) {
    auto socket = std::make_shared<TSocket>(host_name, port);
    socket->setRecvTimeout(10000);
    auto transport = std::shared_ptr<TTransport>(new TFramedTransport(socket));
    auto protocol = std::shared_ptr<TProtocol>(new TBinaryProtocol(transport));
    client_ = std::make_shared<pancake_thriftConcurrentClient>(protocol);
    transport->open();
    requests_ = std::make_shared<queue<int>>();
    client_id_ = get_client_id();
    std::cout << "Client id: " << client_id_ << std::endl;
    seq_id_ = sequence_id();
    seq_id_.__set_client_id(client_id_);
    response_thread_ = new std::thread(&async_proxy_client::read_responses, this);
    reader_ = command_response_reader(protocol_);
}

int64_t async_proxy_client::get_client_id() {
  auto id = client_->get_client_id();
  auto block_id_ = 1;
  client_->register_client_id(block_id_, id);
  return id;
}


std::string async_proxy_client::get(const std::string &key) {
    seq_id_.__set_client_seq_no(sequence_num++);
    client_->async_get(seq_id_, key);
    requests_->push(GET);
    return "";
}

void async_proxy_client::put(const std::string &key, const std::string &value) {
    seq_id_.__set_client_seq_no(sequence_num++);
    client_->async_put(seq_id_, key, value);
    requests_->push(PUT);
}

std::vector<std::string> async_proxy_client::get_batch(const std::vector<std::string> &keys) {
    seq_id_.__set_client_seq_no(sequence_num++);
    std::cout << "Seq id: " << seq_id_ << std::endl;
    client_->async_get_batch(seq_id_, keys);
    requests_->push(GET_BATCH);
    std::vector<std::string> fake_vec;
    return fake_vec;
}

void async_proxy_client::put_batch(const std::vector<std::string> &keys, const std::vector<std::string> &values) {
    seq_id_.__set_client_seq_no(sequence_num++);
    client_->async_put_batch(seq_id_, keys, values);
    requests_->push(PUT_BATCH);
}

void async_proxy_client::read_responses() { 
    //auto reader = get_command_response_reader(client_id_);
    while (!done_) {
        std::cout << "entering read responses" << std::endl;
        auto type = requests_->pop();
        std::cout << "popped" << std::endl;
        std::vector<std::string> _return;
        reader_.recv_response(_return);
        total_ += _return.size();
    }
}

int async_proxy_client::num_requests_satisfied(){
    return total_;
}

void async_proxy_client::finish(){
    done_ = true;
    for (int i = 0; i < 20; i++){
        async_proxy_client::put("0", "0");
    }
    response_thread_->join();
}
