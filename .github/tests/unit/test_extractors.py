import unittest

from src.extractors.checkbox import CheckboxExtractor
from src.extractors.commit import CommitExtractor
from src.extractors.pr_labels import PrLabelsExtractor
from tests.support.fakes import COMMITS, PR_NUMBER, FakeGitHub


class ExtractorTest(unittest.TestCase):
    def setUp(self):
        self.gh = FakeGitHub()

    def test_commit_sends_one_request_per_commit_without_diff(self):
        requests = CommitExtractor().extract(self.gh, PR_NUMBER)
        self.assertEqual([r.subject for r in requests], [c["sha"] for c in COMMITS])
        first = requests[0].data
        self.assertEqual(first["message"], COMMITS[0]["commit"]["message"])
        self.assertEqual((first["additions"], first["deletions"]), (2, 1))
        self.assertNotIn("patch", first["files"][0])
        self.assertIn("message_clarity", requests[0].schema)

    def test_checkbox_sends_added_lines_only(self):
        [request] = CheckboxExtractor().extract(self.gh, PR_NUMBER)
        self.assertEqual(request.subject, "pr")
        self.assertEqual(request.data["added"], {
            "src/auth.py": ["TOKEN = os.environ['TOKEN']", "TIMEOUT = 5"],
            "tests/test_auth.py": ["def test_login():", "    assert True"],
        })

    def test_pr_labels_sends_metadata_with_the_diff_last(self):
        [request] = PrLabelsExtractor().extract(self.gh, PR_NUMBER)
        data = request.data
        self.assertEqual(data["title"], "Read token from env")
        self.assertEqual(data["commits"], ["feat(auth): read token from env", "test: cover login"])
        self.assertEqual([f["filename"] for f in data["files"]], ["src/auth.py", "tests/test_auth.py", "logo.png"])
        self.assertEqual(list(data)[-1], "diff")  # Laya cuts from the end
        self.assertNotIn("logo.png", data["diff"])
