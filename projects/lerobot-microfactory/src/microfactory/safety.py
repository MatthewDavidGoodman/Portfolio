from __future__ import annotations

from dataclasses import dataclass
import numpy as np
from .types import Action, Observation

@dataclass
class SafetyConfig:
    max_delta_norm: float = 0.055
    workspace_min: float = 0.02
    workspace_max: float = 0.98

@dataclass(frozen=True)
class SafetyDecision:
    action: Action
    projected: bool
    reasons: tuple[str, ...]

class SafetyFilter:
    def __init__(self, config: SafetyConfig | None = None):
        self.config = config or SafetyConfig()

    def project(self, obs: Observation, action: Action) -> SafetyDecision:
        delta = np.asarray(action.delta_xy, dtype=float).reshape(2).copy()
        reasons: list[str] = []
        norm = float(np.linalg.norm(delta))
        if norm > self.config.max_delta_norm:
            delta *= self.config.max_delta_norm / norm
            reasons.append("velocity_limit")
        proposed = obs.gripper_xy + delta
        clipped = np.clip(proposed, self.config.workspace_min, self.config.workspace_max)
        safe_delta = clipped - obs.gripper_xy
        if not np.allclose(safe_delta, delta):
            delta = safe_delta
            reasons.append("workspace_limit")
        grip = float(np.clip(action.grip, -1.0, 1.0))
        if grip != action.grip:
            reasons.append("gripper_command_limit")
        return SafetyDecision(Action(delta_xy=delta, grip=grip), bool(reasons), tuple(reasons))
