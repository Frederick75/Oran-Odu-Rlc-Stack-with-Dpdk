#pragma once
#include <string>
#include <cstdint>

extern "C" {
#include <rte_mempool.h>
}

namespace odu {

class MbufPool {
public:
  bool create(const std::string& name, uint32_t num_mbufs, uint32_t cache_size, uint16_t priv_size, uint16_t data_room);
  rte_mempool* get() const { return pool_; }
private:
  rte_mempool* pool_{nullptr};
};

} // namespace odu
