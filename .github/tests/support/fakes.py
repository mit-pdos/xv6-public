"""Test doubles: a GitHub with one canned PR, a Laya that answers without a model, a git repo builder."""
from __future__ import annotations

import os
import subprocess
import tempfile

PR_NUMBER = 7

AUTH_PATCH = "@@ -1,2 +1,3 @@\n import os\n-TOKEN = 'old'\n+TOKEN = os.environ['TOKEN']\n+TIMEOUT = 5"
TEST_PATCH = "@@ -0,0 +1,2 @@\n+def test_login():\n+    assert True"
FILES = [
    {"filename": "src/auth.py", "status": "modified", "additions": 2, "deletions": 1, "patch": AUTH_PATCH},
    {"filename": "tests/test_auth.py", "status": "added", "additions": 2, "deletions": 0, "patch": TEST_PATCH},
    {"filename": "logo.png", "status": "added", "additions": 0, "deletions": 0},  # binary: no patch
]
COMMITS = [
    {"sha": "a" * 40, "commit": {"message": "feat(auth): read token from env\n\nNo more hardcoded token."}},
    {"sha": "b" * 40, "commit": {"message": "test: cover login"}},
]


class FakeGitHub:
    """Answers the endpoints the extractors use with one small PR.

    Records every path read and every write; comments and labels start as given.
    """

    def __init__(self, comments: list[dict] | None = None, labels: list[str] | None = None):
        self.paths: list[str] = []
        self.writes: list[tuple] = []
        self.comments = comments or []
        self.labels = labels or []

    def get(self, path: str):
        self.paths.append(path)
        if path == f"/issues/{PR_NUMBER}":
            return {"labels": [{"name": n} for n in self.labels]}
        if path == f"/pulls/{PR_NUMBER}":
            return {"title": "Read token from env", "body": "Moves the token to an env var.",
                    "head": {"ref": "feat/env-token"}, "base": {"ref": "main"}, "additions": 4, "deletions": 1}
        sha = path.rsplit("/", 1)[-1]
        commit = next(c for c in COMMITS if c["sha"] == sha)
        return {"commit": commit["commit"], "stats": {"additions": 2, "deletions": 1}, "files": FILES[:1]}

    def get_all(self, path: str):
        self.paths.append(path)
        return {f"/pulls/{PR_NUMBER}/files": FILES, f"/pulls/{PR_NUMBER}/commits": COMMITS,
                f"/issues/{PR_NUMBER}/comments": self.comments}[path]

    def add_comment(self, pr_number: int, body: str):
        self.writes.append(("add_comment", pr_number, body))

    def edit_comment(self, comment_id: int, body: str):
        self.writes.append(("edit_comment", comment_id, body))

    def add_labels(self, pr_number: int, labels: list[str]):
        self.writes.append(("add_labels", pr_number, labels))

    def remove_label(self, pr_number: int, label: str):
        self.writes.append(("remove_label", pr_number, label))


class FakeLaya:
    """Stands in for Laya.extract: fixed answers in Laya's output shape; records every call."""

    def __init__(self, noul: float = 0.8, score: float = 6.0):
        self.noul, self.score = noul, score
        self.calls: list[tuple[dict, dict]] = []

    def extract(self, schema: dict, data: dict) -> dict:
        self.calls.append((schema, data))
        return {"answers": {qid: self.answer(q) for qid, q in schema.items()},
                "usage": {"input_tokens": 100, "output_tokens": 0}}

    def answer(self, q: dict) -> dict:
        if q["type"] == "noul":
            return {"type": "noul", "noul": self.noul, "confidence": max(self.noul, 1 - self.noul)}
        if q["type"] == "score":
            return {"type": "score", "score": self.score, "confidence": 0.5}
        options = list(q["criteria"])
        probs = {k: (0.7 if i == 0 else 0.3 / (len(options) - 1)) for i, k in enumerate(options)}
        return {"type": "choice", "choice": options[0], "probabilities": probs, "confidence": 0.7}


class GitRepo:
    """A throwaway git repository on disk, for tests that need real git history."""

    def __init__(self):
        self._tmp = tempfile.TemporaryDirectory()
        self.path = self._tmp.name
        self.git("init", "-q", "-b", "main")

    def cleanup(self):
        self._tmp.cleanup()

    def git(self, *args: str) -> str:
        env = {**os.environ, "GIT_AUTHOR_NAME": "Test", "GIT_AUTHOR_EMAIL": "test@example.com",
               "GIT_COMMITTER_NAME": "Test", "GIT_COMMITTER_EMAIL": "test@example.com",
               "GIT_CONFIG_GLOBAL": os.devnull}
        return subprocess.run(["git", "-C", self.path, *args], capture_output=True, text=True,
                              check=True, env=env).stdout.strip()

    def write(self, name: str, content: str | bytes):
        full = os.path.join(self.path, name)
        os.makedirs(os.path.dirname(full), exist_ok=True)
        with open(full, "wb" if isinstance(content, bytes) else "w") as f:
            f.write(content)

    def commit(self, message: str):
        self.git("add", "-A")
        self.git("commit", "-q", "-m", message)
