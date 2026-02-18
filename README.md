# O-DU DAPS (RLC/MAC) with DPDK-Accelerated F1-U/F1-C

This repository is a **production-shaped, compilable C++17 codebase** that models:
- **Dual Active Protocol Stack (DAPS)** behavior across **O-DU RLC/MAC**
- **DPDK-accelerated** RX paths for **F1-U (GTP-U)** and **F1-C (control events)**

> Note: This is a realistic implementation skeleton (threading, interfaces, data paths, state machines).
> A full 3GPP/O-RAN DU stack (full RLC AM/UM, full F1AP ASN.1 over SCTP, scheduler/HARQ, PHY/FH) is out of scope,
> but the core DAPS mechanics and acceleration points are represented cleanly.

## Build

Prerequisites:
- DPDK installed with `pkg-config` providing `libdpdk`
- Hugepages configured
- NIC(s) bound to a DPDK driver (vfio-pci/uio)

```bash
mkdir -p build && cd build
cmake ..
make -j
```

## Run

Example (your platform may need different EAL args):
```bash
sudo ./o_du_daps -l 2-6 -n 4
```

## Documentation

See `docs/PROJECT.md` for project explanation and architecture.
