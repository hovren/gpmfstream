// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>
//
// Copyright 2018 Hannes Ovrén

#ifndef GOPRO_IMU_EXTRACT_EXTRACTOR_H
#define GOPRO_IMU_EXTRACT_EXTRACTOR_H

#include <string>
#include <vector>
#include <map>
#include <memory>

struct Payload {
  size_t index;
  double start;
  double end;
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
  std::vector<StreamData> stream_data;
  std::vector<std::string> units;
};

struct GpmfExtractor {
  std::vector<std::shared_ptr<Payload>> payloads;
  std::map<std::string, std::shared_ptr<Stream>> streams;
};

std::shared_ptr<GpmfExtractor> ExtractGpmf(const std::string& path);

#endif //GOPRO_IMU_EXTRACT_EXTRACTOR_H
