import sys
import os
from pathlib import Path

import numpy as np

import matplotlib.pyplot as plt

import pygpstream

path = Path("/home/hannes/Code/gopro-imu-extract/gpmf-parser/samples/hero6.mp4")
print(path, path.exists())
streams = pygpstream.extract_streams(str(path))

for key, stream in streams.items():
    print(key, stream)

gyro = streams['GYRO']
print(gyro)

for sd in gyro.stream_data:
    arr = np.array(sd)
    print(sd, arr.shape)


gyro_data = np.vstack([sd for sd in gyro.stream_data])
print(gyro_data.shape)

payload_times = [(sd.samples, sd.payload.start, sd.payload.end) for sd in gyro.stream_data]
# Use second, and second to last to extract timing
t0 = payload_times[1][1]
t1 = payload_times[-2][2]
nsamples = sum(n for n, *rest in payload_times[1:-1])
rate = nsamples / (t1 - t0)
print(f'{nsamples} samples in {t1-t0} seconds -> {rate} Hz')
total_samples = sum(n for n, *rest in payload_times)
first_payload_nsamples = payload_times[0][0]
offset = t0 - first_payload_nsamples / rate
times = np.arange(total_samples) / rate + offset
print(repr(times))

fig, ax = plt.subplots()
for a in gyro_data.T:
    ax.plot(times, a)

# Sanity check solution (works, so-so ...)
import itertools
of = 0
for i, sd in enumerate(itertools.chain(gyro.stream_data)):
    sd_times = times[of:of+sd.samples]
    tmin = sd_times[0]
    tmax = sd_times[-1]
    if sd_times[0] < sd.payload.start or sd_times[-1] > sd.payload.end:
        print(f'{i} {sd} {tmin}, {tmax} fails')
    of += sd.samples

plt.show()