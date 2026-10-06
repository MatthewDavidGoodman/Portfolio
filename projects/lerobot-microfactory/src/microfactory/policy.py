from __future__ import annotations

from dataclasses import dataclass
from typing import Protocol
import numpy as np
from .types import Action, Observation

class Policy(Protocol):
    def reset(self) -> None: ...
    def act(self, obs: Observation) -> Action: ...

@dataclass
class HeuristicPolicyConfig:
    approach_radius: float = 0.03
    release_radius: float = 0.035
    motion_gain: float = 0.8
    max_delta: float = 0.05

class HeuristicPickPlacePolicy:
    def __init__(self, config: HeuristicPolicyConfig | None = None):
        self.config = config or HeuristicPolicyConfig()
        self.phase = "approach"

    def reset(self) -> None:
        self.phase = "approach"

    def _move_toward(self, current: np.ndarray, target: np.ndarray) -> np.ndarray:
        delta = (target - current) * self.config.motion_gain
        norm = float(np.linalg.norm(delta))
        if norm > self.config.max_delta:
            delta = delta / norm * self.config.max_delta
        return delta

    def act(self, obs: Observation) -> Action:
        if self.phase == "approach":
            if float(np.linalg.norm(obs.gripper_xy - obs.object_xy)) <= self.config.approach_radius:
                self.phase = "grasp"
                return Action(np.zeros(2), grip=1.0)
            return Action(self._move_toward(obs.gripper_xy, obs.object_xy), grip=-1.0)
        if self.phase == "grasp":
            if obs.object_grasped:
                self.phase = "transport"
            else:
                return Action(np.zeros(2), grip=1.0)
        if self.phase == "transport":
            if float(np.linalg.norm(obs.gripper_xy - obs.goal_xy)) <= self.config.release_radius:
                self.phase = "release"
                return Action(np.zeros(2), grip=-1.0)
            return Action(self._move_toward(obs.gripper_xy, obs.goal_xy), grip=1.0)
        if self.phase == "release":
            return Action(np.zeros(2), grip=-1.0)
        raise RuntimeError(f"Unknown policy phase: {self.phase}")
