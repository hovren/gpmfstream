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

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <sstream>

#include "extractor.h"

namespace py = pybind11;

void declare_stream(py::module& m) {
  using Class = Stream;
  auto cls = py::class_<Class, std::shared_ptr<Class>>(m, "Stream");
  cls.def_readonly("key", &Class::key);
  cls.def_readonly("name", &Class::name);
  cls.def_readonly("stream_data", &Class::stream_data);
  cls.def_property_readonly("units", [](Class &self){
    std::vector<py::bytes> bytes_vector;

    for (auto& s : self.units) {
      bytes_vector.push_back(py::bytes(s));
    }

    return bytes_vector;
  });
}

void declare_streamdata(py::module& m) {
  using Class = StreamData;
  auto cls = py::class_<Class, std::shared_ptr<Class>>(m, "StreamData", py::buffer_protocol());
  cls.def_readonly("samples", &Class::samples);
  cls.def_readonly("elements", &Class::samples);
  cls.def_readonly("payload", &Class::payload);
  cls.def_readonly("buffer", &Class::buffer);

  cls.def_buffer([](Class& obj) -> py::buffer_info {
    return py::buffer_info(
      obj.buffer,
      sizeof(double),
      py::format_descriptor<double>::format(),
      2,                                         // dimesions
      {obj.samples, obj.elements},               // shape
      {sizeof(double) * obj.elements,             // strides
       sizeof(double)}
    );
  });
}

void declare_payload(py::module& m) {
  using Class = Payload;
  auto cls = py::class_<Class, std::shared_ptr<Class>>(m, "Payload");
  cls.def_readonly("index", &Class::index);
  cls.def_readonly("start", &Class::start);
  cls.def_readonly("end", &Class::end);

}

std::map<std::string, std::shared_ptr<Stream>> extract_streams(const std::string path) {
    // Throws exceptions on error
    auto extractor = ExtractGpmf(path);
    return extractor->streams;
};

PYBIND11_MODULE(_gpmfstream, m) {
  declare_stream(m);
  declare_streamdata(m);
  declare_payload(m);

  m.def("extract_streams", &extract_streams);
}