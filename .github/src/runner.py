from __future__ import annotations

from concurrent.futures import ThreadPoolExecutor
from typing import Callable

from src.combine import combine
from src.extractors.base import Extractor
from src.github import GitHub

# (schema, data) -> model output. Plug in the real model client here.
Send = Callable[[dict, dict], dict]


def run(gh: GitHub, pr_number: int, extractors: list[Extractor], send: Send, max_workers: int = 8) -> dict:
    """Run every extractor in parallel; each fetches its data and sends its requests.

    Requests sharing a subject (chunks of one diff) are merged into one result.
    Returns {extractor name: {subject: model output}}.
    """
    def run_one(extractor: Extractor) -> dict:
        by_subject: dict[str, list[dict]] = {}
        for r in extractor.extract(gh, pr_number):
            result = send(r.schema, r.data)
            by_subject.setdefault(r.subject, []).append(result)
        schema = extractor.schema()
        combined = {}
        for subject, results in by_subject.items():
            combined[subject] = combine(schema, results)
        return combined

    with ThreadPoolExecutor(max_workers=max_workers) as pool:
        futures = {e.name: pool.submit(run_one, e) for e in extractors}
        return {name: future.result() for name, future in futures.items()}
