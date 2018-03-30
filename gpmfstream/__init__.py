from ._gpmfstream import extract_streams

import numpy as np

class Stream:
    """GPMF Stream

    Represents a GPMF stream loaded from e.g. a MP4 file.
    Handles extraction of timestamps.

    Example:
        streams = Stream.extract_streams(path)
        gyro = streams['GYRO']
        plt.plot(gyro.timestamps, gyro.data)

    """

    def __init__(self, stream):
        """Create a new stream by wrapping a raw stream

        Use the extract_streams method to extract streams.
        """
        self._stream = stream
        self._data = None
        self._timestamps = None
        self.rate = None

    def __repr__(self):
        return f"<Stream of {self.name}>"

    @classmethod
    def extract_streams(cls, path):
        """Extract streams from a GPMF source file

        Returns a map of streams, by name/fourcc.
        """
        return {
            fourcc: cls(stream)
            for fourcc, stream in extract_streams(str(path)).items()
        }

    @property
    def name(self):
        "Stream name (FOURCC)"
        return self._stream.name

    @property
    def data(self):
        "Data sample array of shape (nsamples, ndim)"
        if self._data is None:
            self._assemble_data()
        return self._data

    @property
    def timestamps(self):
        "Sample timestamps"
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


