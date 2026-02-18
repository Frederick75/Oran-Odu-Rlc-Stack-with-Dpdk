#include "f1/F1cControlplane.hpp"
#include "f1/SctpLite.hpp"
#include "du/UEContextManager.hpp"
#include "du/DapsManager.hpp"
#include "common/Log.hpp"

extern "C" {
#include <rte_mbuf.h>
}

namespace odu {

bool F1cControlplane::start(DpdkPort* port, UEContextManager* uecm, DapsManager* daps) {
  port_ = port; uecm_ = uecm; daps_ = daps;
  run_ = true;
  th_ = std::thread([this]{ rx_loop(); });
  Log::info("F1-C controlplane started");
  return true;
}

void F1cControlplane::stop() {
  run_ = false;
  if (th_.joinable()) th_.join();
  Log::info("F1-C controlplane stopped");
}

void F1cControlplane::rx_loop() {
  constexpr uint16_t BURST = 32;
  void* bufs[BURST];

  while (run_) {
    uint16_t n = port_->rx_burst(bufs, BURST);
    for (uint16_t i = 0; i < n; ++i) {
      auto* m = reinterpret_cast<rte_mbuf*>(bufs[i]);
      uint16_t len = rte_pktmbuf_pkt_len(m);
      const uint8_t* data = rte_pktmbuf_mtod(m, const uint8_t*);

      auto msg = SctpLite::parse(data, len);
      if (msg) {
        switch (msg->type) {
          case CtrlMsgType::UE_CONTEXT_SETUP:
            uecm_->ensure_ue(msg->ue);
            break;
          case CtrlMsgType::DAPS_START:
            daps_->start_daps(msg->ue, msg->drb);
            break;
          case CtrlMsgType::DAPS_SWITCH_TO_TARGET:
            daps_->switch_to_target(msg->ue, msg->drb);
            break;
          case CtrlMsgType::DAPS_END:
            daps_->end_daps(msg->ue, msg->drb);
            break;
          default:
            break;
        }
      }

      rte_pktmbuf_free(m);
    }
  }
}

} // namespace odu
