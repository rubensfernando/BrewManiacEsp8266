#include <ArduinoOTA.h>
#include <FS.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "config.h"
#include "HttpUpdateHandler.h"
#define RESET_WAITING_TIME 3000

#define SPIFFS_FORMAT_PATH     "/format-spiffs"
#define SPIFFS_FORMATTING_PATH "/exeformat-spiffs"


#if SerialDebug == true
#define DBG_PRINTF(...) DebugPort.printf(__VA_ARGS__)
#else
#define DBG_PRINTF(...)
#endif
const unsigned char ConfigWiFi_htm_gz[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x08, 0x80, 0x7a, 0x5a, 0x5b, 0x02, 0x03, 0x43, 0x6f,
  0x6e, 0x66, 0x69, 0x67, 0x57, 0x69, 0x46, 0x69, 0x2e, 0x6d, 0x69, 0x6e,
  0x2e, 0x68, 0x74, 0x6d, 0x6c, 0x00, 0x8d, 0x57, 0x6d, 0x73, 0xa2, 0xc8,
  0x16, 0xfe, 0x3e, 0xbf, 0x82, 0x61, 0xee, 0xa4, 0x60, 0x55, 0x34, 0xc6,
  0x64, 0x27, 0x20, 0x4e, 0x45, 0xc7, 0x49, 0x32, 0x9b, 0x98, 0x17, 0xf3,
  0x36, 0xc9, 0xe6, 0xde, 0x6a, 0xa0, 0xc1, 0x8e, 0xd0, 0x10, 0xba, 0x15,
  0x8d, 0xcb, 0x7f, 0xdf, 0xd3, 0x80, 0x8a, 0x66, 0xa6, 0xea, 0x5a, 0x56,
  0x41, 0x77, 0x9f, 0xf3, 0x9c, 0xd7, 0x7e, 0xba, 0x69, 0x7f, 0xfc, 0x76,
  0xd1, 0xbb, 0xf9, 0x79, 0xd9, 0x97, 0x46, 0x3c, 0xf0, 0x3b, 0x1f, 0xda,
  0xe2, 0x21, 0xf9, 0x88, 0x7a, 0x26, 0xa6, 0x62, 0x88, 0x91, 0x03, 0x8f,
  0x00, 0x73, 0x04, 0x12, 0x3c, 0xaa, 0xe1, 0xd7, 0x09, 0x99, 0x9a, 0xbd,
  0x90, 0x72, 0x4c, 0x79, 0xed, 0x66, 0x1e, 0x61, 0xc9, 0xce, 0x07, 0xa6,
  0xcc, 0xf1, 0x8c, 0xd7, 0x05, 0x80, 0x21, 0xd9, 0x23, 0x14, 0x33, 0xcc,
  0xcd, 0x09, 0x77, 0xbf, 0xc8, 0x4b, 0x00, 0x8a, 0x02, 0x6c, 0x4e, 0x09,
  0x4e, 0xa2, 0x30, 0xe6, 0x6b, 0xb5, 0x84, 0x38, 0x7c, 0x64, 0x3a, 0x78,
  0x4a, 0x6c, 0x5c, 0xcb, 0x06, 0x55, 0x89, 0x50, 0xc2, 0x09, 0xf2, 0x6b,
  0xcc, 0x46, 0x3e, 0x36, 0x77, 0xab, 0xd2, 0x84, 0xe1, 0x38, 0x1b, 0x21,
  0x0b, 0x26, 0x68, 0x28, 0x40, 0x39, 0xe1, 0x3e, 0xee, 0x80, 0x2f, 0x2e,
  0xf1, 0xa4, 0x7b, 0xf2, 0x9d, 0xb4, 0xeb, 0xf9, 0xd4, 0x87, 0x36, 0xb3,
  0x63, 0x12, 0xf1, 0x8e, 0x3b, 0xa1, 0x36, 0x27, 0x21, 0x95, 0x6c, 0x05,
  0xa9, 0x0b, 0x27, 0xb4, 0x27, 0x01, 0x98, 0xd4, 0x3c, 0xcc, 0xfb, 0x3e,
  0x16, 0xaf, 0xdd, 0xf9, 0xa9, 0xa3, 0xc8, 0x4c, 0x56, 0xb5, 0x29, 0xf2,
  0x27, 0xd8, 0x44, 0x1a, 0xa1, 0x14, 0xc7, 0x37, 0x10, 0xc9, 0x3f, 0xff,
  0x20, 0x4d, 0x44, 0x54, 0xc4, 0x6a, 0xfc, 0x56, 0x3b, 0x02, 0x6d, 0x17,
  0x16, 0x99, 0xa2, 0x1a, 0x31, 0xe6, 0x93, 0x98, 0x4a, 0x2e, 0xf2, 0x19,
  0x4e, 0x57, 0xd6, 0x01, 0x9b, 0x38, 0xa7, 0x97, 0x8a, 0xa5, 0x2e, 0xa6,
  0x28, 0x96, 0x5c, 0xd3, 0xd2, 0x58, 0xe4, 0x13, 0xae, 0xc8, 0x9a, 0xac,
  0x1a, 0x62, 0x0a, 0x9b, 0x0d, 0x83, 0xb8, 0x8a, 0xab, 0xf9, 0x98, 0x7a,
  0x7c, 0xf4, 0xd1, 0x6c, 0xa9, 0x8b, 0x4d, 0xac, 0x30, 0x56, 0x84, 0x20,
  0x02, 0x41, 0xd4, 0x6e, 0x19, 0xa8, 0x52, 0xc9, 0xc1, 0x1c, 0x33, 0x12,
  0xa9, 0x3e, 0xa5, 0x5c, 0x71, 0x9f, 0xd0, 0xb3, 0x6a, 0x60, 0x53, 0xc1,
  0xed, 0xf6, 0x17, 0xb5, 0xe2, 0x08, 0x44, 0xa7, 0xd3, 0xdc, 0xdf, 0xdf,
  0xc2, 0x4a, 0x8b, 0x51, 0xc9, 0x43, 0xf6, 0x3f, 0xf4, 0x82, 0x66, 0x22,
  0x49, 0x39, 0x26, 0xc5, 0x89, 0xf4, 0x70, 0x7e, 0x76, 0x02, 0x25, 0xbf,
  0x86, 0x92, 0x63, 0xc6, 0x21, 0x38, 0x47, 0x0b, 0x69, 0x0c, 0x0d, 0x31,
  0x67, 0x1c, 0x71, 0x0c, 0x25, 0xa6, 0x1e, 0x36, 0x97, 0x10, 0x8a, 0xba,
  0x10, 0xe6, 0xb4, 0x4c, 0x60, 0x28, 0x04, 0x4c, 0x11, 0x44, 0x36, 0x27,
  0xe4, 0x27, 0xcc, 0x34, 0x9b, 0x8d, 0x86, 0xba, 0x40, 0x1a, 0x9b, 0xd8,
  0x36, 0x66, 0x2c, 0x13, 0x66, 0x51, 0x48, 0x19, 0x16, 0xf9, 0x56, 0x53,
  0x0c, 0xce, 0x2d, 0x84, 0x11, 0x1c, 0xc7, 0x10, 0xee, 0x52, 0x4f, 0x4d,
  0xd3, 0x34, 0xb3, 0xcd, 0x49, 0x80, 0xc3, 0x09, 0xdf, 0x32, 0xc9, 0xa1,
  0x07, 0x43, 0x57, 0x82, 0x62, 0xe5, 0xcb, 0x1f, 0x4d, 0x79, 0x42, 0x1d,
  0xec, 0x12, 0x8a, 0x1d, 0x59, 0x98, 0x2b, 0xe6, 0x95, 0x77, 0x06, 0x6a,
  0xbb, 0x00, 0x5d, 0x5d, 0x8d, 0xd7, 0xb8, 0xd6, 0x26, 0xb0, 0x8b, 0x88,
  0xff, 0x0e, 0x55, 0x4c, 0x82, 0x60, 0xee, 0x5a, 0x84, 0xa9, 0x82, 0xb4,
  0xa0, 0x8a, 0xb4, 0x49, 0xec, 0x57, 0x79, 0x3c, 0xc1, 0xaa, 0x51, 0x46,
  0x70, 0x10, 0x47, 0x5b, 0x08, 0x10, 0x1d, 0xe6, 0x45, 0x6e, 0x4f, 0x20,
  0x67, 0x38, 0x56, 0xe4, 0xf2, 0xb6, 0x92, 0xab, 0x6b, 0xf5, 0x00, 0x22,
  0xd8, 0x54, 0xff, 0x9a, 0x4f, 0xea, 0x32, 0x8a, 0xa0, 0x91, 0x6c, 0x24,
  0xdc, 0xae, 0xcf, 0x6a, 0x49, 0x92, 0xd4, 0xa0, 0x53, 0x82, 0x1a, 0xb8,
  0x81, 0xa9, 0x1d, 0x3a, 0x42, 0xd6, 0x10, 0xa6, 0xa8, 0xa3, 0xe4, 0x6e,
  0xac, 0xb2, 0x90, 0xcd, 0x81, 0xff, 0xa2, 0xe0, 0x57, 0xeb, 0xd8, 0xd1,
  0xaa, 0x59, 0x56, 0x0d, 0x0f, 0x3e, 0xc6, 0xf3, 0x21, 0xf6, 0xb1, 0xcd,
  0x21, 0x6b, 0x80, 0x90, 0xb5, 0xec, 0x00, 0xb6, 0xf7, 0x42, 0xec, 0x53,
  0xbd, 0x54, 0x0f, 0x3e, 0x22, 0x4c, 0x83, 0xce, 0xc6, 0x81, 0x79, 0x05,
  0xdd, 0x4d, 0x13, 0x9f, 0x30, 0x0e, 0x2e, 0xac, 0xe7, 0x35, 0x68, 0x57,
  0x00, 0x1d, 0x80, 0x6f, 0x50, 0xfe, 0x20, 0x9c, 0xe2, 0xde, 0x88, 0xf8,
  0x8e, 0xb2, 0x96, 0x28, 0xa4, 0x21, 0x3d, 0x93, 0xe8, 0x7e, 0x08, 0x2e,
  0x56, 0x69, 0x82, 0xa7, 0xa0, 0xa3, 0x97, 0x0b, 0x94, 0x6f, 0x06, 0x21,
  0x59, 0xca, 0xb4, 0xa5, 0x09, 0x7b, 0xef, 0x6a, 0x25, 0x26, 0x95, 0x7c,
  0xad, 0x08, 0xbf, 0xac, 0xc2, 0x18, 0x71, 0xb6, 0x54, 0x60, 0x79, 0x35,
  0x0f, 0xc3, 0x84, 0x50, 0x27, 0x4c, 0x34, 0x3f, 0xcc, 0x13, 0x0d, 0x7e,
  0xfb, 0x21, 0x02, 0x97, 0x45, 0x9d, 0xa1, 0x39, 0xd3, 0x6a, 0x0c, 0xb2,
  0xfa, 0xfb, 0x14, 0x2a, 0xa8, 0xd3, 0x50, 0xbf, 0xca, 0x5f, 0x65, 0xfd,
  0x1c, 0xf1, 0x11, 0x54, 0x8c, 0x2a, 0xf9, 0x0b, 0x6c, 0xb5, 0xe6, 0x1f,
  0x0a, 0xaa, 0xec, 0xc2, 0x86, 0xa8, 0xc2, 0x5f, 0x3c, 0xd3, 0xaa, 0xf0,
  0x6f, 0x8d, 0xe2, 0xe6, 0x31, 0x5a, 0x22, 0x91, 0x9f, 0x28, 0xe6, 0x49,
  0x18, 0x8f, 0x81, 0xa8, 0x0c, 0x2b, 0xa7, 0xa8, 0x93, 0x9b, 0xf3, 0x33,
  0x70, 0xce, 0x58, 0x12, 0x83, 0x03, 0xc4, 0xe0, 0xb4, 0x97, 0x04, 0x62,
  0x38, 0x4b, 0x7e, 0xc8, 0x73, 0x54, 0xe4, 0xde, 0xf6, 0xa1, 0xd7, 0x45,
  0xea, 0x73, 0xdf, 0x0d, 0x54, 0x22, 0x33, 0xd6, 0x9d, 0xdf, 0x20, 0x6f,
  0x00, 0x1c, 0xad, 0xc8, 0x48, 0x56, 0x9f, 0x1a, 0xcf, 0x25, 0x43, 0xee,
  0x93, 0xf3, 0x9c, 0x25, 0xa4, 0x60, 0xab, 0xdf, 0x29, 0xb2, 0x08, 0xd1,
  0x4c, 0xd7, 0xc0, 0x1b, 0x6e, 0x56, 0x32, 0x27, 0x44, 0x9a, 0x94, 0x0c,
  0x4a, 0xbc, 0xa9, 0x15, 0xf9, 0xb3, 0x9c, 0xb1, 0x9e, 0x98, 0x81, 0x76,
  0x55, 0x17, 0x18, 0x3c, 0x44, 0x8c, 0x09, 0x2c, 0xb3, 0xf4, 0x5e, 0x91,
  0x25, 0x5f, 0x4e, 0x2d, 0x0d, 0x9a, 0x1d, 0x9a, 0x36, 0xef, 0x17, 0x24,
  0xf6, 0x6e, 0xd1, 0x23, 0xe5, 0x0e, 0x2c, 0xf5, 0x05, 0xd2, 0x12, 0x96,
  0x31, 0xd9, 0x3d, 0xb6, 0x86, 0xa1, 0x3d, 0xc6, 0x40, 0xb8, 0x09, 0xd3,
  0xeb, 0x75, 0xb9, 0xb2, 0xea, 0xed, 0x55, 0x4d, 0x47, 0x21, 0xe3, 0x15,
  0xb9, 0x9e, 0x88, 0x14, 0x0b, 0x45, 0x60, 0x05, 0xc8, 0x16, 0xc3, 0x1b,
  0xac, 0x00, 0x87, 0x51, 0x0c, 0x20, 0xf7, 0x43, 0x29, 0x5b, 0xd3, 0xe5,
  0x8a, 0xa5, 0x89, 0x5d, 0x06, 0x5b, 0xa2, 0xd0, 0x09, 0x80, 0xd8, 0x50,
  0x99, 0x16, 0x9d, 0x65, 0x19, 0xeb, 0xff, 0xfd, 0x9b, 0xfd, 0xa1, 0xfc,
  0x9d, 0x54, 0x54, 0x78, 0xea, 0xe2, 0x7d, 0xa1, 0x55, 0xd4, 0xff, 0xd4,
  0x35, 0x3c, 0xc3, 0x36, 0x90, 0x5d, 0xb6, 0x39, 0x45, 0x3a, 0xac, 0x9d,
  0x1d, 0xeb, 0x69, 0xf7, 0xd9, 0x14, 0x27, 0xa3, 0x4b, 0xb2, 0x06, 0x2e,
  0x36, 0x80, 0xf2, 0x63, 0x78, 0x31, 0xd0, 0x32, 0xbe, 0x57, 0xac, 0xa7,
  0xe6, 0xb3, 0x2a, 0xba, 0xaf, 0x0a, 0xa7, 0x22, 0x7d, 0x9f, 0x81, 0xad,
  0xae, 0x41, 0xe5, 0x72, 0x0c, 0x41, 0x83, 0x12, 0xea, 0x69, 0x9a, 0x26,
  0x1b, 0x05, 0xf5, 0x2f, 0x02, 0x5d, 0x3e, 0xee, 0xdf, 0xc8, 0x55, 0xa0,
  0x0e, 0x1d, 0xf2, 0x00, 0xa6, 0x05, 0xb0, 0x5c, 0x2d, 0xd8, 0xba, 0x6c,
  0x21, 0x4d, 0xb7, 0x4e, 0xba, 0x2a, 0x43, 0x53, 0xbc, 0xed, 0x83, 0x65,
  0xca, 0x34, 0x31, 0xe5, 0x4a, 0x4e, 0x44, 0xb7, 0xd7, 0xa7, 0xbd, 0x30,
  0x00, 0xb6, 0x17, 0x71, 0x08, 0xe7, 0x56, 0x87, 0x6e, 0x16, 0xb4, 0x98,
  0x89, 0x96, 0x33, 0xf9, 0xae, 0xb3, 0x4c, 0xab, 0x22, 0xef, 0x44, 0xd0,
  0x03, 0xbf, 0x07, 0x59, 0xa9, 0xa8, 0x69, 0xde, 0x98, 0xcb, 0xa3, 0x36,
  0xb3, 0x00, 0xc7, 0x07, 0xb1, 0xc9, 0x5a, 0xc6, 0xc8, 0x37, 0x4a, 0x59,
  0xc6, 0x83, 0x83, 0x2a, 0x41, 0xf3, 0x4d, 0x11, 0xb4, 0x21, 0x02, 0x59,
  0x0c, 0x10, 0x1b, 0x6f, 0xb8, 0x8b, 0x77, 0x76, 0x9c, 0x9d, 0x1d, 0xb4,
  0x74, 0x92, 0x44, 0xe0, 0xe2, 0xaf, 0x4c, 0x6a, 0x3c, 0x26, 0x81, 0x02,
  0x9d, 0xbe, 0xe3, 0x25, 0x85, 0xc8, 0x96, 0xc5, 0xb5, 0x04, 0x0d, 0x0a,
  0x89, 0x2d, 0x83, 0x85, 0x44, 0x5a, 0x2a, 0xd4, 0xe5, 0xc5, 0x70, 0xa3,
  0x52, 0x70, 0x97, 0x92, 0xab, 0xa2, 0x81, 0x74, 0xeb, 0xff, 0xaa, 0x57,
  0x6a, 0xb4, 0xeb, 0xc5, 0x15, 0x09, 0xee, 0x4a, 0x7c, 0x0e, 0x77, 0x26,
  0xcd, 0x5e, 0x88, 0xdb, 0x4e, 0x0d, 0x02, 0xf7, 0xa8, 0x6e, 0x43, 0x82,
  0x71, 0x9c, 0x3a, 0x64, 0x5a, 0x25, 0x34, 0x9a, 0xf0, 0x45, 0x84, 0x1c,
  0x07, 0x3a, 0x46, 0xdf, 0x8f, 0x66, 0x40, 0x39, 0x70, 0x46, 0x31, 0xf2,
  0x86, 0xf5, 0x5d, 0x1c, 0xa4, 0xf9, 0x7a, 0x76, 0x6f, 0xd3, 0x0f, 0xf7,
  0x3f, 0xa7, 0x56, 0xe8, 0xcc, 0xdf, 0x43, 0xe5, 0x4a, 0x2e, 0x0a, 0x88,
  0x3f, 0xd7, 0xa7, 0x38, 0x76, 0x10, 0x45, 0xa9, 0x35, 0xe1, 0x3c, 0xa4,
  0x0b, 0x2b, 0x8c, 0xe1, 0xf8, 0xd3, 0x1b, 0x46, 0xfe, 0x52, 0x8b, 0x91,
  0x43, 0x26, 0x4c, 0xd7, 0xf6, 0xe0, 0x7c, 0x30, 0x2c, 0x64, 0x8f, 0xbd,
  0x38, 0x04, 0x76, 0xae, 0xd9, 0xa1, 0x1f, 0xc6, 0xfa, 0xa7, 0x5d, 0x17,
  0xed, 0x61, 0xdb, 0x28, 0x46, 0xae, 0xeb, 0x1a, 0x3e, 0xd0, 0x76, 0x6d,
  0x84, 0x89, 0x37, 0xe2, 0x7a, 0x53, 0x6b, 0x09, 0xb5, 0x92, 0x8f, 0x5a,
  0x53, 0x4c, 0xe4, 0x0e, 0x02, 0xd1, 0x7e, 0x4e, 0xb5, 0xd7, 0x85, 0x0b,
  0x04, 0xce, 0xf5, 0x58, 0x68, 0x14, 0x2b, 0x07, 0x2d, 0x88, 0xac, 0xe4,
  0x76, 0xb6, 0x96, 0x6a, 0xfe, 0x62, 0xed, 0x80, 0x0e, 0x09, 0x57, 0xe4,
  0x2c, 0xcd, 0x24, 0x80, 0x4d, 0x5e, 0x8f, 0xa8, 0x07, 0xfe, 0x31, 0x7c,
  0xd0, 0xaa, 0x92, 0xbb, 0xee, 0xc5, 0x75, 0xd2, 0xf8, 0xeb, 0xd8, 0x0b,
  0x8f, 0xe0, 0x37, 0x18, 0xde, 0x8e, 0xfa, 0xb7, 0x1e, 0xbc, 0xf5, 0xc4,
  0xf0, 0xc8, 0xeb, 0x1d, 0x9d, 0xc3, 0xa3, 0xdb, 0x8f, 0x4e, 0xe3, 0x63,
  0x31, 0x71, 0x76, 0xd7, 0x3d, 0xbf, 0xeb, 0x3f, 0xd4, 0xeb, 0xf5, 0x2f,
  0xfd, 0x6e, 0xe2, 0x76, 0x13, 0x76, 0x96, 0x7c, 0xb9, 0x3c, 0x7a, 0x1b,
  0xbc, 0xa0, 0x9e, 0xd7, 0x1a, 0xdc, 0xdc, 0xdd, 0xdd, 0xbe, 0xfc, 0x20,
  0x8f, 0xdf, 0xae, 0x6f, 0x6f, 0x6f, 0xbf, 0xcf, 0x1c, 0xf2, 0x78, 0x3c,
  0x1c, 0x85, 0x07, 0x17, 0xc3, 0xf1, 0xfe, 0xa5, 0xd7, 0xc2, 0xdf, 0xe7,
  0xce, 0xc9, 0x4d, 0xef, 0x05, 0xb9, 0x7b, 0x02, 0xeb, 0xb1, 0xef, 0xf7,
  0xaf, 0xee, 0xae, 0x5a, 0x2f, 0xb8, 0x39, 0x18, 0x26, 0x7f, 0x1e, 0x9d,
  0x1e, 0x8d, 0xfa, 0x5d, 0x14, 0xfc, 0x45, 0x0f, 0xff, 0xac, 0x4f, 0xce,
  0x1f, 0xfa, 0xc7, 0xdd, 0x69, 0xf8, 0x36, 0xbe, 0xb7, 0x0e, 0x7b, 0xcd,
  0xc7, 0x59, 0x6b, 0xf6, 0x76, 0x3f, 0x1f, 0x77, 0x47, 0xdf, 0x8f, 0xf0,
  0xcf, 0xe8, 0xd0, 0x1b, 0x9f, 0xcd, 0x1f, 0xfb, 0x8d, 0xb7, 0xd3, 0x73,
  0x1a, 0x1e, 0xd2, 0x96, 0xb7, 0x7b, 0x38, 0x0a, 0x9c, 0x9f, 0x7b, 0x87,
  0xcc, 0x4e, 0x5e, 0xef, 0xc6, 0x17, 0x0f, 0x68, 0x16, 0x8d, 0x1a, 0x8f,
  0xbd, 0x87, 0x2b, 0xfb, 0x75, 0x36, 0x8c, 0xbc, 0xab, 0xe8, 0x62, 0x80,
  0xf6, 0x0f, 0x93, 0xf1, 0xf5, 0xb7, 0x8b, 0xb3, 0xc3, 0x3d, 0x7c, 0xf4,
  0x30, 0x25, 0x41, 0xe2, 0x5b, 0x97, 0x56, 0x92, 0xdc, 0x1d, 0x61, 0xef,
  0x6c, 0xb8, 0x7b, 0x72, 0xec, 0x3e, 0x66, 0x21, 0x77, 0x7f, 0x5c, 0xdf,
  0xee, 0xf7, 0xe3, 0xf1, 0x0f, 0xcf, 0xf3, 0x80, 0xcb, 0x54, 0x89, 0x86,
  0xb5, 0x18, 0x47, 0x18, 0x71, 0xc9, 0xc7, 0x2e, 0xdc, 0xff, 0xf3, 0xbe,
  0x28, 0xd5, 0x77, 0xd5, 0x52, 0xd0, 0x9c, 0x59, 0x4f, 0x7e, 0x68, 0xd7,
  0x8b, 0x0f, 0x10, 0xd1, 0x54, 0x52, 0x48, 0xc5, 0xb1, 0x6b, 0xc2, 0xc5,
  0x43, 0x13, 0xf7, 0x0e, 0x45, 0x85, 0x05, 0xe8, 0x50, 0x29, 0x13, 0x36,
  0x4b, 0xb5, 0x13, 0xf8, 0x86, 0x43, 0xe0, 0x8e, 0x8d, 0xe6, 0x3a, 0xa1,
  0x59, 0x7b, 0x58, 0xc0, 0xf4, 0x63, 0x03, 0x8e, 0xe0, 0xfc, 0x13, 0x43,
  0x6f, 0x1e, 0x34, 0xa2, 0x59, 0x01, 0x40, 0xc4, 0x85, 0x37, 0xa7, 0xca,
  0x62, 0x26, 0x3b, 0x7a, 0xcc, 0xfc, 0xfa, 0x02, 0x53, 0xf0, 0xf1, 0x13,
  0x63, 0xd7, 0xfc, 0x24, 0x89, 0x43, 0x81, 0xd8, 0x63, 0xd3, 0xce, 0xee,
  0x2b, 0x6a, 0x21, 0x27, 0x0e, 0xc6, 0xce, 0x20, 0x47, 0x68, 0xd7, 0x51,
  0x67, 0x87, 0x5a, 0x2c, 0x32, 0xda, 0xe2, 0x30, 0x2c, 0x24, 0x5e, 0x3b,
  0x10, 0x13, 0x0c, 0xe1, 0x01, 0xf0, 0x22, 0xb0, 0xfc, 0xb1, 0xb6, 0x65,
  0x77, 0xde, 0x5b, 0x91, 0x8b, 0x4d, 0x2b, 0x22, 0x16, 0x64, 0xac, 0xa8,
  0x72, 0x47, 0x70, 0xb7, 0xb0, 0xb1, 0x44, 0x10, 0xd7, 0x3d, 0x09, 0x3e,
  0xaf, 0x46, 0xa1, 0x63, 0xc2, 0x99, 0x2c, 0xa1, 0x6c, 0xd3, 0x9b, 0x9f,
  0x60, 0x29, 0xdb, 0x99, 0x22, 0x38, 0x96, 0x7f, 0x7a, 0x31, 0x29, 0xbf,
  0x1c, 0x98, 0x7b, 0x4d, 0x09, 0x52, 0x63, 0xe3, 0x51, 0xe8, 0xc3, 0x7e,
  0x33, 0x87, 0xc3, 0xd3, 0x6f, 0x9d, 0xb6, 0x15, 0x97, 0x55, 0xa2, 0x5c,
  0x25, 0x5a, 0xaa, 0x1c, 0xb4, 0x24, 0x71, 0x61, 0x32, 0x05, 0x1d, 0x43,
  0x98, 0xce, 0x06, 0xc0, 0x72, 0x32, 0x03, 0xd9, 0x02, 0x5a, 0x32, 0x62,
  0xe1, 0xc2, 0x72, 0x54, 0xc0, 0xee, 0x6e, 0xe0, 0xc8, 0xc3, 0x6c, 0x59,
  0x3a, 0xbd, 0x94, 0xb7, 0x61, 0x0a, 0xd6, 0xcc, 0x51, 0x96, 0x83, 0x5f,
  0x82, 0x1c, 0xe7, 0x8b, 0xdb, 0x00, 0x05, 0xa9, 0xe6, 0x00, 0xcb, 0xc1,
  0xaf, 0xbd, 0x80, 0x74, 0x4b, 0xe7, 0x82, 0x80, 0x4b, 0xf1, 0xe4, 0x54,
  0x95, 0xe7, 0x80, 0x4d, 0xac, 0x80, 0xf0, 0x5f, 0x57, 0x09, 0xce, 0x41,
  0x51, 0x25, 0xf1, 0x6c, 0xd7, 0x73, 0x25, 0x51, 0x6d, 0x51, 0xa5, 0x75,
  0xd5, 0xeb, 0xa2, 0x9d, 0xb3, 0xee, 0x16, 0x1f, 0xdd, 0xff, 0x02, 0x41,
  0x4e, 0x82, 0x68, 0x84, 0x0f, 0x00, 0x00
};
static const char indexhtml[] PROGMEM = R"END(
<!doctype html>
<html>
<head>
<meta charset=utf-8 name=viewport content="width=device-width, initial-scale=1.0, minimum-scale=1.0, maximum-scale=1.0, user-scalable=no">
<title>BrewManiac Update</title>
<style>.fwstat,.jsstat{display:none}body{background-color:#e5e6ea}</style>
<script>eval(function(p,a,c,k,e,r){e=function(c){return(c<a?'':e(parseInt(c/a)))+((c=c%a)>35?String.fromCharCode(c+29):c.toString(36))};if(!''.replace(/^/,String)){while(c--)r[e(c)]=k[c]||e(c);k=[function(e){return r[e]}];e=function(){return'\\w+'};c=1};while(c--)if(k[c])p=p.replace(new RegExp('\\b'+e(c)+'\\b','g'),k[c]);return p}('3 o(i){5.8(i).B.Y=\'1O\'}3 C(i){5.8(i).B.Y=\'1g\'}3 1h(a){6 b=1P.1i.Z;a=a.1j(/[\\[\\]]/g,"\\\\$&");6 c=K 1Q("[?&]"+a+"(=([^&#]*)|&|#|$)"),L=c.1R(b);7(!L)10 1k;7(!L[2])10\'\';10 1S(L[2].1j(/\\+/g," "))}3 q(a){6 b=K 1T();b.1U=3(){7(b.1V==4){7(b.1l==1W){a.s(b.1X)}9{7(D a["H"]!="E")a.H(b.1l);9 11("1Y 1m.")}}};b.1Z=3(){7(D a["M"]!="E")a.M();9 b.1n()},b.1n=3(){7(D a["H"]!="E")a.H(-1);9 11("21 1m.")};b.22(a.m,a.n,1o);7(D a["N"]!="E")b.23("24-25",a.N);7(D a["k"]!="E"){b.1p(a.k)}9 b.1p()}3 12(){6 a=5.1q("26");27(i=0;i<a.13;i++){a[i].28=1o}}3 O(p){14.w=p}O.29.15=3(a,b,h,f){o(14.w+"16");6 t=14;f=(f)?f:2a;q({m:"I",n:a,s:3(r){C(t.w+"16");k=P.Q(r);7(!f&&b==k.R){o(t.w+"2b")}9{h(k);o(t.w+"2c")}},H:3(){C(t.w+"16");o(t.w+"17")}})};3 18(a,b,c){y(3(){1i.2d()},a*1r);5.8(b).j=c+"<1s>2e 2f 2g 2h <1t 2i=1u>"+a+"</1t> 2j.</1s>";1v(3(){19=5.8("1u");19.j=""+(2k(19.j)-1)},1r)}3 S(){q({m:"I",n:l.2l,M:3(){5.8("J").1w(\'1x\',"...");y(S,F)},s:3(r){d=P.Q(r);7(d.1y){7(D d["1z"]!="E"){18(d.1z,"J","2m 2n.")}9{5.8("J").j=d.2o}}9{5.8("J").1w(\'1x\',"...");y(S,F)}}})}3 2p(){C("2q");o("J");12();q({m:"1A",n:l.2r,N:"1B/x-1C-1D-1E",k:"1F="+1a(T),s:3(){y(S,F)}})}6 u;6 z;3 U(){q({m:"I",n:l.2s,M:3(){y(U,F)},s:3(r){k=P.Q(r);7(k.1y){7(k.17!=0){11("17:"+k.2t)}9{z++;1b(z);7(z==u.v.13){q({m:"I",n:"/2u-2v",s:3(){18(20,"1G","2w 2x 2y.")}})}9{1c(z)}}}9{y(U,F)}}})}3 1c(i){k="V="+u.v[i].V+"&G="+1a(u.v[i].G);7(u.v[i].V!="2z")k+="&1H="+1a(u.v[i].1H);q({m:"1A",n:l.2A,N:"1B/x-1C-1D-1E",k:k,s:3(){y(U,F)}})}3 2B(){o("A");5.8("1G").j="2C..";12();z=0;1c(z);1b(0)}3 2D(){7(5.8("A").B.Y=="1g")o("A");9 C("A")}6 T="";3 1I(){6 e=(1h("1J")!=1k);5.8("2E").j=""+l.1K;6 f=K O("2F");f.15(l.T+"&1L="+l.1L,l.1K,3(d){5.8("2G").j=""+d.R;5.8("2H").Z=d.n;5.8("2I").j=d.2J;T=d.1F},e);5.8("2K").j=""+l.1d;6 g=K O("2L");6 h=(e)?"&1J=1":"";g.15(l.2M+l.1d+h,l.1d,3(d){5.8("2N").j=""+d.R;5.8("2O").Z=d.n;5.8("2P").j=""+d.v.13;C("A");6 c=5.W("1e");u=d;d.v.1M(3(a,i){6 b=5.W("1N");b.j="["+a.V+"] "+a.G;c.X(b)});5.8("A").X(c)},e)}3 1b(d){6 e=5.W("1e");u.v.1M(3(a,i){6 b=5.W("1N");7(i==d){b.j="[2Q]"+a.G;6 c=b;1v(3(){7(c.B.1f==0)c.B.1f=1;9 c.B.1f=0},2R)}9 7(i<d)b.j="[2S] "+a.G;9 b.j="[2T] "+a.G;e.X(b)});6 f=5.8("A");6 g=f.1q("1e")[0];f.2U(g);f.X(e)}6 l={};3 2V(){q({m:"I",n:"R.2W",s:3(r){l=P.Q(r);1I()}})}',62,183,'|||function||document|var|if|getElementById|else||||||||||innerHTML|data|ver_info||url|show||invoke||success||jsfiles|list|pre||setTimeout|processingIndex|filedetial|style|hide|typeof|undefined|3000|dst|fail|GET|fw_progress|new|results|timeout|mime|SoftwareUpdater|JSON|parse|version|fwprogress|fwurl|jsprogress|action|createElement|appendChild|display|href|return|alert|noSubmit|length|this|loadinfo|initial|error|countdown|ele|encodeURIComponent|updatelist|processFile|js|ul|opacity|none|getPara|location|replace|null|status|Error|onerror|true|send|getElementsByTagName|1000|h2|span|COUNTDOWN|setInterval|insertAdjacentHTML|beforeend|finished|refresh|POST|application|www|form|urlencoded|source|listdes|src|checkUpdate|forced|fw|opt|forEach|li|block|window|RegExp|exec|decodeURIComponent|XMLHttpRequest|onreadystatechange|readyState|200|responseText|Unexpected|ontimeout||Communication|open|setRequestHeader|Content|Type|input|for|disabled|prototype|false|latest|avail|reload|Wait|to|reconnect|in|id|seconds|parseInt|fwus|Update|Succeeded|result|updatefw|fw_avail|fwu|jsus|msg|system|reboot|All|file|updated|del|jsu|updatejs|Updating|showfilelist|fw_version|fw_|newversion|infolink|fsize|size|js_version|js_|jsurl|jsnewversion|jsinfolink|fileno|Processing|500|DONE|wait|removeChild|init|php'.split('|'),0,{}))</script>
</head>
<body onload=init()>
<H2>BrewManiac ESP8266 System update</H2>
<h3>Firmware version: <span id=fw_version>0.0</span></h3>
<div id=fw_initial class=fwstat>Getting firmware information..</div>
<div id=fw_error class=fwstat>Failed to get information. Check network connection or <a href=http://vito.tw/brewmaniac>check webside</a></div>
<div id=fw_latest class=fwstat>Latest version. No update available.</div>
<div id=fw_avail class=fwstat><h3>Latest version: <span id=newversion></span></h3>Size: <span id=fsize></span> bytes
<br />
<a href id=infolink target=_blank>Detail..</a> <form type=POST action=javascript:updatefw();><input type=submit value="update Now"></form>
</div>
<div id=fw_progress class=fwstat>Sart updating...</div>
<hr />
<h3>HTML/Javascript version: <span id=js_version>0.0</span></h3>
<div id=js_initial class=jsstat>Getting HTML/Javascript information..</div>
<div id=js_error class=jsstat>Failed to get information. Check network connection or <a href=http://vito.tw/brewmaniac>check webside</a></div>
<div id=js_latest class=jsstat>All files are up to date.</div>
<div id=js_avail class=jsstat><h3>Latest version: <span id=jsnewversion></span></h3>
<a href=javascript:showfilelist()><span id=fileno></span> files</a> to be updated. <a href id=jsinfolink target=_blank>Detail..</a> <form type=POST action=javascript:updatejs();><input type=submit value="update Now"></form>
<div id=filedetial><span id=listdes>Update list:</span></div>
</div>
</body>
</html>
)END";

static const char spiffsformat_html[] PROGMEM = R"END(
<html>
<head>
<title>SPIFFS Format</title>
<script>eval(function(p,a,c,k,e,r){e=function(c){return c.toString(a)};if(!''.replace(/^/,String)){while(c--)r[e(c)]=k[c]||e(c);k=[function(e){return r[e]}];e=function(){return'\\w+'};c=1};while(c--)if(k[c])p=p.replace(new RegExp('\\b'+e(c)+'\\b','g'),k[c]);return p}('2 3(){4(5.6("0").7){1 8}9{a("b c 0 d e f g h.");1 i}};',19,19,'sure|return|function|makesure|if|document|getElementById|checked|true|else|alert|Please|make|you|know|what|will|happen|false'.split('|'),0,{}))</script>
</head>
<body>
By click the "Format" button. The SPIFFS will be formated.<br>
Wait for 60 seconds for formating the file system.
<form onsubmit="return makesure()" action="exeformat-spiffs">
<input type="checkbox" id="sure"> I know all files and data will be gone.<br>
<input type="submit" value="Format">
</form>
</body>
</html>
)END";

static const char spiffsformating_html[] PROGMEM = R"END(
<html>
<head>
<title>SPIFFS Format</title>
<script>/*<![CDATA[*/eval(function(p,a,c,k,e,r){e=function(c){return c.toString(a)};if(!''.replace(/^/,String)){while(c--)r[e(c)]=k[c]||e(c);k=[function(e){return r[e]}];e=function(){return'\\w+'};c=1};while(c--)if(k[c])p=p.replace(new RegExp('\\b'+e(c)+'\\b','g'),k[c]);return p}('2 7(){0 a=3.4();8(2(){0 c=9.d((3.4()-a)/5);e(c<6){0 b=f.g("h");b.i=""+(6-c)}j{1.k=1.l+"//"+1.m}},5)};',23,23,'var|location|function|Date|now|1000|60|count|setInterval|Math||||round|if|document|getElementById|sec|innerHTML|else|href|protocol|hostname'.split('|'),0,{}))/*]]>*/</script>
</head>
<body onload=count()>
Please Wait. Keep Calm and let the formatting carry on. Don't touch anything until the page reloaded.<br>
Formating... <span id="sec" style="font-size:32">60</span>
</body>
</html>
)END";

void HttpUpdateHandler::_sendVersion(AsyncWebServerRequest *request)
{
	// construct customer parameter
	String param="{\"fw\":\"" + _fwVersion
			+"\",\"opt\":\"" + BUILD_OPTIONS
			+"\",\"fwurl\":\"" + _fwInfoUrl
			+"\",\"fwu\":\"/updatefw\",\"fwus\":\"/updatefwq\" ,\"js\":\"" + _jsVersion
			+ "\",\"jsurl\":\"" + _jsInfoUrl
			+"\",\"jsu\":\"/updatejs\",\"jsus\":\"/updatejsq\"}";
	request->send(200,"application/json",param);
}

void HttpUpdateHandler::_sendHtml(AsyncWebServerRequest *request, const char *html)
{
	AsyncWebServerResponse *response = request->beginResponse(String("text/html"),
  		strlen_P(html),
  		[=](uint8_t *buffer, size_t maxLen, size_t alreadySent) -> size_t {
    		if (strlen_P(html+alreadySent)>maxLen) {
      		// We have more to read than fits in maxLen Buffer
      		memcpy_P((char*)buffer, html+alreadySent, maxLen);
      		return maxLen;
    	}
    	// Ok, last chunk
    	memcpy_P((char*)buffer, html+alreadySent, strlen_P(html+alreadySent));
    	return strlen_P(html+alreadySent); // Return from here to end of indexhtml
 	 }
	);
//	if(gzip) response->addHeader("Content-Encoding", "gzip");
	request->send(response);
}

void HttpUpdateHandler::_firmwareUpdateStatus(AsyncWebServerRequest *request)
{
	if(_state == US_Idle){
		if(_updateReturn==HTTP_UPDATE_FAILED) {
        	DBG_PRINTF("[update] Update failed.");
	      	request->send(200, "application/json", "{\"finished\":1, \"result\":\"Error:"+ ESPhttpUpdate.getLastErrorString() +"\"}");
        }else if(_updateReturn == HTTP_UPDATE_NO_UPDATES){
		    DBG_PRINTF("[update] Update no Update.");
	      	request->send(200, "application/json", "{\"finished\":1, \"result\":\"Nothing to Update.\"}");
		}else if(_updateReturn == HTTP_UPDATE_OK){
			DBG_PRINTF("[update] Update ok.");
	      	request->send(200, "application/json", "{\"finished\":1, \"refresh\":15, \"result\":\"OK\"}");
	      	// start timer to reset
	      	_state=US_RestartInitiated;
		}else{
			request->send(200, "application/json", "{\"finished\":1, \"result\":\"unknown error\"}");
		}
	}else if (_state == US_FirmwareUpdatePending || _state == US_FirmwareUpdating){
		DBG_PRINTF("[RSP] Updating...\n");
		request->send(200, "application/json",R"({"finished":0})");
	}else{
		request->send(500);
	}
}

void HttpUpdateHandler::_firmwareUpdateStart(AsyncWebServerRequest *request)
{
	if(_state != US_Idle){
		request->send(403);
		return;
	}

	if(request->hasParam("source", true)){
  		AsyncWebParameter* p = request->getParam("source", true);
		_firmwareUpdateUrl= p->value();

		request->send(200);

		_state = US_FirmwareUpdatePending;
	}else{
		request->send(404);
	}

}

void HttpUpdateHandler::_scriptUpdateStart(AsyncWebServerRequest *request)
{
	if(_state != US_Idle){
		request->send(403);
		return;
	}

	if(!request->hasParam("action", true) || !request->hasParam("dst", true)){
		request->send(403);
		return;
	}
	//else
	AsyncWebParameter* p = request->getParam("action", true);
	AsyncWebParameter* d = request->getParam("dst", true);

	String filename=d->value();
	if(!filename.startsWith("/")) filename = "/"+filename;

	if(p->value().equals("new")){
	  	if(!request->hasParam("src", true)){
		  		request->send(403);
	  			return;
	  	}
		String src=request->getParam("src",true)->value();
	  	request->send(200);
	  	_fileDownloader.begin(src,filename);

	  	_state=US_FileDownloadPending;

	}else if(p->value().equals("del")){
		SPIFFS.remove(filename);
	  	request->send(200);
	}
}

void HttpUpdateHandler::_scriptUpdateStatus(AsyncWebServerRequest *request)
{
	DBG_PRINTF("JS update status, state=%d\n",_state);
	if(_state == US_Idle){
		//finished
		if(_fileDownloader.isSuccess()){
			request->send(200, "application/json","{\"finished\":1,\"error\":0}");
		}else{
			String output="{\"finished\":1,\"error\":"+ String(_fileDownloader.errorCode())
						+",\"msg\":\"" + _fileDownloader.errorMessage() + "\"}";
			request->send(200, "application/json",output);
		}
	}else if(_state == US_FileDownloadPending || _state == US_FileDownloading){
		DBG_PRINTF("Query download Status...\n");
		request->send(200, "application/json",R"({"finished":0})");
	}else {
		request->send(500);
	}
}


bool HttpUpdateHandler::canHandle(AsyncWebServerRequest *request)
{
    if(request->url().equals("/version.php")
    	|| request->url().equals(_url)
    	|| request->url().equals(SPIFFS_FORMAT_PATH)
    	|| request->url().equals(SPIFFS_FORMATTING_PATH)
    	|| request->url().equals("/updatefw")
    	|| request->url().equals("/updatefwq")
    	|| request->url().equals("/updatejs")
    	|| request->url().equals("/updatejsq")
    	|| request->url().equals("/system-reboot"))

        return true;
	return false;
}


void HttpUpdateHandler::handleRequest(AsyncWebServerRequest *request)
{
	if(_user!=NULL && _pass!=NULL && !request->authenticate(_user, _pass))
		return request->requestAuthentication();

    if(request->url().equals("/version.php")){
	    _sendVersion(request);
    }else if(request->url().equals(_url)){

		if(WiFi.status() == WL_CONNECTED)  _sendHtml(request,indexhtml);
		else{
				 AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", ConfigWiFi_htm_gz, sizeof(ConfigWiFi_htm_gz));
                response->addHeader("Content-Encoding", "gzip");
                request->send(response);

		}
   	}else if(request->url().equals(SPIFFS_FORMAT_PATH)) {
		_sendHtml(request,spiffsformat_html);
    }else if(request->url().equals(SPIFFS_FORMATTING_PATH)){
		_sendHtml(request,spiffsformating_html);
		_state = US_FormatPending;
    }else if(request->url().equals("/updatefw")){
    	_firmwareUpdateStart(request);
    }else if(request->url().equals("/updatefwq")){
	    _firmwareUpdateStatus(request);
    }else if(request->url().equals("/updatejs")){
    	_scriptUpdateStart(request);
    }else if(request->url().equals("/updatejsq")){
    	_scriptUpdateStatus(request);
    }else if(request->url().equals("/system-reboot")){
    	if(_state == US_Idle){
    		request->send(200);
	    	_state = US_RestartInitiated;
	    }
	}
}

void HttpUpdateHandler::runUpdate(void)
{
	if(_state ==US_FirmwareUpdatePending){
		_state = US_FirmwareUpdating;
		DBG_PRINTF("Start http update:%s\n",_firmwareUpdateUrl.c_str());
		ESPhttpUpdate.rebootOnUpdate(false);
		_updateReturn = ESPhttpUpdate.update(_firmwareUpdateUrl, _fwVersion);
		DBG_PRINTF("End of http update\n");

		_state = US_Idle;
	}else if(_state ==US_FileDownloadPending){
		_state= US_FileDownloading;
		DBG_PRINTF("Start file download\n");
		_fileDownloader.download();
		DBG_PRINTF("End file download\n");
		_state = US_Idle;
	}else if(_state == US_RestartInitiated){
		_resetInitiatedTimer= millis();
		_state = US_RestartPending;
	}else if(_state == US_RestartPending){
		if((millis() - _resetInitiatedTimer) > RESET_WAITING_TIME)
		 	ESP.restart();
	}else if(_state ==US_FormatPending){
		_state= US_Formating;
		DBG_PRINTF("Start Formating SPIFFS\n");
		SPIFFS.format();
		DBG_PRINTF("End Formating SPIFFS\n");
		_state = US_RestartInitiated;
	}

}


void HttpFileDownloader::download(void)
{
 	// configure server and url

    File f=SPIFFS.open(_filename,"w+");
    if(!f){
    	DBG_PRINTF("file open failed\n");
		_error = -1;
		_errorMsg = "Error open filed!";

    	_finished=true;
    	return;
    }
	DBG_PRINTF("Download %s\n",_filename.c_str());

	_buff =(uint8_t*) malloc(FLASH_SECTOR_SIZE);
	if(!_buff){
		DBG_PRINTF("error allocate memory\n");
		_error = -1;
		_errorMsg = "Not enough resource.";

		_finished=true;
		return;
	}

    _http.begin(_url);

    DBG_PRINTF("[HTTP] GET %s...\n",_url.c_str());
    // start connection and send HTTP header
    int httpCode = _http.GET();
    if(httpCode > 0) {
    	// HTTP header has been send and Server response header has been handled
        DBG_PRINTF("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if(httpCode == HTTP_CODE_OK) {

            // get lenght of document (is -1 when Server sends no Content-Length header)
            int len = _http.getSize();
            int wlen=0;
			DBG_PRINTF("[HTTP] Content Length: %d\n", len);
            // create buffer for read
            //uint8_t buff[512] = { 0 };

            // get tcp stream
            WiFiClient * stream = _http.getStreamPtr();
            // read all data from server

            uint8_t *ptr =_buff;
            size_t space=FLASH_SECTOR_SIZE;
            while(_http.connected() && (len > 0 || len == -1)) {
                // get available data size
            	size_t size = stream->available();

                if(size) {
                	int c = stream->readBytes(ptr, ((size > space) ? space : size));
					DBG_PRINTF("read %d bytes\n",c);
					ptr += c;
					space -= c;
                    if(len > 0) len -= c;

                    if(space == 0) {
	                    DBG_PRINTF("write %d bytes\n",FLASH_SECTOR_SIZE);
                        ESP.wdtDisable();
                        f.write(_buff,FLASH_SECTOR_SIZE);
                        ESP.wdtEnable(10);
                        ptr =_buff;
                        space=FLASH_SECTOR_SIZE;
                        wlen += space;
                    }
                }
                delay(1);
            }
            if(space < FLASH_SECTOR_SIZE){
				DBG_PRINTF("write %d bytes\n",FLASH_SECTOR_SIZE-space);
                ESP.wdtDisable();
                f.write(_buff,FLASH_SECTOR_SIZE - space);
                wlen += FLASH_SECTOR_SIZE-space;
                ESP.wdtEnable(10);
            }

			if(len == 0 || len == -1){
				_error = 0;
			}else{
				_error = -1;
				_errorMsg = "Connection broken!";
			}

            DBG_PRINTF("[HTTP] connection closed. written %d.\n",wlen);

    	} else {
    		// httpCode != OKs
    		DBG_PRINTF("[HTTP] Response:%d\n",httpCode);
    		_error = httpCode;
    		_errorMsg="HTTP code:" + String(_error);
    	}
    } else {
    	// httpCode <=0
        DBG_PRINTF("[HTTP] GET... failed, error: %s\n", _http.errorToString(httpCode).c_str());
        _error=httpCode;
        _errorMsg=_http.errorToString(httpCode);
    }
	f.close();
	free(_buff);

    _http.end();
    _finished=true;
}


