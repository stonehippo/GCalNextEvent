#include <SPI.h>
#include <Adafruit_CC3000.h>
#include <Adafruit_CC3000_Server.h>
#include <ccspi.h>
#include <Client.h>
#include <Temboo.h>
#include "TembooAccount.h" // Contains Temboo account information
#include "GoogleAccount.h"

#define ADAFRUIT_CC3000_IRQ 3
#define ADAFRUIT_CC3000_VBAT 5
#define ADAFRUIT_CC3000_CS 10

Adafruit_CC3000 cc3k = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT);

Adafruit_CC3000_Client client;

int numRuns = 1;   // Execution count, so this doesn't run forever
int maxRuns = 10;   // Maximum number of times the Choreo should be executed

void setup() {
  Serial.begin(9600);
  
  // For debugging, wait until the serial console is connected
  delay(4000);
  while(!Serial);

  status_t wifiStatus = STATUS_DISCONNECTED;
  while (wifiStatus != STATUS_CONNECTED) {
    Serial.print("WiFi:");
    if (cc3k.begin()) {
      if (cc3k.connectToAP(WIFI_SSID, WPA_PASSWORD, WLAN_SEC_WPA2)) {
        wifiStatus = cc3k.getStatus();
      }
    }
    if (wifiStatus == STATUS_CONNECTED) {
      Serial.println("OK");
    } else {
      Serial.println("FAIL");
    }
    delay(5000);
  }

  cc3k.checkDHCP();
  delay(1000);

  Serial.println("Setup complete.\n");
}

void loop() {
  if (numRuns <= maxRuns) {
    Serial.println("Running GetNextEvent - Run #" + String(numRuns++));

    TembooChoreo GetNextEventChoreo(client);

    // Invoke the Temboo client
    GetNextEventChoreo.begin();

    // Set Temboo account credentials
    GetNextEventChoreo.setAccountName(TEMBOO_ACCOUNT);
    GetNextEventChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    GetNextEventChoreo.setAppKey(TEMBOO_APP_KEY);

    // Set Choreo inputs
    GetNextEventChoreo.addInput("RefreshToken", GOOGLE_API_REFRESH_TOKEN);
    GetNextEventChoreo.addInput("CalendarID", GOOGLE_API_CALENDAR_ID);
    GetNextEventChoreo.addInput("ClientSecret", GOOGLE_API_CLIENT_SECRET);
    GetNextEventChoreo.addInput("ClientID", GOOGLE_API_CLIENT_ID);

    // Identify the Choreo to run
    GetNextEventChoreo.setChoreo("/Library/Google/Calendar/GetNextEvent");

    // Run the Choreo; when results are available, print them to serial
    GetNextEventChoreo.run();

    while(GetNextEventChoreo.available()) {
      char c = GetNextEventChoreo.read();
      Serial.print(c);
    }
    GetNextEventChoreo.close();
  }

  Serial.println("\nWaiting...\n");
  delay(30000); // wait 30 seconds between GetNextEvent calls
}
