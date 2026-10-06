from __future__ import annotations

from dataclasses import dataclass
from typing import Any

@dataclass(frozen=True)
class LeRobotAvailability:
    installed: bool
    version: str | None

def availability() -> LeRobotAvailability:
    try:
        import lerobot  # type: ignore
        return LeRobotAvailability(True, getattr(lerobot, "__version__", None))
    except ImportError:
        return LeRobotAvailability(False, None)

def make_robot(robot_config: Any) -> Any:
    try:
        from lerobot.robots.utils import make_robot_from_config  # type: ignore
    except ImportError as exc:
        raise RuntimeError("LeRobot is not installed. Install this project with the 'lerobot' extra.") from exc
    return make_robot_from_config(robot_config)
