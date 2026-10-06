from __future__ import annotations

from dataclasses import dataclass
import numpy as np

@dataclass(frozen=True)
class EpisodeScenario:
    name: str
    object_xy: np.ndarray
    goal_xy: np.ndarray
    gripper_xy: np.ndarray

class ScenarioGenerator:
    def __init__(self, seed: int = 7):
        self.rng = np.random.default_rng(seed)

    def nominal(self) -> EpisodeScenario:
        return EpisodeScenario("nominal", np.array([0.34, 0.55]), np.array([0.76, 0.68]), np.array([0.50, 0.12]))

    def random_pose(self, index: int) -> EpisodeScenario:
        return EpisodeScenario(
            f"random_pose_{index:03d}",
            self.rng.uniform([0.16, 0.28], [0.54, 0.80]),
            self.rng.uniform([0.62, 0.34], [0.90, 0.86]),
            self.rng.uniform([0.32, 0.06], [0.68, 0.18]),
        )
