import unittest

from src.extractors import EXTRACTORS
from src.extractors.base import Extractor, Request
from src.output import answers_table, format_answer
from src.runner import run
from tests.support.fakes import COMMITS, PR_NUMBER, FakeGitHub, FakeLaya


class FormatAnswerTest(unittest.TestCase):
    def test_noul(self):
        self.assertEqual(format_answer({"type": "noul"}, {"noul": 0.83}), "[x] 83%")
        self.assertEqual(format_answer({"type": "noul"}, {"noul": 0.2}), "[] 20%")

    def test_numeric_score_is_shown_on_the_criteria_scale(self):
        q = {"type": "score", "criteria": [str(i) for i in range(1, 11)]}
        self.assertEqual(format_answer(q, {"score": 6.2}), "7.2 / 10")  # Laya's level 6 is "7"

    def test_worded_score_shows_the_nearest_level(self):
        q = {"type": "score", "criteria": ["not urgent", "soon", "critical"]}
        self.assertEqual(format_answer(q, {"score": 1.2}), "soon (2.2 / 3)")

    def test_choice(self):
        answer = {"choice": "fix", "probabilities": {"fix": 0.81, "feat": 0.19}}
        self.assertEqual(format_answer({"type": "choice"}, answer), "`fix` (81%)")


class AnswersTableTest(unittest.TestCase):
    SCHEMA = {"ok": {"type": "noul", "instructions": "Is it | fine?"}}

    def test_one_subject_is_a_row_per_question(self):
        table = answers_table(self.SCHEMA, {"pr": {"answers": {"ok": {"noul": 0.9}}}}, {"pr": "pr"})
        self.assertEqual(table, "| Question | Answer |\n|---|---|\n| Is it \\| fine? | [x] 90% |")

    def test_a_single_commit_is_still_a_row(self):
        table = answers_table(self.SCHEMA, {"abc": {"answers": {"ok": {"noul": 0.9}}}}, {"abc": "`abc` fix"})
        self.assertEqual(table.splitlines(), ["| | ok |", "|---|---|", "| `abc` fix | [x] 90% |"])

    def test_several_subjects_are_a_row_each(self):
        results = {s: {"answers": {"ok": {"noul": p}}} for s, p in (("a", 0.9), ("b", 0.1))}
        table = answers_table(self.SCHEMA, results, {"a": "first", "b": "second"})
        self.assertEqual(table.splitlines()[2:], ["| first | [x] 90% |", "| second | [] 10% |"])


class ExtractorOutputTest(unittest.TestCase):
    def setUp(self):
        self.gh = FakeGitHub()
        self.results = run(self.gh, PR_NUMBER, EXTRACTORS, send=FakeLaya(noul=0.8, score=6.0).extract)

    def output(self, name):
        extractor = next(e for e in EXTRACTORS if e.name == name)
        return extractor.output(self.gh, PR_NUMBER, self.results[name])

    def test_commits_are_shown_by_short_sha_and_subject(self):
        comment = self.output("commit").comment
        self.assertTrue(comment.startswith("### Commits\n"))
        self.assertIn("| `aaaaaaa` feat(auth): read token from env | 7.0 / 10 |", comment)
        self.assertEqual(comment.count("\n| `"), len(COMMITS))

    def test_code_checks_use_the_question_text(self):
        comment = self.output("checkbox").comment
        self.assertIn("### Code checks", comment)
        self.assertIn("| Does the code have unit tests that cover it? | [x] 80% |", comment)

    def test_pr_labels_also_become_labels(self):
        out = self.output("pr_labels")
        self.assertEqual(out.labels, ["review:approve", "category:code", "urgency:low", "type:feat"])
        self.assertIn("### Labels", out.comment)

    def test_a_new_schema_gets_a_section_without_any_output_code(self):
        """What a contributor adding a schema gets for free."""
        class Docs(Extractor):
            name = "docs_quality"

            def schema(self):
                return {"has_readme": {"type": "noul", "instructions": "Is the README updated?",
                                       "criteria": {"true": "yes", "false": "no"}}}

            def extract(self, gh, pr_number):
                return [Request(self.schema(), {"title": "x"}, subject="pr")]

        results = run(self.gh, PR_NUMBER, [Docs()], send=FakeLaya(noul=0.3).extract)
        out = Docs().output(self.gh, PR_NUMBER, results["docs_quality"])
        self.assertEqual(out.comment, "### Docs quality\n\n| Question | Answer |\n|---|---|\n"
                                      "| Is the README updated? | [] 30% |")
        self.assertEqual(out.labels, [])
