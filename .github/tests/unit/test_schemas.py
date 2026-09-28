import json
import unittest

from src.combine import AGGREGATES
from src.extractors import EXTRACTORS
from src.extractors.base import SCHEMAS_DIR


class SchemaTest(unittest.TestCase):
    """Every schema must be something Laya accepts, and pair up with exactly one extractor."""

    def test_every_schema_has_an_extractor_and_back(self):
        files = {p.stem for p in SCHEMAS_DIR.glob("*.json")}
        self.assertEqual(files, {e.name for e in EXTRACTORS})

    def test_questions_follow_laya_format(self):
        for path in SCHEMAS_DIR.glob("*.json"):
            for qid, q in json.loads(path.read_text()).items():
                with self.subTest(schema=path.stem, question=qid):
                    self.assertIn(q["type"], ("choice", "score", "noul"))
                    self.assertTrue(q["instructions"].strip())
                    self.assertIn(q.get("aggregate", "mean"), AGGREGATES)
                    if q["type"] == "noul":
                        self.assertEqual(set(q["criteria"]), {"true", "false"})
                    elif q["type"] == "score":
                        self.assertIsInstance(q["criteria"], list)
                        self.assertGreaterEqual(len(q["criteria"]), 2)
                    else:
                        self.assertIsInstance(q["criteria"], dict)
                        self.assertGreaterEqual(len(q["criteria"]), 2)
