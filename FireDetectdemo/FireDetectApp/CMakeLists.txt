cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_VERBOSE_MAKEFILE on)
PROJECT(mpreivew_app)

set(OpenCV_DIR "/usr/local/opencv/share/OpenCV/")

find_package(OpenCV REQUIRED)
message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)


set(GSTREAMER_INCLUDE_DIRS "/usr/include/gstreamer-1.0;")
set(GSTREAMER_INCLUDE_DIRS ${GSTREAMER_INCLUDE_DIRS} "/usr/lib/aarch64-linux-gnu/gstreamer-1.0/include;")
set(GSTREAMER_INCLUDE_DIRS ${GSTREAMER_INCLUDE_DIRS} "/usr/include/glib-2.0;")
set(GSTREAMER_INCLUDE_DIRS ${GSTREAMER_INCLUDE_DIRS} "/usr/lib/aarch64-linux-gnu/glib-2.0/include;")
set(GSTREAMER_LIBS  "gstapp-1.0;")
set(GSTREAMER_LIBS ${GSTREAMER_LIBS} "gstbase-1.0;")
set(GSTREAMER_LIBS ${GSTREAMER_LIBS} "gstrtsp-1.0;")
set(GSTREAMER_LIBS ${GSTREAMER_LIBS} "gstreamer-1.0;")
set(GSTREAMER_LIBS ${GSTREAMER_LIBS} "gstsdp-1.0;")
set(GSTREAMER_LIBS ${GSTREAMER_LIBS} "gio-2.0;")
set(GSTREAMER_LIBS ${GSTREAMER_LIBS} "gobject-2.0;")
set(GSTREAMER_LIBS ${GSTREAMER_LIBS} "glib-2.0;")
set(GSTREAMER_LIBRARY_DIRS  "/usr/lib/aarch64-linux-gnu;")


#OPENGL
set(OPENGL_LIBS  "GL;GLU;glut;GLEW;")

#YUV
set(YUV_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/include/;")
set(YUV_LIBRARY_DIRS  "${CMAKE_SOURCE_DIR}/lib/;")
set(YUV_LIBS  "yuv;")

set(FIREDETECT_INC_DIRS "${CMAKE_SOURCE_DIR}/include/;")
set(FIREDETECT_LIB_DIRS  "${CMAKE_SOURCE_DIR}/lib/;")
set(FireDetect "FireDetect;") 

#param
set(PARAM_CONF_LIBS "config")
set(PARAM_CONF_LIBS_DIRS "${CMAKE_SOURCE_DIR}/release/lib/")
set(PARAM_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/config/include/")

#baidu ai client
set(AI_CLIENT_INC "${CMAKE_SOURCE_DIR}/ai_client/include/")
set(AI_CLIENT_LIB "ai_client;") 


message(STATUS "    GST DEBUG:###########################")
message(STATUS "    libraries: ${GSTREAMER_LIBS}")
message(STATUS "    include path: ${GSTREAMER_INCLUDE_DIRS}")
message(STATUS "    library path: ${GSTREAMER_LIBRARY_DIRS}")

message(STATUS "    SNPE DEBUG:###########################")
message(STATUS "    libraries: ${SNPE_LIBS}")
message(STATUS "    include path: ${SNPE_INCLUDE_DIRS}")
message(STATUS "    library path: ${SNPE_LIBRARY_DIRS}")

#find_package( REQUIRED )

find_library( # Sets the name of the path variable.
              log-lib
              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

include_directories(${CMAKE_SOURCE_DIR}/
                   ${OpenCV_INCLUDE_DIRS}/
                   ${GSTREAMER_INCLUDE_DIRS}/
                   ${GSTREAMER_APP_INCLUDE_DIRS}/
                   ${GSTREAMER_RTSP_INCLUDE_DIRS}/
                   ${YUV_INCLUDE_DIRS}/
				   ${FIREDETECT_INC_DIRS}/
                   ${PARAM_INCLUDE_DIRS}/
				   ${AI_CLIENT_INC}/
				   ${CMAKE_SOURCE_DIR}/include )


link_directories(${OpenCV_LIBRARY_DIRS}/
                   ${GSTREAMER_LIBRARY_DIRS}/
                   ${YUV_LIBRARY_DIRS}/
				   ${FIREDETECT_LIB_DIRS}/
                   ${PARAM_CONF_LIBS_DIRS}
		)

ADD_SUBDIRECTORY(ai_client)
ADD_SUBDIRECTORY(config)

file(GLOB MY_SRC ${CMAKE_SOURCE_DIR}/FireDetectApp.cpp )

add_executable(fire_detect_app ${MY_SRC})


# opencv libraries


#target_link_libraries(fire_detect_app  ${PARAM_CONF_LIBS} ${OpenCV_LIBS} ${GSTREAMER_LIBS} ${YUV_LIBS} ${UIRPARSER_LIB} ${CURLI_LIB}  )
#target_link_libraries(fire_detect_app   ai_client ${PARAM_CONF_LIBS} ${OpenCV_LIBS} ${GSTREAMER_LIBS} ${YUV_LIBS}  )
target_link_libraries(fire_detect_app   ai_client config  ${FireDetect} ${OpenCV_LIBS} ${GSTREAMER_LIBS} ${YUV_LIBS}  )

set(CMAKE_CXX_FLAGS "-std=c++0x  -lm -pthread")
