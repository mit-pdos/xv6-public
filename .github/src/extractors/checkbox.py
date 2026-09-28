from __future__ import annotations

from src.extractors.base import Extractor, Request, chunk_added_lines
from src.github import GitHub


class CheckboxExtractor(Extractor):
    """Code-quality questions: needs the added lines, split across several requests."""

    name = "checkbox"
    title = "Code checks"

    def extract(self, gh: GitHub, pr_number: int) -> list[Request]:
        pr = gh.get(f"/pulls/{pr_number}")
        files = gh.get_all(f"/pulls/{pr_number}/files")
        schema = self.schema()
        chunks = chunk_added_lines(files, header={"title": pr["title"]})
        return [Request(schema, data, subject="pr") for data in chunks]
