import unittest

from src.extractors import EXTRACTORS
from src.extractors.checkbox import CheckboxExtractor
from src.runner import run
from tests.support.fakes import COMMITS, PR_NUMBER, FakeGitHub, FakeLaya


class RunnerTest(unittest.TestCase):
    def test_every_extractor_gets_a_result_per_subject(self):
        results = run(FakeGitHub(), PR_NUMBER, EXTRACTORS, send=FakeLaya().extract)
        self.assertEqual(set(results), {"checkbox", "commit", "pr_labels"})
        self.assertEqual(set(results["commit"]), {c["sha"] for c in COMMITS})
        self.assertEqual(set(results["pr_labels"]), {"pr"})

    def test_chunks_of_one_subject_are_merged(self):
        big = {"filename": "big.py", "status": "added", "additions": 400, "deletions": 0,
               "patch": "@@\n" + "\n".join(f"+value_{i} = {i}" for i in range(400))}
        gh = FakeGitHub()
        gh.get_all = lambda path: [big]
        laya = FakeLaya(noul=0.8)
        results = run(gh, PR_NUMBER, [CheckboxExtractor()], send=laya.extract)
        pr = results["checkbox"]["pr"]
        self.assertGreater(len(laya.calls), 1)
        self.assertEqual(pr["chunks"], len(laya.calls))
        self.assertEqual(pr["answers"]["q1"]["noul"], 0.8)
