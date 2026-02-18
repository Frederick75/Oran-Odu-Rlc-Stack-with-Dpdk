#pragma once
#include <atomic>
#include <thread>

#include "dpdk/DpdkPort.hpp"
#include "common/Types.hpp"

namespace odu {

class UEContextManager;
class DapsManager;

class F1uDataplane {
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
