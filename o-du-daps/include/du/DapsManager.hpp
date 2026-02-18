#pragma once
#include <cstdint>
#include "common/Types.hpp"

namespace odu {

class UEContextManager;
class RlcBearer;
class MacDapsGlue;

class DapsManager {
public:
  DapsManager(UEContextManager* uecm, RlcBearer* rlc, MacDapsGlue* mac)
    : uecm_(uecm), rlc_(rlc), mac_(mac) {}

  // Control-plane driven
  void start_daps(UeId ue, DrbId drb);
  void switch_to_target(UeId ue, DrbId drb);
  void end_daps(UeId ue, DrbId drb);

  // User-plane driven (from F1-U RX)
  void on_f1u_downlink_pdu(UeId ue, DrbId drb, const uint8_t* p, uint16_t n);

private:
  UEContextManager* uecm_{nullptr};
  RlcBearer* rlc_{nullptr};
  MacDapsGlue* mac_{nullptr};
};

} // namespace odu
