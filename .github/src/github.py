from __future__ import annotations

import json
import re
import urllib.parse
import urllib.request
from functools import lru_cache
from typing import Any

API_URL = "https://api.github.com"
NEXT_LINK = re.compile(r'<([^>]+)>;\s*rel="next"')


class GitHub:
    """Minimal GitHub REST client scoped to one repository.

    Reads are cached, so extractors asking for the same endpoint
    only hit the API once. Writes are never cached.
    """

    def __init__(self, token: str, repo: str):
        self.token = token
        self.repo = repo

    @lru_cache(maxsize=None)
    def get(self, path: str) -> Any:
        body, _ = self._request(f"{API_URL}/repos/{self.repo}{path}")
        return body

    @lru_cache(maxsize=None)
    def get_all(self, path: str) -> list:
        """GET a list endpoint, following pagination."""
        sep = "&" if "?" in path else "?"
        url = f"{API_URL}/repos/{self.repo}{path}{sep}per_page=100"
        items = []
        while url:
            body, link = self._request(url)
            items.extend(body)
            match = NEXT_LINK.search(link or "")
            url = match.group(1) if match else None
        return items

    # PR comments and labels live on the PR's issue in the REST API.

    def add_comment(self, pr_number: int, body: str) -> dict:
        return self._write("POST", f"/issues/{pr_number}/comments", {"body": body})

    def edit_comment(self, comment_id: int, body: str) -> dict:
        return self._write("PATCH", f"/issues/comments/{comment_id}", {"body": body})

    def add_labels(self, pr_number: int, labels: list[str]) -> list:
        """Missing labels are created by GitHub on the fly."""
        return self._write("POST", f"/issues/{pr_number}/labels", {"labels": labels})

    def remove_label(self, pr_number: int, label: str) -> list:
        return self._write("DELETE", f"/issues/{pr_number}/labels/{urllib.parse.quote(label, safe='')}")

    def _write(self, method: str, path: str, body: dict | None = None) -> Any:
        result, _ = self._request(f"{API_URL}/repos/{self.repo}{path}", method, body)
        return result

    def _request(self, url: str, method: str = "GET", body: dict | None = None) -> tuple[Any, str | None]:
        req = urllib.request.Request(url, method=method, headers={
            "Authorization": f"Bearer {self.token}",
            "Accept": "application/vnd.github+json",
            "X-GitHub-Api-Version": "2022-11-28",
        }, data=json.dumps(body).encode() if body is not None else None)
        with urllib.request.urlopen(req) as resp:
            raw = resp.read()
            return (json.loads(raw) if raw else None), resp.headers.get("Link")
