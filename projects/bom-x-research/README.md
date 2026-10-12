# BOM-X — AI / Semiconductor Research Engine

BOM-X is a self-contained research terminal that translates AI/software demand into **compute requirements, semiconductor BOM demand, physical supply constraints, supplier earnings, and expectations gaps**.

The core question is:

> If AI demand changes upstream, what physical semiconductor and infrastructure demand follows — and what part of that implication is not yet reflected in market expectations?

## Research chain

```text
AI software demand
      ↓
compute / accelerator-hours
      ↓
physical BOM
      ↓
capacity / yield / inventory constraints
      ↓
supplier revenue / margin / EPS
      ↓
consensus / revisions / implied expectations
      ↓
research signal
```

## Production verticals

The current prototype carries the same installed-accelerator state through five physical verticals:

1. **Memory** — HBM, DRAM, NAND, wafer-capacity pressure and contract pricing.
2. **Silicon** — foundry demand, die equivalents, yield, advanced packaging and bottleneck loading.
3. **Networking / optics** — ports, optical modules, bandwidth generation and silicon content.
4. **Power / cooling** — IT MW, facility MW, racks, CDUs and electrical infrastructure.
5. **WFE / test** — capacity gaps, wafer-equivalent starts, ATE seconds and tool demand.

## Model-risk controls

BOM-X makes the uncertain bridges visible instead of hiding them:

- explicit utilization, spare-capacity and workload assumptions;
- bounded price-response functions;
- event deduplication to avoid double/triple counting one physical build;
- sensitivity analysis around ASP, supplier share and spare capacity;
- point-in-time discipline for backtests and expectations data;
- event-level clustering and multiple-testing controls.

## Run

The project is a self-contained HTML application.

Open [`index.html`](./index.html) in a modern browser. No build step or backend is required.

## Status

This is a research prototype, not an investment recommendation or a production forecasting system. Historical consensus, revision, factor-return and transaction-cost data remain separate data-engineering requirements; the interface deliberately marks unavailable inputs rather than fabricating them.
