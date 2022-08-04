cmake_minimum_required (VERSION 3.8)

project ("Smart Car")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PaddleLite QUIET)
find_package(OpenCV REQUIRED)

message(STATUS "PaddleLite found (${PADDLELITE_LIBRARY})")
message(STATUS "OpenCV found (${OpenCV_CONFIG_PATH})，${OpenCV_LIBS}")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")

include_directories(${PADDLELITE_INCLUDE_DIR})
include_directories(${OpenCV_INCLUDE_DIRS})

if (PADDLELITE_FOUND)
	message("-- PADDLELITE_FOUND")

	add_executable(trackTest "example/trackTest.cpp" "include/track.hpp" "include/PID.hpp")
	#add_executable(cameraTest "example/cameraTest.cpp")
	#add_executable(smartcar "src/smartcar.cpp" "src/track.cpp" "src/PID.cpp" "src/serialPort.cpp" "src/predict.cpp")
	#add_executable(predict "example/predictTest.cpp" "src/predict.cpp")
	
	link_directories(${OpenCV_LIBRARY_DIRS})

	#target_link_libraries(smartcar ${OpenCV_LIBS})
	#target_link_libraries(smartcar ${PADDLELITE_LIBRARY})
	target_link_libraries(trackTest ${OpenCV_LIBS})
	#target_link_libraries(cameraTest ${OpenCV_LIBS})
	#target_link_libraries(predict ${PADDLELITE_LIBRARY})
	#target_link_libraries(predict ${OpenCV_LIBS} ) 

else ()
	message(FATAL_ERROR "Error !!!! : PADDLELITE NOT FOUND")
endif ()