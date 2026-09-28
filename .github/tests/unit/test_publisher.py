import unittest

from src.output import Output
from src.publisher import HEADING, MARKER, Publisher
from tests.support.fakes import PR_NUMBER, FakeGitHub


class PublisherTest(unittest.TestCase):
    OUTPUTS = [Output(comment="### A\n\na"), Output(labels=["type:fix"]), Output(comment="### B\n\nb")]

    def publish(self, gh, outputs=OUTPUTS):
        Publisher(gh, PR_NUMBER).publish(outputs)
        return gh.writes

    def test_first_run_posts_one_comment_with_every_section_in_order(self):
        [comment, _] = self.publish(FakeGitHub())
        self.assertEqual(comment, ("add_comment", PR_NUMBER, f"{MARKER}\n\n{HEADING}\n\n### A\n\na\n\n### B\n\nb"))

    def test_later_runs_edit_our_comment_and_ignore_other_peoples(self):
        gh = FakeGitHub(comments=[{"id": 1, "body": "LGTM"}, {"id": 2, "body": f"{MARKER}\nold"}])
        self.assertEqual(self.publish(gh)[0][:2], ("edit_comment", 2))

    def test_labels_are_added(self):
        self.assertEqual(self.publish(FakeGitHub())[1], ("add_labels", PR_NUMBER, ["type:fix"]))

    def test_stale_labels_with_the_same_key_are_replaced_and_others_kept(self):
        gh = FakeGitHub(labels=["type:feat", "urgency:high", "needs-design", "review:block"])
        writes = self.publish(gh, [Output(labels=["type:fix", "review:block"])])
        self.assertEqual(writes, [("remove_label", PR_NUMBER, "type:feat"),
                                  ("add_labels", PR_NUMBER, ["type:fix"])])

    def test_nothing_to_publish_writes_nothing(self):
        self.assertEqual(self.publish(FakeGitHub(), [Output()]), [])
