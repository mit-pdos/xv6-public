from __future__ import annotations

from src.extractors.base import Extractor, Request, trim_files
from src.github import GitHub


class CommitExtractor(Extractor):
    """Commit score: one request per commit, metadata only (no diff)."""

    name = "commit"
    title = "Commits"

    def extract(self, gh: GitHub, pr_number: int) -> list[Request]:
        schema = self.schema()
        requests = []
        for c in gh.get_all(f"/pulls/{pr_number}/commits"):
            # The list endpoint has no stats/files; the single-commit one does.
            commit = gh.get(f"/commits/{c['sha']}")
            data = {
                "message": commit["commit"]["message"],
                "additions": commit["stats"]["additions"],
                "deletions": commit["stats"]["deletions"],
                "files": trim_files(commit.get("files", [])),
            }
            requests.append(Request(schema, data, subject=c["sha"]))
        return requests

    def describe(self, gh: GitHub, subject: str) -> str:
        message = gh.get(f"/commits/{subject}")["commit"]["message"]
        return f"`{subject[:7]}` {message.splitlines()[0]}"
