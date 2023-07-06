cmake_minimum_required(VERSION 3.8)
project(gazebo_actor_collisions_plugin)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(gazebo_ros REQUIRED)

include_directories(${GAZEBO_INCLUDE_DIRS})
link_directories(${GAZEBO_LIBRARY_DIRS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GAZEBO_CXX_FLAGS}")


add_library(ActorCollisionsPlugin SHARED ActorCollisionsPlugin.cc)
target_link_libraries(ActorCollisionsPlugin ${GAZEBO_LIBRARIES})

ament_target_dependencies(ActorCollisionsPlugin
  gazebo_ros
)

install(
  TARGETS ActorCollisionsPlugin
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)