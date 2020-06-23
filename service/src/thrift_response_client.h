#include <thrift/transport/TSocket.h>
#include "thrift_response_service.h"

/* thrift response client */
class thrift_response_client {
 public:
  typedef pancake_thrift_responseClient thrift_client;

  /**
   * @brief Constructor
   * @param protocol Protocol
   */

  explicit thrift_response_client(std::shared_ptr<apache::thrift::protocol::TProtocol> protocol);

  /**
   * @brief Response
   * @param seq Sequence identifier
   * @param result Operation result
   */

  void async_response(const sequence_id& seq_id, const int32_t op_code, const std::vector<std::string> & result);

 private:
  /* thrift response service client */
  std::shared_ptr<thrift_client> client_{};
};