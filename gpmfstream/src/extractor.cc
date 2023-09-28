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

std::shared_ptr<GpmfExtractor> ExtractGpmf(const std::string& path) {
  int32_t ret = GPMF_OK;
  GPMF_stream metadata_stream, *ms = &metadata_stream;
  double metadatalength;
  uint32_t *payload = nullptr; //buffer to store GPMF samples from the MP4.

  metadatalength = OpenMP4Source(const_cast<char*>(path.c_str()), MOV_GPMF_TRAK_TYPE, MOV_GPMF_TRAK_SUBTYPE);

  if (metadatalength <= 0.f) {
    throw std::invalid_argument("Failed to open GPMF source");
  }

  uint32_t index, payloads;
  payloads = GetNumberPayloads(metadatalength);

  auto extractor = std::make_shared<GpmfExtractor>();
  for (index = 0; index < payloads; index++) {
    uint32_t payloadsize = GetPayloadSize(metadatalength, index);
    payload = GetPayload(metadatalength, payload, index);

    if (payload == nullptr)
      throw std::runtime_error("Payload is null");

    auto p = std::make_shared<Payload>();
    p->index = index;

    ret = GetPayloadTime(metadatalength, index, &p->start, &p->end);
    if (ret != GPMF_OK)
      throw std::runtime_error("Could not get payload times");

    ret = GPMF_Init(ms, payload, payloadsize);
    if (ret != GPMF_OK)
      throw std::runtime_error("Could not initialize");

    // Find all streams
    while (GPMF_OK == GPMF_FindNext(ms, GPMF_KEY_STREAM, GPMF_RECURSE_LEVELS)) {
      if (GPMF_OK == GPMF_SeekToSamples(ms)) { //find the last FOURCC within the stream
        uint32_t key = GPMF_Key(ms);
        auto sd = std::make_shared<StreamData>();
        sd->payload = p;

        sd->elements = GPMF_ElementsInStruct(ms);
        sd->samples = GPMF_PayloadSampleCount(ms);

        auto key_string = Key2String(key);

        auto stream = extractor->streams[key_string];
        if(stream == nullptr) {
          stream = std::make_shared<Stream>(key_string);
          extractor->streams[key_string] = stream;
        }

        if (sd->samples) {
          sd->buffer_size = sd->samples * sd->elements * sizeof(double);
          sd->buffer = new double[sd->buffer_size];

          GPMF_ScaledData(ms, sd->buffer, sd->buffer_size, 0, sd->samples, GPMF_TYPE_DOUBLE);
          stream->stream_data.push_back(sd);

          // Extract units?
          // FIXME: If no units found we should probably not continue looking
          if (stream->units.size() == 0) {
            //Search for any units to display
            GPMF_stream find_stream;
            GPMF_CopyState(ms, &find_stream);
            if (GPMF_OK == GPMF_FindPrev(&find_stream, GPMF_KEY_SI_UNITS, GPMF_CURRENT_LEVEL) ||
                GPMF_OK == GPMF_FindPrev(&find_stream, GPMF_KEY_UNITS, GPMF_CURRENT_LEVEL))
            {
              char *data = (char *)GPMF_RawData(&find_stream);
              int ssize = GPMF_StructSize(&find_stream);
              uint32_t unit_samples = GPMF_Repeat(&find_stream);

              for (int i = 0; i < unit_samples; i++) {
                std::string unit_string(data, ssize);
                stream->units.push_back(unit_string);
                data += ssize;
              }
            }
          } // extract units

          // Extract name?
          // FIXME: If no units found we should probably not continue looking
          if (stream->name.empty()) {
            //Search for any units to display
            GPMF_stream find_stream;
            GPMF_CopyState(ms, &find_stream);
            if (GPMF_OK == GPMF_FindPrev(&find_stream, GPMF_KEY_STREAM_NAME, GPMF_CURRENT_LEVEL)) {
              char *data = (char *)GPMF_RawData(&find_stream);
              uint32_t nchars = GPMF_Repeat(&find_stream);
              std::string name_string(data, nchars);
              stream->name = name_string;
            }
          } // extract name

        }

        //extractor.streams[]
      } // SeekToSamples
    } // while next stream

  }

  return extractor;
}
