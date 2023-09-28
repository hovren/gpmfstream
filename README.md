# GPMF Streams
This package was created to extract sensor data
from files containing GPMF telemetry.
It has so far only been tested with the IMU data provided by GoPro Hero 5 and Hero 6 cameras.

## Example
Extract all streams and then plot the gyroscope data

    streams = Stream.extract_streams(path)
    gyro = streams['GYRO']
    plt.plot(gyro.timestamps, gyro.data)

## Timestamps
Since the sensor does not provide per-sample timestamps,
we must compute them explicitly.
Currently, we assume a fixed sample rate, and compute it by
simple linear interpolation.

## License
This software is released under the MIT license.

## Dependencies
The code depends on https://github.com/gopro/gpmf-parser.
To install the correct version, run 'git submodule update --init' from within
this git repo before compiling.
