from __future__ import annotations

from statistics import mean

AGGREGATES = {"mean": mean, "min": min, "max": max}


def combine(schema: dict, results: list[dict]) -> dict:
    """Merge the Laya results of several chunks of one subject into a single result.

    Each schema question may set "aggregate": "mean" (default), "min" or "max".
    Use "min" when every chunk must pass (e.g. "free of secrets?") and "max" when
    one chunk is enough (e.g. "has tests?").
      noul:   aggregate of P(true)
      score:  aggregate of the expected level
      choice: aggregate of each option's probability, then the most likely option
    """
    if len(results) == 1:
        return results[0]
    answers = {}
    for qid, spec in schema.items():
        agg = AGGREGATES[spec.get("aggregate", "mean")]
        parts = [r["answers"][qid] for r in results]
        if spec["type"] == "noul":
            p = agg(a["noul"] for a in parts)
            answers[qid] = {"type": "noul", "noul": round(p, 4), "confidence": round(max(p, 1 - p), 4)}
        elif spec["type"] == "score":
            answers[qid] = {
                "type": "score",
                "score": round(agg(a["score"] for a in parts), 4),
                "confidence": round(mean(a["confidence"] for a in parts), 4),
            }
        else:
            probs = {k: round(agg(a["probabilities"][k] for a in parts), 4) for k in parts[0]["probabilities"]}
            answers[qid] = {"type": "choice", "choice": max(probs, key=probs.get), "probabilities": probs}
    return {
        "answers": answers,
        "chunks": len(results),
        "usage": {"input_tokens": sum(r["usage"]["input_tokens"] for r in results), "output_tokens": 0},
    }
