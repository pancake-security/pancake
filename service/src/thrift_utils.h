//
// Created by Lloyd Brown on 10/6/19.
//

#ifndef PANCAKE_THRIFT_UTILS_H
#define PANCAKE_THRIFT_UTILS_H

#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TSocket.h>
#include <string>

void wait_for_server_start(const std::string &host, int port){
    bool check = true;
    while (check) {
        using namespace apache::thrift::transport;
        try {
            auto transport = std::shared_ptr<TTransport>(new TFramedTransport(std::make_shared<TSocket>(host, port)));
            transport->open();
            transport->close();
            check = false;
        } catch (TTransportException &e) {
            usleep(100000);
        }
    }
}
#endif //PANCAKE_THRIFT_UTILS_H
