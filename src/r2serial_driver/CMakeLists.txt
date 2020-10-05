cmake_minimum_required(VERSION 3.0.2)
project(r2serial_driver)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  geometry_msgs
  message_generation
)

add_message_files(
  FILES
  speed_wheel.msg
  MLS_Measurement.msg
)

generate_messages(
   DEPENDENCIES
   std_msgs
)

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES r2serial_driver
   CATKIN_DEPENDS roscpp rospy std_msgs
   CATKIN_DEPENDS message_runtime
#  DEPENDS system_lib
)

#r2serial_driver
add_executable(r2serial_driver src/r2serial.cpp)
add_dependencies(r2serial_driver ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(r2serial_driver
   ${catkin_LIBRARIES})

#MD200_control_driver
add_executable(MD200_control_driver src/MD200_control_driver.cpp)
add_dependencies(MD200_control_driver ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(MD200_control_driver ${catkin_LIBRARIES})

#Navigation_cotrol
##add_executable(Navigation_cotrol src/Navigation_cotrol.cpp)
##add_dependencies(Navigation_cotrol ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
##target_link_libraries(Navigation_cotrol ${catkin_LIBRARIES})

#linefolowersick
add_executable(linefolowersick src/linefolowersick.cpp)
add_dependencies(linefolowersick ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(linefolowersick ${catkin_LIBRARIES})


include_directories(
# include
  ${catkin_INCLUDE_DIRS}
)



