#include "dpdk/DpdkPort.hpp"
#include "common/Log.hpp"

extern "C" {
#include <rte_ethdev.h>
#include <rte_mbuf.h>
}

namespace odu {

bool DpdkPort::init(const PortConfig& cfg, rte_mempool* pool) {
  cfg_ = cfg;

  rte_eth_conf port_conf{};
  port_conf.rxmode.max_rx_pkt_len = RTE_ETHER_MAX_LEN;

  int ret = rte_eth_dev_configure(cfg_.port_id, cfg_.rxq + 1, cfg_.txq + 1, &port_conf);
  if (ret < 0) { Log::error("rte_eth_dev_configure failed"); return false; }

  ret = rte_eth_rx_queue_setup(cfg_.port_id, cfg_.rxq, cfg_.nb_rx_desc, rte_eth_dev_socket_id(cfg_.port_id), nullptr, pool);
  if (ret < 0) { Log::error("rx_queue_setup failed"); return false; }

  ret = rte_eth_tx_queue_setup(cfg_.port_id, cfg_.txq, cfg_.nb_tx_desc, rte_eth_dev_socket_id(cfg_.port_id), nullptr);
  if (ret < 0) { Log::error("tx_queue_setup failed"); return false; }

  ret = rte_eth_dev_start(cfg_.port_id);
  if (ret < 0) { Log::error("rte_eth_dev_start failed"); return false; }

  rte_eth_promiscuous_enable(cfg_.port_id);
  up_ = true;
  Log::info("DPDK port %u started", cfg_.port_id);
  return true;
}

uint16_t DpdkPort::rx_burst(void** mbufs, uint16_t max_burst) {
  if (!up_) return 0;
  return rte_eth_rx_burst(cfg_.port_id, cfg_.rxq, reinterpret_cast<rte_mbuf**>(mbufs), max_burst);
}

uint16_t DpdkPort::tx_burst(void** mbufs, uint16_t count) {
  if (!up_) return 0;
  return rte_eth_tx_burst(cfg_.port_id, cfg_.txq, reinterpret_cast<rte_mbuf**>(mbufs), count);
}

} // namespace odu
