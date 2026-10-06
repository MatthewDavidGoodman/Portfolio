from __future__ import annotations

import argparse, json
from .controller import ControllerConfig, RobotCellController
from .evaluation import evaluate
from .lerobot_bridge import availability
from .perturbations import ScenarioGenerator
from .policy import HeuristicPickPlacePolicy
from .safety import SafetyFilter
from .sim import SimConfig, TabletopSim
from .verifier import StateVerifier

def build_controller(seed: int, noise: float, trace: bool = False) -> RobotCellController:
    return RobotCellController(
        TabletopSim(SimConfig(seed=seed, control_noise_std=noise)),
        HeuristicPickPlacePolicy(), SafetyFilter(), StateVerifier(),
        ControllerConfig(record_trace=trace)
    )

def cmd_demo(args):
    result = build_controller(args.seed, args.noise, True).run_episode()
    print(json.dumps({"success":result.success,"steps":result.steps,
                      "interventions":result.interventions,
                      "safety_projections":result.safety_projections}, indent=2))
    return 0 if result.success else 2

def cmd_eval(args):
    controller = build_controller(args.seed, args.noise)
    gen = ScenarioGenerator(args.seed)
    scenarios = [gen.nominal()] + [gen.random_pose(i) for i in range(args.episodes - 1)]
    summary = evaluate(controller, scenarios)
    print(json.dumps(summary.as_dict(), indent=2))
    return 0 if summary.success_rate >= args.min_success_rate else 3

def cmd_status(_):
    status = availability()
    print(json.dumps({"lerobot_installed":status.installed,"version":status.version}, indent=2))
    return 0

def build_parser():
    p = argparse.ArgumentParser(prog="microfactory")
    sub = p.add_subparsers(dest="command", required=True)
    d = sub.add_parser("demo"); d.add_argument("--seed",type=int,default=7); d.add_argument("--noise",type=float,default=0.0); d.set_defaults(func=cmd_demo)
    e = sub.add_parser("eval"); e.add_argument("--episodes",type=int,default=25); e.add_argument("--seed",type=int,default=7); e.add_argument("--noise",type=float,default=0.0); e.add_argument("--min-success-rate",type=float,default=0.95); e.set_defaults(func=cmd_eval)
    s = sub.add_parser("status"); s.set_defaults(func=cmd_status)
    return p

def main():
    args = build_parser().parse_args()
    return args.func(args)

if __name__ == "__main__":
    raise SystemExit(main())
