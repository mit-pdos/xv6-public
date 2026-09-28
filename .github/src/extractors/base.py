from __future__ import annotations

import json
from abc import ABC, abstractmethod
from dataclasses import dataclass
from pathlib import Path

from src.github import GitHub
from src.output import Output, answers_table

SCHEMAS_DIR = Path(__file__).resolve().parents[2] / "schemas"
# Laya reads at most 512 tokens per question (up to 192 of them for the question itself)
# and silently cuts off the rest, so ~320 tokens are left for the data. Code runs at ~3+
# chars per token, so a request's data is kept under this many chars of JSON.
CHUNK_CHARS = 1_000
# Single-request extractors only get the start of the diff; keep it last so the
# smaller fields before it survive the cut.
MAX_DIFF_CHARS = 1_500


@dataclass
class Request:
    """One call to the model: a schema plus the GitHub data it should be applied to."""
    schema: dict
    data: dict
    subject: str  # what the result refers to, e.g. "pr" or a commit sha


class Extractor(ABC):
    """Mirror of one schema file: fetches the PR data that schema needs.

    A new extractor only has to set `name` and implement `extract`; its section in the
    PR comment is built from the schema. Override `output` for labels or a custom section,
    or `describe` to show subjects (e.g. commits) as something nicer than their id.
    """

    name: str  # matches schemas/<name>.json
    title: str = ""  # section heading in the PR comment; defaults to the name

    def schema(self) -> dict:
        return json.loads((SCHEMAS_DIR / f"{self.name}.json").read_text())

    @abstractmethod
    def extract(self, gh: GitHub, pr_number: int) -> list[Request]:
        ...

    def output(self, gh: GitHub, pr_number: int, results: dict) -> Output:
        """Turn this extractor's results ({subject: Laya result}) into what gets published."""
        subjects = {subject: self.describe(gh, subject) for subject in results}
        heading = self.title or self.name.replace("_", " ").capitalize()
        table = answers_table(self.schema(), results, subjects)
        return Output(comment=f"### {heading}\n\n{table}")

    def describe(self, gh: GitHub, subject: str) -> str:
        """How a subject is shown in the comment."""
        return subject


def trim_files(files: list[dict]) -> list[dict]:
    """Keep only the useful fields of GitHub file entries (no patches)."""
    return [{k: f[k] for k in ("filename", "status", "additions", "deletions")} for f in files]


def trim_patches(files: list[dict], limit: int = MAX_DIFF_CHARS) -> dict[str, str]:
    """Map filename -> patch, stopping once the total size reaches limit."""
    patches, budget = {}, limit
    for f in files:
        if not f.get("patch") or budget <= 0:
            continue
        patches[f["filename"]] = f["patch"][:budget]
        budget -= len(f["patch"])
    return patches


def chunk_added_lines(files: list[dict], header: dict, limit: int = CHUNK_CHARS) -> list[dict]:
    """Split the added lines of files into request data that each fit one Laya call.

    Every chunk is header + {"added": {filename: [lines]}}. Removed and context lines are
    dropped: a removed secret must not read as a secret in the code.
    """
    budget = limit - len(json.dumps(header)) - len('"added": {}')
    chunks: list[dict] = []
    current: dict[str, list[str]] = {}
    size = 0
    for f in files:
        name = f["filename"]
        for line in (f.get("patch") or "").splitlines():
            if not line.startswith("+"):
                continue
            line = line[1:][: budget // 2]
            cost = len(json.dumps(line)) + 2
            if name not in current:
                cost += len(json.dumps(name)) + 4
            if current and size + cost > budget:
                chunks.append(current)
                current, size = {}, 0
                cost = len(json.dumps(line)) + len(json.dumps(name)) + 6
            current.setdefault(name, []).append(line)
            size += cost
    if current:
        chunks.append(current)
    return [{**header, "added": c} for c in chunks] or [header]
