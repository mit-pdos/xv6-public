import unittest

from src.combine import combine


def result(answers, tokens=10):
    return {"answers": answers, "usage": {"input_tokens": tokens, "output_tokens": 0}}


class CombineTest(unittest.TestCase):
    def test_single_result_is_returned_unchanged(self):
        r = result({"q": {"type": "noul", "noul": 0.3}})
        self.assertIs(combine({"q": {"type": "noul"}}, [r]), r)

    def test_noul_uses_each_questions_aggregate(self):
        schema = {"secrets": {"type": "noul", "aggregate": "min"},
                  "tests": {"type": "noul", "aggregate": "max"},
                  "readable": {"type": "noul"}}
        parts = [result({q: {"noul": p} for q in schema}) for p in (0.9, 0.2, 0.7)]
        answers = combine(schema, parts)["answers"]
        self.assertEqual(answers["secrets"]["noul"], 0.2)
        self.assertEqual(answers["tests"]["noul"], 0.9)
        self.assertEqual(answers["readable"]["noul"], 0.6)
        self.assertEqual(answers["secrets"]["confidence"], 0.8)

    def test_score_is_averaged(self):
        schema = {"s": {"type": "score"}}
        parts = [result({"s": {"score": v, "confidence": 0.4}}) for v in (2.0, 6.0)]
        self.assertEqual(combine(schema, parts)["answers"]["s"]["score"], 4.0)

    def test_choice_picks_the_best_averaged_option(self):
        schema = {"c": {"type": "choice"}}
        parts = [result({"c": {"probabilities": {"a": 0.6, "b": 0.4}}}),
                 result({"c": {"probabilities": {"a": 0.1, "b": 0.9}}})]
        answer = combine(schema, parts)["answers"]["c"]
        self.assertEqual(answer["choice"], "b")
        self.assertEqual(answer["probabilities"], {"a": 0.35, "b": 0.65})

    def test_counts_chunks_and_sums_tokens(self):
        schema = {"q": {"type": "noul"}}
        combined = combine(schema, [result({"q": {"noul": 0.5}}, tokens=t) for t in (10, 20, 30)])
        self.assertEqual(combined["chunks"], 3)
        self.assertEqual(combined["usage"]["input_tokens"], 60)
