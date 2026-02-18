#include "common/Log.hpp"
#include "dpdk/DpdkEal.hpp"
#include "dpdk/MbufPool.hpp"
#include "dpdk/DpdkPort.hpp"

#include "du/UEContextManager.hpp"
#include "rlc/RlcBearer.hpp"
#include "mac/MacScheduler.hpp"
#include "mac/MacDapsGlue.hpp"
#include "du/DapsManager.hpp"
#include "f1/F1uDataplane.hpp"
#include "f1/F1cControlplane.hpp"

#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>

using namespace odu;

static std::atomic<bool> g_run{true};
static void on_sig(int) { g_run = false; }

int main(int argc, char** argv) {
  std::signal(SIGINT, on_sig);
  Log::set_level(LogLevel::INFO);

  DpdkEal eal;

  // Minimal default: pass only program name.
  // In real deployments, pass DPDK args like: -l 2-6 -n 4 --proc-type=primary ...
  std::vector<std::string> dpdkArgs;
  for (int i = 0; i < argc; ++i) dpdkArgs.emplace_back(argv[i]);

  if (!eal.init(dpdkArgs)) return 1;

  MbufPool pool;
  if (!pool.create("MBUF_POOL", 8192, 256, 0, 2176)) return 1;

  // Demo uses 2 ports:
  //  - port 0: F1-U (user plane)
  //  - port 1: F1-C (control plane)
  DpdkPort f1uPort, f1cPort;
  if (!f1uPort.init({0,0,0,1024,1024}, pool.get())) return 1;
  if (!f1cPort.init({1,0,0,1024,1024}, pool.get())) return 1;

  UEContextManager uecm;
  RlcBearer rlc;
  MacScheduler mac;
  MacDapsGlue macGlue(&mac);
  DapsManager daps(&uecm, &rlc, &macGlue);

  mac.start(&rlc, &uecm);

  // Seed UE + mapping (TEID -> UE/DRB) for demo. In production, F1AP sets this up.
  uecm.ensure_ue(/*ue*/ 1001);
  uecm.bind_f1u_teid(/*ue*/ 1001, /*drb*/ 1, /*teid*/ 0xAABBCCDD);

  F1uDataplane f1u;
  F1cControlplane f1c;
  f1u.start(&f1uPort, &uecm, &daps);
  f1c.start(&f1cPort, &uecm, &daps);

  Log::info("O-DU DAPS runtime started. Press Ctrl+C to stop.");

  while (g_run) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  Log::info("Shutting down...");
  f1c.stop();
  f1u.stop();
  mac.stop();
  eal.cleanup();
  return 0;
}
