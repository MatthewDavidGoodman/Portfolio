# MicroFactory-LR

A simulation-first personal robotics project built around a small autonomous manufacturing cell:

**observe -> manipulate -> verify -> recover**

The eventual physical task is tabletop component pick/place or insertion using LeRobot-compatible hardware. v0.1 deliberately starts with a tiny deterministic simulator so safety, recovery, evaluation, and policy boundaries can be tested before hardware or imitation learning is introduced.

## What already works

- 2-D pick/place simulator
- closed-loop policy controller
- deterministic action safety projection
- grasp and placement verification hooks
- automatic grasp-recovery path
- randomized evaluation scenarios
- success/intervention/safety metrics
- optional LeRobot compatibility boundary
- ACT and SmolVLA training command templates
- unit tests

## Quick start

```bash
python -m venv .venv
source .venv/bin/activate
pip install -e '.[dev]'

microfactory demo
microfactory eval --episodes 100
pytest
```

Expected baseline: near-100% success in the deterministic v0 simulator. The heuristic policy uses privileged state; it exists to prove the surrounding robot-cell logic is sound before replacing it with learned perception/control.

## Roadmap

- **v0.1:** deterministic simulation, policy protocol, safety, verification, recovery, evaluation
- **v0.2:** synthetic RGB + OpenCV visual state
- **v0.3:** LeRobotDataset + ACT adapter
- **v0.4:** physical arm/cameras/calibration/watchdog
- **v0.5:** SmolVLA robustness study
