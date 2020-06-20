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
  //pancake_thrift_response_async_response_args
  pancake_thrift_response_async_response_args result;
  result.read(this->iprot_);
  this->iprot_->readMessageEnd();
  this->iprot_->getTransport()->readEnd();
  if (result.__isset.seq_id && result.__isset.result) {
    out = result.result;
    return result.seq_id.client_seq_no;
  }
  throw TApplicationException(TApplicationException::MISSING_RESULT, "Command failed: unknown result");

  
  /*
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
  */
  
}
