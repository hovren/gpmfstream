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
  cls.def("__repr__", [](Class &self){
    std::stringstream ss;
    ss << "<Stream of " << self.name << ", payloads=" << self.stream_data.size() << ">";
    return ss.str();
  });
}

void declare_streamdata(py::module& m) {
  using Class = StreamData;
  auto cls = py::class_<Class, std::shared_ptr<Class>>(m, "StreamData", py::buffer_protocol());
  cls.def_readonly("samples", &Class::samples);
  cls.def_readonly("elements", &Class::samples);
  cls.def_readonly("payload", &Class::payload);
  cls.def_readonly("buffer", &Class::buffer);

  cls.def("__repr__", [](Class& self){
    std::stringstream ss;
    ss << "<StreamData";
    ss << " t=[" << self.payload->start << ", " << self.payload->end << "]";
    ss << ">";
    return ss.str();
  });

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

  cls.def("__repr__", [](Class& self){
    std::stringstream ss;
    ss << "<Payload p=" << self.index;
    ss << " t=[" << self.start << ", " << self.end << "]";
    ss << ">";
    return ss.str();
  });
}

std::map<std::string, std::shared_ptr<Stream>> extract_streams(const std::string path) {
  auto extractor = ExtractGpmf(path);
  if(extractor == nullptr) {
    throw py::value_error("Failed to extrac streams");
  }

  return extractor->streams;
};

PYBIND11_MODULE(_gpmfstream, m) {

m.doc() = "GPMF (GoPro(/General Purpose) Metadata Format stream library";

  declare_stream(m);
  declare_streamdata(m);
  declare_payload(m);

  //m.def("extract_streams", &ExtractStreams);
  m.def("extract_streams", &extract_streams);
}