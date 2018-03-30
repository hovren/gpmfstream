//
// Created by hannes on 2018-03-30.
//
#include <memory>
#include <iostream>
#include "extractor.h"

#include "GPMF_mp4reader.h"  // <- This file keeps state!
#include "GPMF_parser.h"

std::string Key2String(uint32_t key) {
  char fourcc[4+1];
  for (int i=0; i < 4; ++i)
    fourcc[i] = (key >> (i*8)) & 0xff;
  fourcc[4] = 0;
  return fourcc;
}

std::shared_ptr<GpmfExtractor> ExtractStreams(const std::string& path) {
  int32_t ret = GPMF_OK;
  GPMF_stream metadata_stream, *ms = &metadata_stream;
  double metadatalength;
  uint32_t *payload = nullptr; //buffer to store GPMF samples from the MP4.

  metadatalength = OpenGPMFSource(path.c_str());
  std::cout << "metadatalength: " << metadatalength << std::endl;

  if (metadatalength <= 0.f) {
    std::cerr << "Failed to open source" << std::endl;
    return nullptr;
  }

  uint32_t index, payloads;
  payloads = GetNumberGPMFPayloads();
  std::cout << path << " contains " << metadatalength << " seconds of metadata from " << payloads << " payloads " << std::endl;

  auto extractor = std::make_shared<GpmfExtractor>();
  for (index = 0; index < payloads; index++) {
    uint32_t payloadsize = GetGPMFPayloadSize(index);
    payload = GetGPMFPayload(payload, index);

    if (payload == nullptr)
      throw std::runtime_error("Payload is null");

    auto p = std::make_shared<Payload>();
    p->index = index;

    ret = GetGPMFPayloadTime(index, &p->start, &p->end);
    if (ret != GPMF_OK)
      throw std::runtime_error("Could not get payload times");

    ret = GPMF_Init(ms, payload, payloadsize);
    if (ret != GPMF_OK)
      throw std::runtime_error("Could not initialize");

    // Find all streams
    while (GPMF_OK == GPMF_FindNext(ms, GPMF_KEY_STREAM, GPMF_RECURSE_LEVELS)) {
      if (GPMF_OK == GPMF_SeekToSamples(ms)) { //find the last FOURCC within the stream
        uint32_t key = GPMF_Key(ms);
        //GPMF_SampleType type = GPMF_Type(ms);
        StreamData sd;
        sd.payload = p;

        sd.elements = GPMF_ElementsInStruct(ms);
        sd.samples = GPMF_PayloadSampleCount(ms);

        auto name = Key2String(key);

        auto stream = extractor->streams[name];
        if(stream == nullptr) {
          stream = std::make_shared<Stream>(name);
          extractor->streams[name] = stream;
        }

        if (sd.samples) {
          sd.buffer_size = sd.samples * sd.elements * sizeof(double);
          sd.buffer = new double[sd.buffer_size];

          GPMF_ScaledData(ms, sd.buffer, sd.buffer_size, 0, sd.samples, GPMF_TYPE_DOUBLE);
          stream->stream_data.push_back(sd);
        }

        //extractor.streams[]
      } // SeekToSamples
    } // while next stream

  }

  return extractor;
}