# Methodology Notes

## Structural alpha

The conceptual research signal is:

```text
StructuralAlpha(i,t) = EconomicImplication(i,t) - MarketExpectation(i,t)
```

The difficult part is not the subtraction. It is building a defensible bridge from upstream demand to physical units and supplier economics.

## Demand backsolver

A simplified contract-driven path is:

```text
AcceleratorHours = Commitment × ComputeShare / EffectiveAcceleratorHourCost
ContinuousAccelerators = AcceleratorHours / 8,760
InstalledAccelerators = ContinuousAccelerators × Reserve / Utilization × (1 - SpareCapacity)
HBM = InstalledAccelerators × HBM_GB_per_accelerator
Storage = InstalledAccelerators × TB_per_accelerator
```

A separate workload path provides a cross-check using users, query intensity, tokens, agentic amplification and GPU-seconds per 1k tokens.

## Why the model is explicit

Every important bridge can be wrong: utilization, effective compute price, architecture mix, storage intensity, HBM per accelerator, packaging yield, optical attach rate, rack density, supplier share and pricing pass-through.

The research design therefore treats uncertainty as a first-class output. A model that produces one precise number without showing those sensitivities is less useful than a simpler model that makes the error budget visible.

## Backtest discipline

A valid event-study version should use:

- event timestamps known at the time;
- point-in-time consensus and revision data;
- contemporaneous prices and factor returns;
- event-level rather than supplier-row independence;
- predefined horizons;
- transaction costs where a trading interpretation is made;
- explicit controls for overlapping events and multiple testing.

The current public prototype focuses on the structural engine and interface. It does not pretend missing historical feeds are already solved.
