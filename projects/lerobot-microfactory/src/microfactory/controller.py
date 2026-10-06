from __future__ import annotations

from dataclasses import dataclass
from .policy import Policy
from .safety import SafetyFilter
from .sim import TabletopSim
from .types import EpisodeResult
from .verifier import StateVerifier

@dataclass
class ControllerConfig:
    grasp_patience_steps: int = 8
    max_recoveries: int = 2
    record_trace: bool = False

class RobotCellController:
    def __init__(self, env: TabletopSim, policy: Policy, safety: SafetyFilter,
                 verifier: StateVerifier, config: ControllerConfig | None = None):
        self.env = env
        self.policy = policy
        self.safety = safety
        self.verifier = verifier
        self.config = config or ControllerConfig()

    def run_episode(self, reset_kwargs: dict | None = None) -> EpisodeResult:
        obs = self.env.reset(**(reset_kwargs or {}))
        self.policy.reset()
        interventions = 0
        safety_projections = 0
        grasp_wait = 0
        trace: list[dict] = []

        while True:
            raw_action = self.policy.act(obs)
            decision = self.safety.project(obs, raw_action)
            safety_projections += int(decision.projected)
            next_obs, done, info = self.env.step(decision.action)

            if self.config.record_trace:
                trace.append({
                    "step": next_obs.step,
                    "gripper_xy": next_obs.gripper_xy.tolist(),
                    "object_xy": next_obs.object_xy.tolist(),
                    "goal_xy": next_obs.goal_xy.tolist(),
                    "gripper_closed": next_obs.gripper_closed,
                    "object_grasped": next_obs.object_grasped,
                    "policy_phase": getattr(self.policy, "phase", "unknown"),
                    "safety_reasons": list(decision.reasons),
                })

            phase = getattr(self.policy, "phase", None)
            grasp_wait = grasp_wait + 1 if phase == "grasp" and not self.verifier.grasp_confirmed(next_obs) else 0

            if grasp_wait >= self.config.grasp_patience_steps:
                if interventions >= self.config.max_recoveries:
                    return EpisodeResult(False, next_obs.step, interventions, safety_projections,
                                         "grasp_failed_after_recovery", tuple(trace))
                interventions += 1
                if hasattr(self.policy, "phase"):
                    self.policy.phase = "approach"
                grasp_wait = 0

            obs = next_obs
            if done:
                success = bool(info["success"] or self.verifier.placement_confirmed(obs))
                return EpisodeResult(success, obs.step, interventions, safety_projections,
                                     None if success else "timeout", tuple(trace))
