from ._gpmfstream import Stream, extract_streams

import numpy as np

class Stream:
    def __init__(self, stream):
        self._stream = stream
        self._data = None
        self._timestamps = None

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


    def _assemble_data(self):
        self._data = np.vstack([sd for sd in self._stream.stream_data])
