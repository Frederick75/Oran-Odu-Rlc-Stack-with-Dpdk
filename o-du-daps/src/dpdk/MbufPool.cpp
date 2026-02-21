#include "dpdk/MbufPool.hpp"
#include "common/Log.hpp"

extern "C" {
#include <rte_mbuf.h>
#include <rte_lcore.h>
#include <rte_eal.h>
}

namespace odu {

bool MbufPool::create(const std::string& name, uint32_t num_mbufs, uint32_t cache_size, uint16_t priv_size, uint16_t data_room) {
  pool_ = rte_pktmbuf_pool_create(
    name.c_str(),
    num_mbufs,
    cache_size,
    priv_size,
    data_room,
    rte_socket_id()
  );
  if (!pool_) {
    Log::error("Failed to create mbuf pool: %s", name.c_str());
    return false;
  }
  Log::info("Created mbuf pool: %s", name.c_str());
  return true;
}

} // namespace odu
