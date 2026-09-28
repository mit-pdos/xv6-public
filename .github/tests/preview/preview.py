"""Score the current branch as if it were opened as a PR, without opening one.

    python -m tests.preview.preview [--repo PATH] [--base BRANCH]
"""
from __future__ import annotations

import argparse
import sys
import warnings
from statistics import mean

from src.extractors import EXTRACTORS
from tests.preview.local_repo import PR_NUMBER, LocalRepo
from src.runner import run


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--repo", default=".", help="repository to preview (default: current directory)")
    parser.add_argument("--base", help="branch the PR would target (default: origin's default, main or master)")
    args = parser.parse_args()

    repo = LocalRepo(args.repo, args.base)
    pr = repo.get(f"/pulls/{PR_NUMBER}")
    commits = repo.get_all(f"/pulls/{PR_NUMBER}/commits")
    files = repo.get_all(f"/pulls/{PR_NUMBER}/files")
    if not commits:
        raise SystemExit(f"{repo.head} has no commits on top of {repo.base}; nothing to preview.")

    print(f"Loading Laya and scoring {len(commits)} commit(s)...", file=sys.stderr)
    warnings.filterwarnings("ignore")
    from src.laya import Laya  # imported late: loading torch takes a few seconds

    results = run(repo, PR_NUMBER, EXTRACTORS, send=Laya().extract)
    schemas = {e.name: e.schema() for e in EXTRACTORS}
    print(report(pr, commits, files, results, schemas))


def report(pr: dict, commits: list[dict], files: list[dict], results: dict, schemas: dict) -> str:
    """Terminal summary of the extractors' results, ending with an overall 0-10 score."""
    out = [
        f"PR preview: {pr['head']['ref']} -> {pr['base']['ref']}",
        f"  \"{pr['title']}\"  ({len(commits)} commits, {len(files)} files, "
        f"+{pr['additions']} -{pr['deletions']})",
    ]
    normalised = []  # every noul/score answer on a 0-1 scale, higher = better

    labels = results["pr_labels"]["pr"]["answers"]
    out += ["", "Labels"]
    for qid, a in labels.items():
        out.append(f"  {qid:<10} {a['choice']:<14} {a['probabilities'][a['choice']]:.0%} sure")

    checks = results["checkbox"]["pr"]
    out += ["", f"Code checks (added lines read in {checks.get('chunks', 1)} chunk(s))"]
    for qid, a in checks["answers"].items():
        mark = "✔" if a["noul"] >= 0.5 else "✘"
        out.append(f"  {mark} {a['noul']:>4.0%}  {schemas['checkbox'][qid]['instructions']}")
        normalised.append(a["noul"])

    out += ["", "Commits (1-10)"]
    commit_schema = schemas["commit"]
    out.append("  " + " " * 9 + "".join(f"{qid.removeprefix('message_'):>10}" for qid in commit_schema))
    for c in commits:
        answers = results["commit"][c["sha"]]["answers"]
        cells = []
        for qid, spec in commit_schema.items():
            levels = len(spec["criteria"]) - 1
            normalised.append(answers[qid]["score"] / levels)
            cells.append(f"{answers[qid]['score'] + 1:>10.1f}")  # Laya counts levels from 0
        subject = c["commit"]["message"].splitlines()[0]
        out.append(f"  {c['sha'][:7]}  " + "".join(cells) + f"   {subject[:50]}")

    out += ["", f"Overall score: {10 * mean(normalised):.1f} / 10  (mean of code checks and commit scores)"]
    return "\n".join(out)


if __name__ == "__main__":
    main()
