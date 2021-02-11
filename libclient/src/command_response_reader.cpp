#include "command_response_reader.h"

command_response_reader::command_response_reader(std::shared_ptr<apache::thrift::protocol::TProtocol> prot)
                                   : prot_(std::move(prot)) {
  iprot_ = prot_.get();
}

int64_t command_response_reader::recv_response(std::vector<std::string> &out) {
  
  using namespace ::apache::thrift::protocol;
  using namespace ::apache::thrift;
  int32_t rseqid = 0;
  std::string fname;
  TMessageType mtype;

  this->iprot_->readMessageBegin(fname, mtype, rseqid);
  if (mtype == T_EXCEPTION) {
    TApplicationException x;
    x.read(this->iprot_);
    this->iprot_->readMessageEnd();
    this->iprot_->getTransport()->readEnd();
    throw x;
  }
  pancake_thrift_response_async_response_args result;
  result.read(this->iprot_);
  this->iprot_->readMessageEnd();
  this->iprot_->getTransport()->readEnd();
  if (result.__isset.seq_id && result.__isset.result) {
    out = result.result;
    return result.seq_id.client_seq_no;
  }
}
