/*
 * Copyright (C) 2018 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#include <string>
#include <map>
#include <ignition/math/Vector3.hh>

#include <gazebo/physics/Actor.hh>
#include <gazebo/physics/BoxShape.hh>
#include <gazebo/physics/Collision.hh>
#include <gazebo/physics/SurfaceParams.hh>
#include <gazebo/physics/Link.hh>
#include "ActorCollisionsPlugin.hpp"


namespace gazebo
{

ActorCollisionsPlugin::ActorCollisionsPlugin()
{
}

void ActorCollisionsPlugin::Load(physics::ModelPtr _model, sdf::ElementPtr _sdf)
{
  // Get a pointer to the actor
  auto actor = boost::dynamic_pointer_cast<physics::Actor>(_model);

  // Map of collision scaling factors
  std::map<std::string, ignition::math::Vector3d> scaling;
  std::map<std::string, ignition::math::Pose3d> offsets;
  // Read in the collision scaling factors, if present
  if (_sdf->HasElement("scaling")) {
    auto elem = _sdf->GetElement("scaling");
    while (elem) {
      if (!elem->HasAttribute("collision")) {
        gzwarn << "Skipping element without collision attribute" << std::endl;
        elem = elem->GetNextElement("scaling");
        continue;
      }
      auto name = elem->Get<std::string>("collision");

      if (elem->HasAttribute("scale")) {
        auto scale = elem->Get<ignition::math::Vector3d>("scale");
        scaling[name] = scale;
      }

      if (elem->HasAttribute("pose")) {
        auto pose = elem->Get<ignition::math::Pose3d>("pose");
        offsets[name] = pose;
      }
      elem = elem->GetNextElement("scaling");
    }
  }

  for (const auto & link : actor->GetLinks()) {
    // Init the links, which in turn enables collisions
    link->Init();

    if (scaling.empty()) {
      continue;
    }

    // Process all the collisions in all the links
    for (const auto & collision : link->GetCollisions()) {
      auto name = collision->GetName();

      // Set bitmask for collisions so actors won't collide with actors
      collision->GetSurface()->collideBitmask = this->actor_bitmask;

      if (scaling.find(name) != scaling.end()) {
        auto boxShape = boost::dynamic_pointer_cast<gazebo::physics::BoxShape>(
          collision->GetShape());

        // Make sure we have a box shape.
        if (boxShape) {
          boxShape->SetSize(boxShape->Size() * scaling[name]);
        }
      }

      if (offsets.find(name) != offsets.end()) {
        collision->SetInitialRelativePose(offsets[name] + collision->InitialRelativePose());
      }
    }

    auto inertial = link->GetInertial();
    inertial->SetMass(0.0001);
    inertial->SetInertiaMatrix(0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001);
  }
}

// Register this plugin with the simulator
GZ_REGISTER_MODEL_PLUGIN(ActorCollisionsPlugin)

}  // namespace gazebo
