from ._gpmfstream import extract_streams

import numpy as np

class Stream:
    def __init__(self, stream):
        self._stream = stream
        self._data = None
        self._timestamps = None
        self.rate = None

    @classmethod
    def extract_from(cls, path):
        return {
            fourcc: cls(stream)
            for fourcc, stream in extract_streams(str(path)).items()
        }

    @property
    def name(self):
        return self._stream.name

    @property
    def data(self):
        if self._data is None:
            self._assemble_data()
        return self._data

    @property
    def timestamps(self):
        if self._timestamps is None:
            self._compute_timestamps()
        return self._timestamps

    def _assemble_data(self):
        self._data = np.vstack([sd for sd in self._stream.stream_data])

    def _compute_timestamps(self):
        # Simple linear method
        payload_times = [(sd.samples, sd.payload.start, sd.payload.end) for sd in self._stream.stream_data]
        # Use second, and second to last to extract timing
        t0 = payload_times[1][1]
        t1 = payload_times[-2][2]
        nsamples = sum(n for n, *rest in payload_times[1:-1])
        self.rate = nsamples / (t1 - t0)
        total_samples = sum(n for n, *rest in payload_times)
        first_payload_nsamples = payload_times[0][0]
        offset = t0 - first_payload_nsamples / self.rate
        self._timestamps = np.arange(total_samples) / self.rate + offset


