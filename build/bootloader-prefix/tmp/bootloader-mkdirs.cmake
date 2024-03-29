# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/APPS/ESP32/IDF/esp-idf/components/bootloader/subproject"
  "E:/kaiti/CSI/Code/ESP32-S3_FallDetection/FallDetection/build/bootloader"
  "E:/kaiti/CSI/Code/ESP32-S3_FallDetection/FallDetection/build/bootloader-prefix"
  "E:/kaiti/CSI/Code/ESP32-S3_FallDetection/FallDetection/build/bootloader-prefix/tmp"
  "E:/kaiti/CSI/Code/ESP32-S3_FallDetection/FallDetection/build/bootloader-prefix/src/bootloader-stamp"
  "E:/kaiti/CSI/Code/ESP32-S3_FallDetection/FallDetection/build/bootloader-prefix/src"
  "E:/kaiti/CSI/Code/ESP32-S3_FallDetection/FallDetection/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "E:/kaiti/CSI/Code/ESP32-S3_FallDetection/FallDetection/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "E:/kaiti/CSI/Code/ESP32-S3_FallDetection/FallDetection/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
