// MIT License
//
// Copyright (c) 2023 Hannes Ovrén
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef GOPRO_IMU_EXTRACT_EXTRACTOR_H
#define GOPRO_IMU_EXTRACT_EXTRACTOR_H

#include <string>
#include <vector>
#include <map>
#include <memory>

struct Payload {
  size_t index;
  float start;
  float end;
};

struct StreamData {
  double* buffer;
  size_t samples;
  size_t elements;
  size_t buffer_size;
  std::shared_ptr<Payload> payload;
};

struct Stream {
  Stream(const std::string key) : key(key) {};
  std::string key;
  std::string name;
  std::vector<std::shared_ptr<StreamData>> stream_data;
  std::vector<std::string> units;
};

struct GpmfExtractor {
  std::vector<std::shared_ptr<Payload>> payloads;
  std::map<std::string, std::shared_ptr<Stream>> streams;
};

std::shared_ptr<GpmfExtractor> ExtractGpmf(const std::string& path);

#endif //GOPRO_IMU_EXTRACT_EXTRACTOR_H
