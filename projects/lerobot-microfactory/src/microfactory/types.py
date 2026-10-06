from __future__ import annotations

from dataclasses import dataclass, field
from typing import Any
import numpy as np

@dataclass(frozen=True)
class Observation:
    gripper_xy: np.ndarray
    object_xy: np.ndarray
    goal_xy: np.ndarray
    gripper_closed: bool
    object_grasped: bool
    step: int
    task: str = "place the object in the fixture"
    extras: dict[str, Any] = field(default_factory=dict)

@dataclass(frozen=True)
class Action:
    delta_xy: np.ndarray
    grip: float = 0.0

@dataclass(frozen=True)
class EpisodeResult:
    success: bool
    steps: int
    interventions: int
    safety_projections: int
    failure_reason: str | None = None
    trace: tuple[dict[str, Any], ...] = ()
