#pragma once
#include "common/Types.hpp"

namespace odu {

class MacScheduler;

class MacDapsGlue {
public:
  explicit MacDapsGlue(MacScheduler* sched) : sched_(sched) {}

  void on_daps_state_change(UeId ue, DrbId drb, bool targetActive);
  void on_switch_to_target(UeId ue, DrbId drb);

private:
  MacScheduler* sched_{nullptr};
};

} // namespace odu
