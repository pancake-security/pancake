//
// Created by Lloyd Brown on 10/3/19.
//

#ifndef PANCAKE_PANCAKE_THRIFT_SERVER_H
#define PANCAKE_PANCAKE_THRIFT_SERVER_H
#include "../../gen-cpp/pancake_thrift.h"
#include "proxy.h"
#include "pancake_proxy.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/server/TServer.h>
#include <thrift/server/TNonblockingServer.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

class pancake_thriftHandler : virtual public pancake_thriftIf {
public:
    pancake_thriftHandler();

    pancake_thriftHandler(distribution dist);

    void get(std::string& _return, const std::string& key);

    void put(std::string& _return, const std::string& key, const std::string& value);

    void get_batch(std::vector<std::string> & _return, const std::vector<std::string> & keys);

    void put_batch(std::string& _return, const std::vector<std::string> & keys, const std::vector<std::string> & values);

    void usage();

    int main(int argc, char **argv);

private:
    int operation_count_ = 0;
    proxy * proxy_;
    std::string proxy_type_ = "pancake";
    distribution distribution_;

};
#endif //PANCAKE_PANCAKE_THRIFT_SERVER_H
