# Architecture v0.1

```text
Observation
    |
    v
Policy -> SafetyFilter -> Robot / Sim -> Verifier
              ^                           |
              +--------- recover ---------+
```

Learned policies do not own safety, episode orchestration, verification, or metrics. Those components remain deterministic and testable.
