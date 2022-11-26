#include <Arduino.h>
#include <nahs-Bricks-Feature-All.h>
#include <nahs-Bricks-Lib-RTCmem.h>
#include <ArduinoJson.h>

/*
This example shows how the processing in deliver and feedback works.
Data of the feature is going to be delivered.
By injecting feedback some data is changed and others are requested.
The request now forces the delivery to return different data than before.
*/

void setup() {
  Serial.begin(115200);
  // for repetitive runs ensure RTCmem is invalid
  RTCmem.destroy();

  // initializing the feature
  FeatureAll.setBrickType(99);
  FeatureAll.begin();
  FeatureAll.start();

  // and printing the RTCdata after init
  Serial.println("\n\nRTCdata (after begin):");
  FeatureAll.printRTCdata();

  // makeing feature to deliver it's data (usually this is done by Bricks-OS, which than transmits the data to Bricks-Server)
  Serial.println("Deliver (after begin):");
  DynamicJsonDocument out_json(1024);
  FeatureAll.deliver(&out_json);
  serializeJsonPretty(out_json, Serial);

  // now injecting some fake feedback, that requests everything from feature and also sets a new delay
  // (this feedback usually comes from Bricks-Server, and is fed to the features by Bricks-OS)
  char json[] = "{\"r\":[1,5,8],\"d\":120}";
  DynamicJsonDocument in_json(1024);
  deserializeJson(in_json, json);
  FeatureAll.feedback(&in_json);

  // let's make the RTCmem valid to get rid of the init marker in Deliver
  RTCmem.write();
  RTCmem.reCalculateCrc();

  // printing the RTCdata again after feedback is processed
  Serial.println("\n\nRTCdata (after feedback):");
  FeatureAll.printRTCdata();

  // makeing feature to deliver again, the result is now different because auf the previous feedback
  Serial.println("Deliver (after feedback):");
  out_json.clear();
  FeatureAll.deliver(&out_json);
  serializeJsonPretty(out_json, Serial);

  // but the delivery also changes some variables in RTCdata, let's print those another time to see what changed
  Serial.println("\n\nRTCdata (after deliver):");
  FeatureAll.printRTCdata();
}

void loop() {
}