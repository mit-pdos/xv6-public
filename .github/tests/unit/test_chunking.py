import json
import unittest

from src.extractors.base import CHUNK_CHARS, chunk_added_lines, trim_files, trim_patches

HEADER = {"title": "Some PR"}


def patch(lines):
    return "@@ -1 +1 @@\n" + "\n".join(lines)


class ChunkAddedLinesTest(unittest.TestCase):
    def test_keeps_only_added_lines_without_the_plus(self):
        files = [{"filename": "a.py", "patch": patch([" context", "-removed = 'secret'", "+added = 1"])}]
        self.assertEqual(chunk_added_lines(files, HEADER), [{**HEADER, "added": {"a.py": ["added = 1"]}}])

    def test_every_chunk_fits_the_limit_and_has_the_header(self):
        files = [{"filename": f"f{i}.py", "patch": patch([f"+line_{i}_{j} = {j}" for j in range(60)])}
                 for i in range(5)]
        chunks = chunk_added_lines(files, HEADER)
        self.assertGreater(len(chunks), 1)
        for c in chunks:
            self.assertLessEqual(len(json.dumps(c)), CHUNK_CHARS)
            self.assertEqual(c["title"], HEADER["title"])

    def test_no_line_is_lost_or_duplicated(self):
        lines = [f"x_{i} = {i}" for i in range(300)]
        files = [{"filename": "a.py", "patch": patch("+" + l for l in lines)}]
        got = [l for c in chunk_added_lines(files, HEADER) for l in c["added"]["a.py"]]
        self.assertEqual(got, lines)

    def test_very_long_line_is_cut_to_fit(self):
        files = [{"filename": "min.js", "patch": patch(["+" + "x" * 50_000])}]
        [chunk] = chunk_added_lines(files, HEADER)
        self.assertLessEqual(len(json.dumps(chunk)), CHUNK_CHARS)

    def test_nothing_added_still_gives_one_request(self):
        files = [{"filename": "a.py", "patch": patch(["-gone"])}, {"filename": "img.png"}]
        self.assertEqual(chunk_added_lines(files, HEADER), [HEADER])


class TrimTest(unittest.TestCase):
    def test_trim_files_drops_patch_and_github_noise(self):
        f = {"filename": "a.py", "status": "added", "additions": 1, "deletions": 0,
             "patch": "+x", "sha": "abc", "blob_url": "https://..."}
        self.assertEqual(trim_files([f]), [{"filename": "a.py", "status": "added", "additions": 1, "deletions": 0}])

    def test_trim_patches_stops_at_the_budget(self):
        files = [{"filename": n, "patch": "x" * 600} for n in ("a", "b", "c")]
        patches = trim_patches(files, limit=1000)
        self.assertEqual(list(patches), ["a", "b"])
        self.assertEqual(sum(map(len, patches.values())), 1000)
