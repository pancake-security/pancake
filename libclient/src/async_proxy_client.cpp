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
    seq_id_ = sequence_id();
    seq_id_.__set_client_id(client_id_);
}

int64_t async_proxy_client::get_client_id() {
  return client_->get_client_id();
}


async_proxy_client::command_response_reader async_proxy_client::get_command_response_reader(int64_t client_id) {
  auto block_id_ = 1;
  client_->register_client_id(block_id_, client_id);
  return async_proxy_client::command_response_reader(protocol_);
}

async_proxy_client::command_response_reader::command_response_reader(std::shared_ptr<apache::thrift::protocol::TProtocol> prot)
    : prot_(std::move(prot)) {
  iprot_ = prot_.get();
}

int64_t async_proxy_client::command_response_reader::recv_response(std::vector<std::string> &out) {
  using namespace ::apache::thrift::protocol;
  using namespace ::apache::thrift;
  /*
  int32_t rseqid = 0;
  std::string fname;
  TMessageType mtype;

  this_resp.iprot_->readMessageBegin(fname, mtype, rseqid);
  if (mtype == T_EXCEPTION) {
    TApplicationException x;
    x.read(this_resp.iprot_);
    this_resp.iprot_->readMessageEnd();
    this_resp.iprot_->getTransport()->readEnd();
    throw x;
  }
  pancake_thrift_response_async_response_pargs result;
  result.read(this_resp.iprot_);
  this_resp.iprot_->readMessageEnd();
  this_resp.iprot_->getTransport()->readEnd();
  if (result.__isset.seq && result.__isset.result) {
    out = result.result;
    return result.seq.client_seq_no;
  }
  throw TApplicationException(TApplicationException::MISSING_RESULT, "Command failed: unknown result");
  */
  ::apache::thrift::protocol::TInputRecursionTracker tracker(*this->iprot_);
  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += this->iprot_->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;

  auto this_resp = pancake_thrift_response_async_response_args();
  while (true)
  {
    xfer += this->iprot_->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRUCT) {
          xfer += this_resp.seq_id.read(this->iprot_);
          this_resp.__isset.seq_id = true;
        } else {
          xfer += this->iprot_->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += this->iprot_->readI32(this_resp.op_code);
          this_resp.__isset.op_code = true;
        } else {
          xfer += this->iprot_->skip(ftype);
        }
        break;
      case 3:
        if (ftype == ::apache::thrift::protocol::T_LIST) {
          {
            this_resp.result.clear();
            uint32_t _size55;
            ::apache::thrift::protocol::TType _etype58;
            xfer += this->iprot_->readListBegin(_etype58, _size55);
            this_resp.result.resize(_size55);
            uint32_t _i59;
            for (_i59 = 0; _i59 < _size55; ++_i59)
            {
              xfer += this->iprot_->readString(this_resp.result[_i59]);
            }
            xfer += this->iprot_->readListEnd();
          }
          this_resp.__isset.result = true;
        } else {
          xfer += this->iprot_->skip(ftype);
        }
        break;
      default:
        xfer += this->iprot_->skip(ftype);
        break;
    }
    xfer += this->iprot_->readFieldEnd();
  }

  xfer += this->iprot_->readStructEnd();
  out = this_resp.result;
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
    auto reader = get_command_response_reader(client_id_);
    while (!done_) {
        std::cout << "entering read responses" << std::endl;
        auto type = requests_->pop();
        std::cout << "popped" << std::endl;
        std::vector<std::string> _return;
        reader.recv_response(_return);
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
}
