from __future__ import annotations

from dataclasses import dataclass
import numpy as np
from .types import Observation

@dataclass
class VerifierConfig:
    grasp_confirmation_radius: float = 0.075
    goal_radius: float = 0.075

class StateVerifier:
    def __init__(self, config: VerifierConfig | None = None):
        self.config = config or VerifierConfig()

    def grasp_confirmed(self, obs: Observation) -> bool:
        return bool(obs.object_grasped and obs.gripper_closed and
                    np.linalg.norm(obs.gripper_xy - obs.object_xy) <= self.config.grasp_confirmation_radius)

    def placement_confirmed(self, obs: Observation) -> bool:
        return bool((not obs.object_grasped) and (not obs.gripper_closed) and
                    np.linalg.norm(obs.object_xy - obs.goal_xy) <= self.config.goal_radius)
