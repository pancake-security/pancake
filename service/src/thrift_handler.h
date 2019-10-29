//
// Created by Lloyd Brown on 10/3/19.
//

#ifndef PANCAKE_THRIFT_HANDLER_H
#define PANCAKE_THRIFT_HANDLER_H
#include "thrift.h"
#include "proxy.h"
#include "pancake_proxy.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TNonblockingServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/server/TServer.h>
#include <thrift/server/TNonblockingServer.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

class thrift_handler : virtual public pancake_thriftIf {
public:
    thrift_handler();

    thrift_handler(std::shared_ptr<proxy> proxy, const std::string &proxy_type);

    void get(std::string& _return, const std::string& key);

    void put(std::string& _return, const std::string& key, const std::string& value);

    void get_batch(std::vector<std::string> & _return, const std::vector<std::string> & keys);

    void put_batch(std::string& _return, const std::vector<std::string> & keys, const std::vector<std::string> & values);

private:
    int operation_count_ = 0;
    std::shared_ptr<proxy> proxy_;
    std::string proxy_type_ = "pancake";

};
#endif //PANCAKE_THRIFT_HANDLER_H
