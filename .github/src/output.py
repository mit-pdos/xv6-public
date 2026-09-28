"""What extractors hand to the publisher, and the markdown used to show Laya's answers."""
from __future__ import annotations

from dataclasses import dataclass, field


@dataclass
class Output:
    """One extractor's contribution to the PR: a comment section and/or labels."""
    comment: str | None = None  # markdown section, merged with the others into one comment
    labels: list[str] = field(default_factory=list)


def format_answer(question: dict, answer: dict) -> str:
    """Show one Laya answer; depends only on the question type, so it works for any schema."""
    if question["type"] == "noul":
        p = answer["noul"]
        return f"{'[x]' if p >= 0.5 else '[]'} {p:.0%}"
    if question["type"] == "score":
        levels = question["criteria"]
        value = answer["score"]  # Laya counts levels from 0
        shown = f"{value + 1:.1f} / {len(levels)}"
        level = str(levels[round(value)])
        return shown if level.isdigit() else f"{level} ({shown})"
    choice = answer["choice"]
    return f"`{choice}` ({answer['probabilities'][choice]:.0%})"


def answers_table(schema: dict, results: dict, subjects: dict[str, str]) -> str:
    """Markdown table of results ({subject: Laya result}).

    The whole PR ("pr"): a row per question. Anything else (e.g. commits): a row per
    subject and a column per question, even when there is only one.
    subjects maps each subject to how it is shown.
    """
    if list(results) == ["pr"]:
        [result] = results.values()
        rows = [f"| {cell(q['instructions'])} | {format_answer(q, result['answers'][qid])} |"
                for qid, q in schema.items()]
        return "\n".join(["| Question | Answer |", "|---|---|", *rows])

    header = "| | " + " | ".join(cell(qid) for qid in schema) + " |"
    divider = "|---|" + "---|" * len(schema)
    rows = []
    for subject, result in results.items():
        cells = [format_answer(q, result["answers"][qid]) for qid, q in schema.items()]
        rows.append(f"| {cell(subjects[subject])} | " + " | ".join(cells) + " |")
    return "\n".join([header, divider, *rows])


def cell(text: str) -> str:
    """Make text safe inside a markdown table cell."""
    return text.replace("|", "\\|").replace("\n", " ")
