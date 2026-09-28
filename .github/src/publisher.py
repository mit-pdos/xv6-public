from __future__ import annotations

from src.github import GitHub
from src.output import Output

# Hidden in the comment so later runs find and edit it instead of posting another one.
MARKER = "<!-- layaction -->"
HEADING = "## Layaction review"


class Publisher:
    """Publishes every extractor's output to a PR as one comment plus labels.

    It knows nothing about specific extractors: new ones show up as new sections.
    """

    def __init__(self, gh: GitHub, pr_number: int):
        self.gh = gh
        self.pr_number = pr_number

    def publish(self, outputs: list[Output]) -> None:
        sections = [o.comment for o in outputs if o.comment]
        if sections:
            self.publish_comment(merge_sections(sections))
        labels = [label for o in outputs for label in o.labels]
        if labels:
            self.publish_labels(labels)

    def publish_comment(self, body: str) -> None:
        """Edit our comment from a previous run if there is one, else post a new one."""
        comments = self.gh.get_all(f"/issues/{self.pr_number}/comments")
        ours = next((c for c in comments if MARKER in (c.get("body") or "")), None)
        if ours:
            self.gh.edit_comment(ours["id"], body)
        else:
            self.gh.add_comment(self.pr_number, body)

    def publish_labels(self, labels: list[str]) -> None:
        """Add labels; a "key:value" label replaces any other "key:" label from a previous run.

        Labels without a key we set (e.g. added by people) are left alone.
        """
        current = [label["name"] for label in self.gh.get(f"/issues/{self.pr_number}")["labels"]]
        keys = {label.split(":", 1)[0] + ":" for label in labels if ":" in label}
        for label in current:
            if label.startswith(tuple(keys)) and label not in labels:
                self.gh.remove_label(self.pr_number, label)
        new = [label for label in labels if label not in current]
        if new:
            self.gh.add_labels(self.pr_number, new)


def merge_sections(sections: list[str]) -> str:
    return "\n\n".join([MARKER, HEADING, *sections])
