//
// Created by Lloyd Brown on 10/5/19.
//

#ifndef PANCAKE_THRIFT_HANDLER_FACTORY_H
#define PANCAKE_THRIFT_HANDLER_FACTORY_H

#include "thrift_handler_factory.h"
#include "thrift_handler.h"
//#include <thrift/protocol/TBinaryProtocol.h>
//#include <thrift/server/TSimpleServer.h>
//#include <thrift/transport/TSocket.h>
//#include <thrift/transport/TServerSocket.h>
//#include <thrift/transport/TNonblockinsgServerSocket.h>
//#include <thrift/transport/TBufferTransports.h>
//#include <thrift/server/TServer.h>
//#include <thrift/server/TNonblockingServer.h>
//#include "../../proxy/src/proxy.h"
#include "thrift.h"

class thrift_handler_factory : public pancake_thriftIfFactory {
public:
    explicit thrift_handler_factory(std::shared_ptr<proxy> proxy, const std::string &proxy_type, 
                                    std::shared_ptr<thrift_response_client_map> id_to_client);
    pancake_thriftIf *getHandler(const ::apache::thrift::TConnectionInfo &connInfo) override;
    void releaseHandler(pancake_thriftIf *anIf) override;

private:
    std::shared_ptr<proxy> proxy_;
    std::string proxy_type_ = "pancake";
    std::shared_ptr<thrift_response_client_map> id_to_client_;
    std::atomic<int64_t> client_id_gen_;
};

#endif //PANCAKE_THRIFT_HANDLER_FACTORY_H
