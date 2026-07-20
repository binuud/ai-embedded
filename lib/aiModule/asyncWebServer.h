#ifndef ASYNC_WEB_SERVER_H
#define ASYNC_WEB_SERVER_H


#include "esp_log.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#if CAR_FIXED_STEERING
#include <iotCarFixedSteering.h>
#endif

#if CAR_FLUID_STEERING
#include <iotCarFluidSteering.h>
#endif


#include <iotCmd.h>

#if CAMERA_ENABLED 
#include <cameraInit.h>
#include <asyncCamera.h>
#endif


void handleStream(AsyncWebServerRequest *request); // implement same in camera library

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char* PARAM_INPUT_1 = "cmd";
const char* PARAM_INPUT_2 = "subCmd";
const char* PARAM_INPUT_3 = "identifier";
const char* PARAM_INPUT_4 = "value1";
const char* PARAM_INPUT_5 = "value2";

// Boundary definition for MJPEG stream chunking
#define PART_BOUNDARY "123456789000000000000987654321"


// Helper function to add CORS headers to a response
void addCORSHeaders(AsyncWebServerResponse *response) {
    response->addHeader("Access-Control-Allow-Origin", "*");
    response->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    response->addHeader("Access-Control-Allow-Headers", "Content-Type");
}

void initAsyncServer() {

    // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "<div>This is a test</div>");
  });

  server.on("/update", HTTP_OPTIONS, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse(204); // 204 No Content
        addCORSHeaders(response);
        request->send(response);
    });


  // Send a GET request to <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {

    // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    if (request->hasParam(PARAM_INPUT_1) 
        && request->hasParam(PARAM_INPUT_2) 
         && request->hasParam(PARAM_INPUT_3) 
          && request->hasParam(PARAM_INPUT_4) 
           && request->hasParam(PARAM_INPUT_5) 
    ) {
      String inpCmd = request->getParam(PARAM_INPUT_1)->value();
      String subCmd = request->getParam(PARAM_INPUT_2)->value();
      String identifier = request->getParam(PARAM_INPUT_3)->value();
      String value1 = request->getParam(PARAM_INPUT_4)->value();
      String value2 = request->getParam(PARAM_INPUT_5)->value();
      Serial.printf("GPIO: cmd %s : subcmd %s id (%s) val1:(%s) val2:(%s)\n", subCmd, subCmd, identifier, value1, value2);

        IotCommand cmd;
        cmd.cmd = static_cast<DeviceCategory>(inpCmd.toInt());
        cmd.subcmd = static_cast<SubCmdEnum>(subCmd.toInt());
        cmd.identifier = identifier.toInt();
        cmd.value1 = value1.toInt();
        cmd.value2 = value2.toInt();
         debugIotCommand(&cmd);
        controlpadWithSpeed(&cmd);
    }
    else {

    }
    

    
    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", "{\"status\":\"success\"}");
    addCORSHeaders(response);
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

#if CAMERA_ENABLED 

  server.on("/stream", HTTP_GET, [](AsyncWebServerRequest *request){
    handleStream(request);
  });

#endif

  // Start server
  server.begin();
}


#endif // ASYNC_WEB_SERVER_H