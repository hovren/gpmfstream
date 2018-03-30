//
// Created by hannes on 2018-03-30.
//

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <sstream>

#include "extractor.h"

namespace py = pybind11;

void declare_stream(py::module& m) {
  using Class = Stream;
  auto cls = py::class_<Class, std::shared_ptr<Class>>(m, "Stream");
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