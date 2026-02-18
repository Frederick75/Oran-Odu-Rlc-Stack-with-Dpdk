#pragma once
#include <atomic>
#include <thread>
#include <unordered_map>
#include <mutex>

#include "common/Types.hpp"

namespace odu {

class RlcBearer;
class UEContextManager;

struct UeMacState {
  bool schedule_source{true};
  bool schedule_target{false};
};

class MacScheduler {
public:
  bool start(RlcBearer* rlc, UEContextManager* uecm);
  void stop();

  void set_target_active(UeId ue, DrbId drb, bool active);
  void switch_to_target(UeId ue, DrbId drb);

private:
  void tti_loop();

  std::atomic<bool> run_{false};
  std::thread th_{};

  RlcBearer* rlc_{nullptr};
  UEContextManager* uecm_{nullptr};

  std::mutex mu_;
  std::unordered_map<UeId, UeMacState> ue_state_;
};

} // namespace odu
