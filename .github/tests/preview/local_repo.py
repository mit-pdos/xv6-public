from __future__ import annotations

import re
import subprocess
from typing import Any

PR_NUMBER = 0  # there is no real PR; extractors just need a number to put in paths


class LocalRepo:
    """Stand-in for GitHub that answers the extractors' requests from a local branch.

    It pretends the current branch was opened as a PR against base, the way GitHub
    would show it: commits and files since the merge-base, committed changes only.
    """

    def __init__(self, path: str = ".", base: str | None = None):
        self.path = path
        self.head = self._git("rev-parse", "--abbrev-ref", "HEAD")
        self.base = base or self._default_base()
        self.merge_base = self._git("merge-base", self.base, "HEAD")

    def get(self, path: str) -> Any:
        if path == f"/pulls/{PR_NUMBER}":
            return self._pull()
        if m := re.fullmatch(r"/commits/(\w+)", path):
            return self._commit(m.group(1))
        raise ValueError(f"LocalRepo cannot answer {path}")

    def get_all(self, path: str) -> list:
        if path == f"/pulls/{PR_NUMBER}/files":
            return parse_diff(self._git("diff", self.merge_base, "HEAD"))
        if path == f"/pulls/{PR_NUMBER}/commits":
            shas = self._git("rev-list", "--reverse", f"{self.merge_base}..HEAD").split()
            return [{"sha": sha, "commit": {"message": self._message(sha)}} for sha in shas]
        raise ValueError(f"LocalRepo cannot answer {path}")

    def _pull(self) -> dict:
        commits = self.get_all(f"/pulls/{PR_NUMBER}/commits")
        files = self.get_all(f"/pulls/{PR_NUMBER}/files")
        # GitHub's default title: the commit subject for one commit, else the branch name.
        if len(commits) == 1:
            title = commits[0]["commit"]["message"].splitlines()[0]
        else:
            title = re.sub(r"[-_/]+", " ", self.head).strip().capitalize()
        return {
            "title": title,
            "body": "",
            "head": {"ref": self.head},
            "base": {"ref": self.base},
            "additions": sum(f["additions"] for f in files),
            "deletions": sum(f["deletions"] for f in files),
        }

    def _commit(self, sha: str) -> dict:
        files = parse_diff(self._git("show", "--format=", sha))
        return {
            "commit": {"message": self._message(sha)},
            "stats": {
                "additions": sum(f["additions"] for f in files),
                "deletions": sum(f["deletions"] for f in files),
            },
            "files": files,
        }

    def _message(self, sha: str) -> str:
        return self._git("log", "-1", "--format=%B", sha)

    def _default_base(self) -> str:
        """origin's default branch if known, else main or master."""
        try:
            return self._git("symbolic-ref", "--short", "refs/remotes/origin/HEAD")
        except subprocess.CalledProcessError:
            pass
        for name in ("main", "master"):
            try:
                self._git("rev-parse", "--verify", "--quiet", name)
                return name
            except subprocess.CalledProcessError:
                continue
        raise SystemExit("Could not find a base branch; pass --base.")

    def _git(self, *args: str) -> str:
        return subprocess.run(
            ["git", "-C", self.path, *args], capture_output=True, text=True, check=True
        ).stdout.strip()


def parse_diff(diff: str) -> list[dict]:
    """Turn `git diff` output into GitHub-style file entries (filename, status, counts, patch)."""
    files = []
    for block in re.split(r"^diff --git ", diff, flags=re.M)[1:]:
        lines = block.splitlines()
        header = lines[: next((i for i, l in enumerate(lines) if l.startswith("@@")), len(lines))]
        name = next((l[6:] for l in header if l.startswith("+++ b/")), None) \
            or next((l[6:] for l in header if l.startswith("--- a/")), None) \
            or next((l[10:] for l in header if l.startswith("rename to ")), None) \
            or lines[0].split(" b/", 1)[-1]
        if any(l.startswith("new file mode") for l in header):
            status = "added"
        elif any(l.startswith("deleted file mode") for l in header):
            status = "removed"
        elif any(l.startswith("rename from") for l in header):
            status = "renamed"
        else:
            status = "modified"
        patch_lines = lines[len(header):]
        files.append({
            "filename": name,
            "status": status,
            "additions": sum(1 for l in patch_lines if l.startswith("+")),
            "deletions": sum(1 for l in patch_lines if l.startswith("-")),
            "patch": "\n".join(patch_lines) or None,  # None for binary files, like GitHub
        })
    return files
