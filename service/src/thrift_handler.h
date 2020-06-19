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

    void async_get(const sequence_id& seq_id, const std::string& key);

    void put(std::string& _return, const std::string& key, const std::string& value);

    void async_put(const sequence_id& seq_id, const std::string& key, const std::string& value);

    void get_batch(std::vector<std::string> & _return, const std::vector<std::string> & keys);

    void async_get_batch(const sequence_id& seq_id, const std::vector<std::string> & keys);

    void put_batch(std::string& _return, const std::vector<std::string> & keys, const std::vector<std::string> & values);

    void async_put_batch(const sequence_id& seq_id, const std::vector<std::string> & keys, const std::vector<std::string> & values);

private:
    int operation_count_ = 0;
    std::shared_ptr<proxy> proxy_;
    std::string proxy_type_ = "pancake";

    /* Block response client */
    std::shared_ptr<pancake_thrift_response_client> client_;

    /* Registered client identifier */
    int64_t registered_client_id_;

    /* Client identifier generator */
    std::atomic<int64_t> &client_id_gen_;

    std::shared_ptr<thrift_response_client_map> &id_to_client_;
};
#endif //PANCAKE_THRIFT_HANDLER_H
