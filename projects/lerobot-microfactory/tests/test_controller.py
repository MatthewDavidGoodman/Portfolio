from microfactory.controller import RobotCellController
from microfactory.policy import HeuristicPickPlacePolicy
from microfactory.safety import SafetyFilter
from microfactory.sim import SimConfig, TabletopSim
from microfactory.verifier import StateVerifier

def test_baseline_solves_random_episodes():
    env = TabletopSim(SimConfig(seed=11))
    controller = RobotCellController(env, HeuristicPickPlacePolicy(), SafetyFilter(), StateVerifier())
    for _ in range(10):
        assert controller.run_episode().success
