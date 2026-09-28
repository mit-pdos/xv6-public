import unittest

from tests.support.fakes import GitRepo
from tests.preview.local_repo import PR_NUMBER, LocalRepo, parse_diff

PR = f"/pulls/{PR_NUMBER}"


class LocalRepoTest(unittest.TestCase):
    """LocalRepo must describe a branch the way GitHub would describe its PR."""

    def setUp(self):
        self.repo = GitRepo()
        self.addCleanup(self.repo.cleanup)
        self.repo.write("app.py", "a = 1\nb = 2\n")
        self.repo.write("old_name.py", "x = 1\n" * 20)
        self.repo.write("gone.txt", "bye\n")
        self.repo.commit("initial")
        self.repo.git("checkout", "-q", "-b", "feat/add-login")

    def branch_with_one_commit(self):
        self.repo.write("app.py", "a = 1\nb = 3\nc = 4\n")          # modified: +2 -1
        self.repo.write("login.py", "def login():\n    pass\n")    # added: +2
        self.repo.git("mv", "old_name.py", "new_name.py")          # renamed, same content
        self.repo.git("rm", "-q", "gone.txt")                      # removed: -1
        self.repo.write("logo.png", b"\x89PNG\x00\x01\x02")        # binary: no patch
        self.repo.commit("feat: add login\n\nWith a body.")

    def test_files_match_github_statuses_and_counts(self):
        self.branch_with_one_commit()
        files = {f["filename"]: f for f in LocalRepo(self.repo.path).get_all(f"{PR}/files")}
        self.assertEqual({n: f["status"] for n, f in files.items()}, {
            "app.py": "modified", "login.py": "added", "new_name.py": "renamed",
            "gone.txt": "removed", "logo.png": "added",
        })
        self.assertEqual((files["app.py"]["additions"], files["app.py"]["deletions"]), (2, 1))
        self.assertEqual((files["gone.txt"]["additions"], files["gone.txt"]["deletions"]), (0, 1))
        self.assertTrue(files["login.py"]["patch"].startswith("@@"))
        self.assertIsNone(files["logo.png"]["patch"])

    def test_one_commit_uses_its_subject_as_title(self):
        self.branch_with_one_commit()
        pr = LocalRepo(self.repo.path).get(PR)
        self.assertEqual(pr["title"], "feat: add login")
        self.assertEqual((pr["head"]["ref"], pr["base"]["ref"]), ("feat/add-login", "main"))
        self.assertEqual((pr["additions"], pr["deletions"]), (4, 2))

    def test_several_commits_use_the_branch_name_as_title(self):
        self.branch_with_one_commit()
        self.repo.write("login.py", "def login():\n    return True\n")
        self.repo.commit("fix: return a value")
        local = LocalRepo(self.repo.path)
        self.assertEqual(local.get(PR)["title"], "Feat add login")
        commits = local.get_all(f"{PR}/commits")
        self.assertEqual([c["commit"]["message"] for c in commits],
                         ["feat: add login\n\nWith a body.", "fix: return a value"])  # oldest first

    def test_single_commit_has_its_own_stats_and_files(self):
        self.branch_with_one_commit()
        local = LocalRepo(self.repo.path)
        sha = local.get_all(f"{PR}/commits")[0]["sha"]
        commit = local.get(f"/commits/{sha}")
        self.assertEqual(commit["stats"], {"additions": 4, "deletions": 2})
        self.assertEqual(len(commit["files"]), 5)

    def test_base_moving_on_does_not_leak_into_the_pr(self):
        self.branch_with_one_commit()
        self.repo.git("checkout", "-q", "main")
        self.repo.write("unrelated.py", "z = 0\n")
        self.repo.commit("chore: unrelated work on main")
        self.repo.git("checkout", "-q", "feat/add-login")
        names = [f["filename"] for f in LocalRepo(self.repo.path).get_all(f"{PR}/files")]
        self.assertNotIn("unrelated.py", names)

    def test_explicit_base_and_unknown_paths(self):
        self.branch_with_one_commit()
        local = LocalRepo(self.repo.path, base="main")
        self.assertEqual(local.base, "main")
        with self.assertRaises(ValueError):
            local.get("/issues/1")


class ParseDiffTest(unittest.TestCase):
    def test_counts_ignore_file_header_lines(self):
        diff = ("diff --git a/x.py b/x.py\nindex 1..2 100644\n--- a/x.py\n+++ b/x.py\n"
                "@@ -1,2 +1,2 @@\n-old\n+new\n same\n")
        [f] = parse_diff(diff)
        self.assertEqual((f["filename"], f["status"], f["additions"], f["deletions"]), ("x.py", "modified", 1, 1))
        self.assertEqual(f["patch"], "@@ -1,2 +1,2 @@\n-old\n+new\n same")
