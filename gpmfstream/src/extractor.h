//
// Created by hannes on 2018-03-30.
//

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
  Stream(const std::string name) : name(name) {};
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
