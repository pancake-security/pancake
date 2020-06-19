#include <thrift/transport/TSocket.h>
#include "thrift_response_service.h"

/* thrift response client */
class thrift_response_client {
 public:
  typedef thrift_response_serviceClient thrift_client;

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

  void async_get_response(const sequence_id &seq, const std::string &result);
  void async_put_response(const sequence_id &seq, const std::string &result);
  void async_get_batch_response(const sequence_id &seq, const std::vector<std::string> &result);
  void async_put_batch_response(const sequence_id &seq, const std::string &result);

 private:
  /* thrift response service client */
  std::shared_ptr<thrift_client> client_{};
};

#endif //JIFFY_thrift_RESPONSE_CLIENT_H