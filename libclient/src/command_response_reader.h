/* Command response reader class */
#include <thrift/transport/TSocket.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TTransportUtils.h>
#include <string>
#include <vector>
#include <atomic>
#include <thread>
#include <iostream>

#include "thrift.h"
#include "thrift_response_service.h"

class command_response_reader {
public:

    command_response_reader() = default;

    command_response_reader(std::shared_ptr<apache::thrift::protocol::TProtocol> prot);

    int64_t recv_response(std::vector<std::string> &out);
    /* Thrift protocol */
    std::shared_ptr<apache::thrift::protocol::TProtocol> prot_;
    /* Thrift protocol */
    apache::thrift::protocol::TProtocol *iprot_{};
};