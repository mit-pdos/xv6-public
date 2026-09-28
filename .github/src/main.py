from __future__ import annotations

import json
import os

from src.extractors import EXTRACTORS
from src.github import GitHub
from src.laya import Laya
from src.publisher import Publisher
from src.runner import run


def main() -> None:
    event = json.load(open(os.environ["GITHUB_EVENT_PATH"]))
    if "pull_request" not in event:
        raise SystemExit("Layaction only runs on pull_request or pull_request_target events.")
    pr_number = event["pull_request"]["number"]

    gh = GitHub(os.environ["GITHUB_TOKEN"], os.environ["GITHUB_REPOSITORY"])
    laya = Laya()

    results = run(gh, pr_number, EXTRACTORS, send=laya.extract)
    outputs = [e.output(gh, pr_number, results[e.name]) for e in EXTRACTORS]
    Publisher(gh, pr_number).publish(outputs)


if __name__ == "__main__":
    main()
