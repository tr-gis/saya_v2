cmake_minimum_required(VERSION 2.8.3)
project(diff_drive_controller)

find_package(catkin REQUIRED COMPONENTS
    controller_interface
    urdf
    realtime_tools
    tf
    nav_msgs
    controller_manager
    hardware_interface
    std_msgs
    dynamixel_controllers		
 )

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES ${PROJECT_NAME}
)

include_directories(
  include ${catkin_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME} src/diff_drive_controller.cpp src/odometry.cpp src/speed_limiter.cpp)
target_link_libraries(${PROJECT_NAME} ${catkin_LIBRARIES})

install(TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
  )
	
install(FILES diff_drive_controller_plugins.xml
    DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION})




