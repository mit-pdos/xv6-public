import unittest

from src.extractors import EXTRACTORS
from src.runner import run
from tests.support.fakes import GitRepo, FakeLaya
from tests.preview.local_repo import PR_NUMBER, LocalRepo
from tests.preview.preview import report

PR = f"/pulls/{PR_NUMBER}"


class PreviewTest(unittest.TestCase):
    """The whole preview on a real branch, with a fake Laya so no model is needed."""

    def setUp(self):
        self.repo = GitRepo()
        self.addCleanup(self.repo.cleanup)
        self.repo.write("app.py", "a = 1\n")
        self.repo.commit("initial")
        self.repo.git("checkout", "-q", "-b", "feat/greeting")
        self.repo.write("greet.py", "def greet(name):\n    return f'hi {name}'\n")
        self.repo.commit("feat: add greeting")
        self.repo.write("test_greet.py", "from greet import greet\n\ndef test_greet():\n    assert greet('a')\n")
        self.repo.commit("test: cover greeting")

    def preview(self, laya):
        local = LocalRepo(self.repo.path)
        results = run(local, PR_NUMBER, EXTRACTORS, send=laya.extract)
        schemas = {e.name: e.schema() for e in EXTRACTORS}
        return report(local.get(PR), local.get_all(f"{PR}/commits"), local.get_all(f"{PR}/files"), results, schemas)

    def test_report_lists_labels_checks_and_every_commit(self):
        text = self.preview(FakeLaya(noul=0.8, score=6.0))
        self.assertIn("PR preview: feat/greeting -> main", text)
        self.assertIn('"Feat greeting"  (2 commits, 2 files, +6 -0)', text)
        self.assertIn("review     approve", text)
        self.assertIn("✔  80%  Does the code have unit tests that cover it?", text)
        self.assertIn("feat: add greeting", text)
        self.assertIn("test: cover greeting", text)
        self.assertIn("       7.0", text)  # Laya's level 6 is the 7th criterion, "7"

    def test_overall_score_averages_checks_and_commits(self):
        # checks: 0.5 each; commits: level 9 of 0-9 = 1.0 each; 4 checks + 2 commits x 4 scores
        text = self.preview(FakeLaya(noul=0.5, score=9.0))
        self.assertIn(f"Overall score: {10 * (4 * 0.5 + 8 * 1.0) / 12:.1f} / 10", text)
        self.assertIn("✘", self.preview(FakeLaya(noul=0.2)))
