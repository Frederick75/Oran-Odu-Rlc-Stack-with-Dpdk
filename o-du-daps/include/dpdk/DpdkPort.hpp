#pragma once
#include <cstdint>

extern "C" {
#include <rte_mempool.h>
}

namespace odu {

struct PortConfig {
  uint16_t port_id{0};
  uint16_t rxq{0};
  uint16_t txq{0};
  uint16_t nb_rx_desc{1024};
  uint16_t nb_tx_desc{1024};
};

class DpdkPort {
public:
  bool init(const PortConfig& cfg, rte_mempool* pool);
  uint16_t port_id() const { return cfg_.port_id; }

  uint16_t rx_burst(void** mbufs, uint16_t max_burst);
  uint16_t tx_burst(void** mbufs, uint16_t count);

private:
  PortConfig cfg_{};
  bool up_{false};
};

} // namespace odu
