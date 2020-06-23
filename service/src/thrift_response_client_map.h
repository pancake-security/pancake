#include <cstdint>
#include <libcuckoo/cuckoohash_map.hh>
#include "thrift_response_client.h"


/* thrift response client map class
 * This map records all the client that sent request to the partition.
 * It maps data from client identifier to partition response client
 */
class thrift_response_client_map {
 public:

  /**
   * @brief Constructor
   */

  thrift_response_client_map();

  /**
   * @brief Add a client to the map
   * @param client_id Client identifier
   * @param client thrift response client
   */

  void add_client(int64_t client_id, std::shared_ptr<thrift_response_client> client);

  /**
   * @brief Remove a client from the map
   * @param client_id Client identifier
   */

  void remove_client(int64_t client_id);

  /**
   * @brief Respond to the client
   * @param seq Request sequence identifier
   * @param result Request result
   */

  void async_respond_client(const sequence_id &seq, int op_code, const std::vector<std::string> &result);

  /**
   * @brief Respond to the client
   * @param seq Request sequence identifier
   * @param result Request result
   */

  void clear();

  /**
   * @brief Send failing request when thrift is deleted
   */
  void send_failure();

 private:
  /* Response client map */
  libcuckoo::cuckoohash_map<int64_t, std::shared_ptr<thrift_response_client>> clients_;
};