#include "webserial.h"

const boolean enableWebSerial = true;

/* Message callback of WebSerial */
void recvMsg(uint8_t *data, size_t len){
  WebSerial.println("Received Data...");
  String d = "";
  for(int i=0; i < len; i++){
    d += char(data[i]);
  }
  WebSerial.println(d);
}

void setup_webserial(AsyncWebServer* server) {

    // WebSerial is accessible at "<IP Address>/webserial" in browser
    WebSerial.begin(server);
    // Attach Message Callback
    WebSerial.msgCallback(recvMsg);
}

void print(String txt) {
  if (enableWebSerial) {
    WebSerial.println(txt);
  } else {
    Serial.println(txt);
  }
}
