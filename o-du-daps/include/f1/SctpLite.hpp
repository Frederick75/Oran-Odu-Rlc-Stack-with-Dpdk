#pragma once
#include <cstdint>
#include <optional>

namespace odu {

enum class CtrlMsgType : uint8_t {
  UE_CONTEXT_SETUP = 0,
  RRC_RECONFIG = 1,
  DAPS_START = 2,
  DAPS_SWITCH_TO_TARGET = 3,
  DAPS_END = 4
};

struct CtrlMsg {
  CtrlMsgType type{};
  uint32_t ue{};
  uint8_t  drb{1};
};

class SctpLite {
public:
  static std::optional<CtrlMsg> parse(const uint8_t* data, uint16_t len);
};

} // namespace odu
