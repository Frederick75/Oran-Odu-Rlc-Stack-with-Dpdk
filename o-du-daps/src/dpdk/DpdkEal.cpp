#include "dpdk/DpdkEal.hpp"
#include "common/Log.hpp"

extern "C" {
#include <rte_eal.h>
}

namespace odu {

DpdkEal::~DpdkEal() { cleanup(); }

bool DpdkEal::init(const std::vector<std::string>& args) {
  if (initialized_) return true;

  std::vector<char*> argv;
  argv.reserve(args.size());
  for (auto& s : args) argv.push_back(const_cast<char*>(s.c_str()));

  int ret = rte_eal_init((int)argv.size(), argv.data());
  if (ret < 0) {
    Log::error("DPDK EAL init failed");
    return false;
  }
  initialized_ = true;
  Log::info("DPDK EAL initialized");
  return true;
}

void DpdkEal::cleanup() {
  if (!initialized_) return;
#if defined(RTE_VERSION_NUM)
  rte_eal_cleanup();
#endif
  initialized_ = false;
  Log::info("DPDK EAL cleaned up");
}

} // namespace odu
