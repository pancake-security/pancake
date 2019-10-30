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
    explicit thrift_handler_factory(std::shared_ptr<proxy> proxy, const std::string &proxy_type);
    pancake_thriftIf *getHandler(const ::apache::thrift::TConnectionInfo &connInfo) override;
    void releaseHandler(pancake_thriftIf *anIf) override;

private:
    std::shared_ptr<proxy> proxy_;
    std::string proxy_type_ = "pancake";
};

#endif //PANCAKE_THRIFT_HANDLER_FACTORY_H
