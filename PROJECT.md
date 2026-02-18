# Project Explanation & Architecture — O-DU DAPS (RLC/MAC) with DPDK Acceleration

## 1. Goal

Implement **Dual Active Protocol Stack (DAPS)** capability inside the **O-RAN O-DU** at **RLC/MAC** so that an intra-cell (or intra-DU) handover can be executed with **zero service interruption** during the DAPS window.

At the same time, accelerate **midhaul** interfaces using **DPDK**:
- **F1-U (User Plane)**: high-throughput delivery of PDCP/SDAP payloads over **GTP-U**
- **F1-C (Control Plane)**: low-latency processing of mobility control events (placeholder for SCTP + F1AP)

## 2. Where this fits in O-RAN / 3GPP

Typical functional split:

```
     +-------------------+              +-------------------+
     |        O-CU       |              |         RU        |
     |-------------------|              |-------------------|
     | RRC / PDCP        |              | PHY / RF          |
     +---------+---------+              +---------+---------+
               |  F1-C / F1-U                      |
               v                                    |
     +---------+---------+                          |
     |        O-DU       |  Fronthaul (not modeled) |
     |-------------------|--------------------------+
     | MAC / RLC  (DAPS) |
     +-------------------+
```

This repo focuses on the **O-DU** responsibilities relevant to DAPS:
- Maintaining **two active legs** (SOURCE and TARGET) during the DAPS window
- Delivering user-plane data into **RLC buffers** for both legs (duplication policy shown)
- Steering MAC scheduling so that both legs can be served, then cleanly switching to TARGET only

## 3. Key concepts

### What DAPS means (high-level)

DAPS enables **simultaneous connectivity** to SOURCE and TARGET legs during handover:

- **Before DAPS**: SOURCE active
- **During DAPS window**: SOURCE + TARGET active (dual stack)
- **After switch**: TARGET active, SOURCE released

In this repo, DAPS is modeled per **(UE, DRB)** using `DapsBearerState`.

## 4. Architecture of this codebase

### Data plane (F1-U) — DPDK accelerated

- `dpdk::DpdkPort` provides RX bursts from a DPDK port/queue
- `f1::F1uDataplane` parses minimal **GTP-U** (`GtpU::parse`)
- TEID is mapped to `(UE, DRB)` via `UEContextManager::lookup_f1u_teid`
- Payload is handed to `DapsManager::on_f1u_downlink_pdu()`

```
DPDK RX (port0) -> F1uDataplane -> GTP-U parse -> TEID map -> DapsManager
                                                     |
                                                     v
                                            RLC Bearers (SRC/TGT)
```

### Control plane (F1-C) — DPDK accelerated (placeholder)

Real deployments: **SCTP + F1AP ASN.1**.
This repo provides a **control-event path** to show where DPDK-accelerated control RX would drive mobility and DAPS state.

- `f1::F1cControlplane` receives demo control messages from DPDK port1
- `SctpLite` is a tiny placeholder parser
- DAPS state is driven by: `start_daps`, `switch_to_target`, `end_daps`

```
DPDK RX (port1) -> F1cControlplane -> SctpLite parse -> DapsManager state transitions
```

### DAPS Core (RLC/MAC coordination)

- `DapsManager` is the **central policy engine**
  - During DAPS: duplicates downlink SDU into both RLC legs (simplified)
  - After switch/end: routes downlink into TARGET leg only
- `RlcBearer` holds per-UE/DRB entities for SOURCE and TARGET legs
- `MacScheduler` models a 1ms TTI loop and fetches bytes from RLC for each active leg
- `MacDapsGlue` updates scheduler behavior based on DAPS state

```
             +---------------------- Control-plane ----------------------+
             |                                                          |
DPDK RX -> F1cControlplane -> DapsManager -> MacDapsGlue -> MacScheduler |
             |                                                          |
             +---------------------- User-plane -------------------------+

DPDK RX -> F1uDataplane -> DapsManager -> RlcBearer(SRC/TGT) -> MacScheduler(TTI)
```

## 5. Source tree guide

- `include/dpdk/*` — minimal DPDK wrappers (EAL, mempool, port)
- `include/f1/*`    — F1-U dataplane + minimal GTP-U parsing; F1-C control placeholder
- `include/du/*`    — UE contexts + DAPS manager
- `include/rlc/*`   — RLC entities + dual-leg bearer manager
- `include/mac/*`   — MAC scheduler + DAPS glue

## 6. DAPS behavior implemented (what to look at)

- **Start DAPS**: `DapsManager::start_daps()`
  - Marks SOURCE + TARGET active for a DRB
  - Enables TARGET scheduling in MAC

- **Switch to target**: `DapsManager::switch_to_target()`
  - Stops SOURCE scheduling (MAC)
  - Keeps TARGET scheduling active

- **End DAPS**: `DapsManager::end_daps()`
  - Finalizes TARGET-only state
  - Flushes SOURCE leg buffers

- **Downlink delivery policy**: `DapsManager::on_f1u_downlink_pdu()`
  - During DAPS (pre-switch): duplicates SDU to SOURCE and TARGET legs (zero interruption model)
  - After switch/end: routes to TARGET only

## 7. Build & run notes

- This code expects **two DPDK ports**:
  - `port 0` for F1-U
  - `port 1` for F1-C

In production, you would typically use one NIC with:
- multiple queues (RSS)
- `rte_flow` rules to steer control vs user traffic

## 8. Production extensions (next steps)

To make it a full DU stack:
- Replace `SctpLite` with real **SCTP + F1AP ASN.1**
- Replace simplified RLC with full **AM/UM** (SN, reassembly, status, retransmission)
- Replace MAC loop with full scheduler/HARQ/LCG prioritization
- Integrate PHY/RU fronthaul (O-RAN FH / eCPRI)
- Add `rte_flow` steering, per-UE QoS, per-slice policies
- Add metrics export (Prometheus) and structured tracing

---

## Quick mental model

- **DPDK** makes the F1-U/F1-C packet input fast.
- **DapsManager** decides whether to feed SOURCE, TARGET, or both.
- **RLC** buffers per-leg data.
- **MAC** schedules which leg gets served each TTI.
