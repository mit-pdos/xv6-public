"""Runs the real Laya model. Skipped unless LAYA_INTEGRATION=1 (needs `pip install -r requirements.txt`)."""
import json
import os
import unittest

from src.extractors.base import SCHEMAS_DIR


@unittest.skipUnless(os.environ.get("LAYA_INTEGRATION") == "1", "set LAYA_INTEGRATION=1 to run the real model")
class LayaIntegrationTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        from src.laya import Laya
        cls.laya = Laya()

    def test_every_schema_gets_an_answer_of_the_right_type(self):
        data = {"title": "feat: add greeting", "added": {"greet.py": ["def greet(name):", "    return name"]}}
        for path in SCHEMAS_DIR.glob("*.json"):
            schema = json.loads(path.read_text())
            answers = self.laya.extract(schema, data)["answers"]
            for qid, q in schema.items():
                with self.subTest(schema=path.stem, question=qid):
                    a = answers[qid]
                    if q["type"] == "noul":
                        self.assertTrue(0 <= a["noul"] <= 1)
                    elif q["type"] == "score":
                        self.assertTrue(0 <= a["score"] <= len(q["criteria"]) - 1)
                    else:
                        self.assertIn(a["choice"], q["criteria"])
