from __future__ import annotations

from dataclasses import dataclass
import numpy as np
from .types import Action, Observation

@dataclass
class SimConfig:
    max_steps: int = 180
    dt: float = 1.0
    max_motion_per_step: float = 0.055
    grasp_radius: float = 0.055
    goal_radius: float = 0.065
    workspace_min: float = 0.0
    workspace_max: float = 1.0
    control_noise_std: float = 0.0
    seed: int | None = None

class TabletopSim:
    def __init__(self, config: SimConfig | None = None):
        self.config = config or SimConfig()
        self.rng = np.random.default_rng(self.config.seed)
        self._done = False

    def reset(self, *, object_xy=None, goal_xy=None, gripper_xy=None) -> Observation:
        self._step = 0
        self._done = False
        self.gripper_closed = False
        self.object_grasped = False
        self.gripper_xy = np.array(gripper_xy if gripper_xy is not None else [0.5, 0.12], dtype=float)
        self.object_xy = np.array(object_xy if object_xy is not None else self.rng.uniform([0.18,0.30],[0.52,0.78]), dtype=float)
        self.goal_xy = np.array(goal_xy if goal_xy is not None else self.rng.uniform([0.64,0.40],[0.88,0.84]), dtype=float)
        return self.observe()

    def observe(self) -> Observation:
        return Observation(self.gripper_xy.copy(), self.object_xy.copy(), self.goal_xy.copy(),
                           self.gripper_closed, self.object_grasped, self._step)

    def step(self, action: Action):
        if self._done:
            raise RuntimeError("Episode is done. Call reset() before stepping again.")
        delta = np.asarray(action.delta_xy, dtype=float).reshape(2)
        norm = float(np.linalg.norm(delta))
        if norm > self.config.max_motion_per_step:
            delta = delta / norm * self.config.max_motion_per_step
        if self.config.control_noise_std > 0:
            delta = delta + self.rng.normal(0.0, self.config.control_noise_std, size=2)
        self.gripper_xy = np.clip(self.gripper_xy + delta * self.config.dt,
                                  self.config.workspace_min, self.config.workspace_max)
        was_closed = self.gripper_closed
        if action.grip > 0.25: self.gripper_closed = True
        elif action.grip < -0.25: self.gripper_closed = False
        if (not was_closed or action.grip > 0.25) and self.gripper_closed:
            if float(np.linalg.norm(self.gripper_xy - self.object_xy)) <= self.config.grasp_radius:
                self.object_grasped = True
        if self.object_grasped and self.gripper_closed:
            self.object_xy = self.gripper_xy.copy()
        if self.object_grasped and not self.gripper_closed:
            self.object_grasped = False
        self._step += 1
        success = self.is_success()
        timeout = self._step >= self.config.max_steps
        self._done = success or timeout
        return self.observe(), self._done, {"success": success, "timeout": timeout}

    def is_success(self) -> bool:
        in_goal = float(np.linalg.norm(self.object_xy - self.goal_xy)) <= self.config.goal_radius
        return bool(in_goal and not self.object_grasped and not self.gripper_closed)
