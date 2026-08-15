#ifndef ASYNC_CAMERA_SERVER_H
#define ASYNC_CAMERA_SERVER_H

#include "esp_log.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <iotCarDifferentialDrive.h>
#include <iotCmd.h>
#include <wifiInit.h>
#include <asyncWebServer.h>


void handleStream(AsyncWebServerRequest *request) {
  AsyncWebServerResponse *response = request->beginChunkedResponse(_STREAM_CONTENT_TYPE, [](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
    static camera_fb_t * fb = NULL;
    static size_t _frame_len = 0;
    static size_t _frame_sent = 0;
    static enum { STREAM_START, STREAM_HEADER, STREAM_DATA, STREAM_END } _state = STREAM_START;
    static char header_buf[128];

    size_t outLen = 0;

    while (outLen < maxLen) {
      switch (_state) {
        case STREAM_START:
          fb = esp_camera_fb_get();
          if (!fb) {
            Serial.println("Camera capture failed");
            return 0; 
          }
          _frame_len = fb->len;
          _frame_sent = 0;
          _state = STREAM_HEADER;
          // fall through
          
        case STREAM_HEADER: {
          size_t blen = snprintf(header_buf, sizeof(header_buf), _STREAM_PART, _frame_len);
          size_t avail = maxLen - outLen;
          if (avail >= blen) {
            memcpy(buffer + outLen, header_buf, blen);
            outLen += blen;
            _state = STREAM_DATA;
          } else {
            return outLen; // Handle memory buffer constraints safely
          }
        } // fall through

        case STREAM_DATA: {
          size_t avail = maxLen - outLen;
          size_t remaining = _frame_len - _frame_sent;
          size_t toSend = (remaining < avail) ? remaining : avail;
          
          memcpy(buffer + outLen, fb->buf + _frame_sent, toSend);
          outLen += toSend;
          _frame_sent += toSend;

          if (_frame_sent == _frame_len) {
            esp_camera_fb_return(fb);
            fb = NULL;
            _state = STREAM_END;
          } else {
            return outLen;
          }
        } // fall through

        case STREAM_END: {
          size_t blen = strlen(_STREAM_BOUNDARY);
          size_t avail = maxLen - outLen;
          if (avail >= blen) {
            memcpy(buffer + outLen, _STREAM_BOUNDARY, blen);
            outLen += blen;
            _state = STREAM_START; 
          } else {
            return outLen;
          }
          break;
        }
      }
    }
    return outLen;
  });

  // Prevent browser caching issues
  response->addHeader("Access-Control-Allow-Origin", "*");
  request->send(response);
}


#endif // ASYNC_CAMERA_SERVER_H