//
// Created by Lloyd Brown on 10/5/19.
//
#include "thrift_handler_factory.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;

thrift_handler_factory::thrift_handler_factory(std::shared_ptr<proxy> proxy, const std::string &proxy_type) {
    proxy_ = proxy;
    proxy_type_ = proxy_type;
}

pancake_thriftIf* thrift_handler_factory::getHandler(const ::apache::thrift::TConnectionInfo &conn_info) {
    std::shared_ptr<TSocket> sock = std::dynamic_pointer_cast<TSocket>(conn_info.transport);
    auto transport = std::make_shared<TFramedTransport>(conn_info.transport);
    std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    return new thrift_handler(proxy_, proxy_type_);
}

void thrift_handler_factory::releaseHandler(pancake_thriftIf *handler) {
    auto br_handler = reinterpret_cast<thrift_handler *>(handler);
    delete handler;
}
