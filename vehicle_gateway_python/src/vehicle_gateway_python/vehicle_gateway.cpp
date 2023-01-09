// Copyright 2023 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <pybind11/pybind11.h>

#include "exceptions.hpp"
#include "vehicle_gateway.hpp"

namespace vehicle_gateway_python
{
VehicleGatewayPython::VehicleGatewayPython(
  const std::vector<std::string> & _args,
  const std::string & _plugin_name)
{
  this->loader_ = std::make_shared<pluginlib::ClassLoader<vehicle_gateway::VehicleGateway>>(
    "vehicle_gateway", "vehicle_gateway::VehicleGateway");

  if (_plugin_name == "px4") {
    this->gateway_ = this->loader_->createSharedInstance(
      "vehicle_gateway_px4::VehicleGatewayPX4");
    if (!this->gateway_) {
      throw InvalidHandle("cannot use create the requested API");
    }
  } else {
    throw InvalidHandle("cannot use create the requested API");
  }

  const char ** argv = new const char *[_args.size()];

  for (unsigned index = 0; index < _args.size(); index++) {
    argv[index] = _args[index].c_str();
  }

  this->gateway_->init(_args.size(), argv);
  delete[] argv;
}

void VehicleGatewayPython::Destroy()
{
  if (this->gateway_) {
    this->gateway_->destroy();
    this->gateway_ = nullptr;
  }
  if (this->loader_) {
    this->loader_->unloadLibraryForClass(
      "vehicle_gateway_px4::VehicleGatewayPX4");
    this->loader_ = nullptr;
  }
}

void VehicleGatewayPython::Arm()
{
  this->gateway_->arm();
}

void VehicleGatewayPython::Takeoff()
{
  this->gateway_->takeoff();
}

void VehicleGatewayPython::Land()
{
  this->gateway_->land();
}

VehicleGatewayPython::~VehicleGatewayPython()
{
  this->Destroy();
}

void
define_vehicle_gateway(py::object module)
{
  py::class_<VehicleGatewayPython, Destroyable, std::shared_ptr<VehicleGatewayPython>>(
    module,
    "VehicleGatewayPython")
  .def(
    py::init<const std::vector<std::string> &,
    const std::string &>())
  .def(
    "destroy", &VehicleGatewayPython::Destroy,
    "Destroy object")
  .def(
    "arm", &VehicleGatewayPython::Arm,
    "Arm vehicle")
  .def(
    "takeoff", &VehicleGatewayPython::Takeoff,
    "TakeOff")
  .def(
    "land", &VehicleGatewayPython::Land,
    "Land");
}
}  // namespace vehicle_gateway_python
