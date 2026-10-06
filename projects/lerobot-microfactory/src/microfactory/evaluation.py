from __future__ import annotations

from dataclasses import asdict, dataclass
import numpy as np
from .controller import RobotCellController
from .perturbations import EpisodeScenario

@dataclass(frozen=True)
class EvaluationSummary:
    episodes: int
    successes: int
    success_rate: float
    mean_steps: float
    p95_steps: float
    total_interventions: int
    intervention_rate: float
    safety_projections: int
    def as_dict(self) -> dict:
        return asdict(self)

def evaluate(controller: RobotCellController, scenarios: list[EpisodeScenario]) -> EvaluationSummary:
    results = [controller.run_episode({
        "object_xy": s.object_xy, "goal_xy": s.goal_xy, "gripper_xy": s.gripper_xy
    }) for s in scenarios]
    episodes = len(results)
    successes = sum(int(r.success) for r in results)
    steps = np.array([r.steps for r in results], dtype=float)
    interventions = sum(r.interventions for r in results)
    projections = sum(r.safety_projections for r in results)
    return EvaluationSummary(
        episodes, successes, successes / episodes if episodes else 0.0,
        float(np.mean(steps)) if episodes else 0.0,
        float(np.percentile(steps, 95)) if episodes else 0.0,
        interventions, interventions / episodes if episodes else 0.0, projections
    )
