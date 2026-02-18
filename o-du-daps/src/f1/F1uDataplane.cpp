#include "f1/F1uDataplane.hpp"
#include "f1/GtpU.hpp"
#include "du/UEContextManager.hpp"
#include "du/DapsManager.hpp"
#include "common/Log.hpp"

extern "C" {
#include <rte_mbuf.h>
}

namespace odu {

bool F1uDataplane::start(DpdkPort* port, UEContextManager* uecm, DapsManager* daps) {
  port_ = port; uecm_ = uecm; daps_ = daps;
  run_ = true;
  th_ = std::thread([this]{ rx_loop(); });
  Log::info("F1-U dataplane started");
  return true;
}

void F1uDataplane::stop() {
  run_ = false;
  if (th_.joinable()) th_.join();
  Log::info("F1-U dataplane stopped");
}

void F1uDataplane::rx_loop() {
  constexpr uint16_t BURST = 32;
  void* bufs[BURST];

  while (run_) {
    uint16_t n = port_->rx_burst(bufs, BURST);
    for (uint16_t i = 0; i < n; ++i) {
      auto* m = reinterpret_cast<rte_mbuf*>(bufs[i]);
      uint16_t len = rte_pktmbuf_pkt_len(m);
      const uint8_t* data = rte_pktmbuf_mtod(m, const uint8_t*);

      auto pdu = GtpU::parse(data, len);
      if (pdu) {
        auto map = uecm_->lookup_f1u_teid(pdu->teid);
        if (map) {
          auto [ue, drb] = *map;
          daps_->on_f1u_downlink_pdu(ue, drb, pdu->payload, pdu->payload_len);
        }
      }

      rte_pktmbuf_free(m);
    }
  }
}

} // namespace odu
