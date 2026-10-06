import numpy as np
from microfactory.sim import SimConfig, TabletopSim
from microfactory.types import Action

def test_grasp_and_release_in_goal():
    env = TabletopSim(SimConfig(max_motion_per_step=1.0, grasp_radius=0.1, goal_radius=0.1, seed=1))
    env.reset(object_xy=np.array([0.3,0.3]), goal_xy=np.array([0.7,0.7]), gripper_xy=np.array([0.3,0.3]))
    obs,_,_ = env.step(Action(np.zeros(2),1.0)); assert obs.object_grasped
    obs,_,_ = env.step(Action(np.array([0.4,0.4]),1.0)); assert obs.object_grasped
    _,done,info = env.step(Action(np.zeros(2),-1.0)); assert done and info["success"]
