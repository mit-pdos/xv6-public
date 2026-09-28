from __future__ import annotations

import threading

import laya


class Laya:
    """In-process Laya model: answers a schema's questions about GitHub data.

    The checkpoint is downloaded from Hugging Face on first use and cached.
    """

    def __init__(self, model: str = "convaiinnovations/laya", subfolder: str | None = None):
        self.agent = laya.load(model, subfolder=subfolder)
        # The runner calls extract from several threads; one model forward at a time.
        self._lock = threading.Lock()

    def extract(self, schema: dict, data: dict) -> dict:
        with self._lock:
            return self.agent.predict(data, schema)
