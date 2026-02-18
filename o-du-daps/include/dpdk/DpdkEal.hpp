#pragma once
#include <vector>
#include <string>

namespace odu {

class DpdkEal {
public:
  // args example: {"o_du_daps", "-l", "2-6", "-n", "4"}
  bool init(const std::vector<std::string>& args);
  void cleanup();

  DpdkEal() = default;
  ~DpdkEal();

  DpdkEal(const DpdkEal&) = delete;
  DpdkEal& operator=(const DpdkEal&) = delete;

private:
  bool initialized_{false};
};

} // namespace odu
