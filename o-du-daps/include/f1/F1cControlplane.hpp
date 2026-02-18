#pragma once
#include <atomic>
#include <thread>

#include "dpdk/DpdkPort.hpp"

namespace odu {

class UEContextManager;
class DapsManager;

class F1cControlplane {
public:
  bool start(DpdkPort* port, UEContextManager* uecm, DapsManager* daps);
  void stop();

private:
  void rx_loop();

  std::atomic<bool> run_{false};
  std::thread th_{};

  DpdkPort* port_{nullptr};
  UEContextManager* uecm_{nullptr};
  DapsManager* daps_{nullptr};
};

} // namespace odu
