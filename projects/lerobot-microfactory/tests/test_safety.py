import numpy as np
from microfactory.safety import SafetyConfig, SafetyFilter
from microfactory.types import Action, Observation

def make_obs(xy=(0.5,0.5)):
    return Observation(np.array(xy,float), np.array([0.3,0.3]), np.array([0.8,0.8]), False, False, 0)

def test_velocity_projection():
    d = SafetyFilter(SafetyConfig(max_delta_norm=0.05)).project(make_obs(), Action(np.array([1.0,0.0]),0.0))
    assert d.projected and np.isclose(np.linalg.norm(d.action.delta_xy),0.05)

def test_workspace_projection():
    d = SafetyFilter(SafetyConfig(max_delta_norm=1.0, workspace_min=0.02, workspace_max=0.98)).project(
        make_obs((0.97,0.5)), Action(np.array([0.2,0.0]),0.0))
    assert d.projected and np.isclose(d.action.delta_xy[0],0.01)
