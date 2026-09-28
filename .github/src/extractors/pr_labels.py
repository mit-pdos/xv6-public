from __future__ import annotations

from src.extractors.base import Extractor, Request, trim_files, trim_patches
from src.github import GitHub
from src.output import Output


class PrLabelsExtractor(Extractor):
    """PR labels: description, files and commits to classify; diff to spot blockers."""

    name = "pr_labels"
    title = "Labels"

    def extract(self, gh: GitHub, pr_number: int) -> list[Request]:
        pr = gh.get(f"/pulls/{pr_number}")
        files = gh.get_all(f"/pulls/{pr_number}/files")
        commits = gh.get_all(f"/pulls/{pr_number}/commits")
        data = {
            "title": pr["title"],
            "description": pr.get("body") or "",
            "head": pr["head"]["ref"],
            "base": pr["base"]["ref"],
            "additions": pr["additions"],
            "deletions": pr["deletions"],
            "commits": [c["commit"]["message"].splitlines()[0] for c in commits],
            "files": trim_files(files),
            "diff": trim_patches(files),
        }
        return [Request(self.schema(), data, subject="pr")]

    def output(self, gh: GitHub, pr_number: int, results: dict) -> Output:
        """The default section, plus each answer as a "question:choice" label."""
        out = super().output(gh, pr_number, results)
        answers = results["pr"]["answers"]
        out.labels = [f"{qid}:{answers[qid]['choice']}" for qid in self.schema()]
        return out
