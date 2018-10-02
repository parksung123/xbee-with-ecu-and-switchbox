#include "SoftwareSerial.h"
#include <XBee.h>

uint8_t Rx = 12;
uint8_t Tx = 13;
SoftwareSerial mySerial(Rx, Tx);
XBee xbee = XBee();
XBee xbee2 = XBee();
XBeeResponse response = XBeeResponse();

ZBRxIoSampleResponse ZBioSample1 = ZBRxIoSampleResponse();
ZBRxIoSampleResponse ZBioSample2 = ZBRxIoSampleResponse();
ZBRxIoSampleResponse ZBioSample3 = ZBRxIoSampleResponse();
ZBRxIoSampleResponse ZBioSample4 = ZBRxIoSampleResponse();
ZBRxIoSampleResponse ZBioSample5 = ZBRxIoSampleResponse();
ZBRxResponse rx = ZBRxResponse();

// buttons on Switch Box
uint8_t switchList[4] = {0, 1, 11, 12};

// LEDs corresponding to tube list
uint8_t tubeList[5] = {3, 4, 5, 6, 7};

// LEDs corresponding to the valves
uint8_t valveList[6] = {14, 15, 16, 17, 18, 19};
uint8_t resetPin = 11;

// incoming info
uint8_t tube, dur, cycle, valve, mode;

uint8_t startB, pumpB, igB, resetB, purgeB;
uint8_t startS, pumpS, igS, resetS, purgeS;
uint8_t cnt; // counter

// different modes
uint8_t igniterMode = 0;
uint8_t pumpMode = 0;
uint8_t startMode = 0;
uint8_t resetMode = 0;

uint8_t numberOfPrint = 1;
uint8_t testNum = 0;
uint8_t checkValve = 0;
uint8_t valveType = 0;
uint8_t printRate = 0;
uint8_t coldRun = 0;
uint8_t firingRun = 0;
uint8_t resetNum = 0;

void resetFunc() {
  Serial.println("=============RESET==============");
  testNum = 0;
  checkValve = 0;
  valveType = 0;
  printRate = 0;
  coldRun = 0;
  firingRun = 0;
  resetNum = 0;

  startS = !ZBioSample1.isDigitalOn(switchList[3]); // returns 0, false
  resetS = !ZBioSample4.isDigitalOn(switchList[2]); // returns 0
  pumpS = !ZBioSample2.isDigitalOn(switchList[1]); // returns 0
  igS = !ZBioSample3.isDigitalOn(switchList[0]); // returns 0

  if (startS == 0)
    Serial.println("STATUS: OFF");
  
  if (pumpS == 0)
    Serial.println("PUMP: OFF");
 
  if (igS == 0)
    Serial.println("IGNITER: OFF");
}

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  xbee.begin(mySerial);

  startS = !ZBioSample1.isDigitalOn(switchList[3]); // returns 0, false
  resetS = !ZBioSample4.isDigitalOn(switchList[2]); // returns 0
  pumpS = !ZBioSample2.isDigitalOn(switchList[1]); // returns 0
  igS = !ZBioSample3.isDigitalOn(switchList[0]); // returns 0
  purgeS = !ZBioSample5.isDigitalOn(switchList[3]);

  Serial.println("STATUS: OFF");
  Serial.println("IGNITER: OFF");
  Serial.println("PUMP: OFF");
  // sets type for corresponding LEDs
  for (cnt = 1; cnt < 4; cnt++)
    pinMode(tubeList[cnt], OUTPUT);
  
  for (cnt = 0; cnt < 6; cnt++)
    pinMode(valveList[cnt], OUTPUT);
  
  pinMode(resetPin, OUTPUT);
}

void tubeAll(int ctr) {
  digitalWrite(tubeList[ctr], HIGH);
  delay(dur);
  digitalWrite(tubeList[ctr], LOW);
  delay(50);
}

void tubeIndi(int ctr) {
  digitalWrite(tubeList[cnt], HIGH);
  delay(dur);
  digitalWrite(tubeList[cnt], LOW);
  delay(dur);
}

void valveDetermine(int ctr) {

  if (purgeB) {
    switch (ctr) {
      case 0 :
        Serial.println("OFF");
        break;
      case 1 :
        if (printRate % 10 == 0)
          Serial.println("AIR");
        printRate++;
        break;
      case 2 :
        Serial.println("O2");
        break;
      case 3 :
        Serial.println("N2");
        break;
      case 4 :
        Serial.println("KRS");
        break;
      case 5 :
        Serial.println("IGN");
        break;
    }
  }
}
void loop() {
  xbee.readPacket();

  if (xbee.getResponse().isAvailable()) {
    if (xbee.getResponse().getApiId() == ZB_IO_SAMPLE_RESPONSE) {
      xbee.getResponse().getZBRxIoSampleResponse(ZBioSample1);
      xbee.getResponse().getZBRxIoSampleResponse(ZBioSample2);
      xbee.getResponse().getZBRxIoSampleResponse(ZBioSample3);
      xbee.getResponse().getZBRxIoSampleResponse(ZBioSample4);

      startB = !ZBioSample1.isDigitalOn(switchList[3]);
      pumpB = !ZBioSample2.isDigitalOn(switchList[1]);
      igB = !ZBioSample3.isDigitalOn(switchList[0]);
      resetB = !ZBioSample4.isDigitalOn(switchList[2]);

      if (resetB != resetS & resetB == 1) { 
        // check if the igniter button state has changed and pressed
        if (resetMode == 1) {
          resetMode = 0;
          Serial.println("Reset: OFF");
        }
        else if (resetMode == 0) {
          resetMode = 1;
          Serial.println("Reset: ON");
          resetFunc();
        }
      }
      resetS = resetB;  

      if (startB != startS && startB == 1) { 
        // check if the igniter button state has changed and if it's pressed
        Serial.print("STATUS: ");
        if (startMode == 1) {
          startMode = 0;
          Serial.println("OFF");
        }
        else if (startMode == 0) {
          startMode = 1;
          Serial.println("ON");
        }
      }
      startS = startB;

      if (igB != igS && igB == 1) { 
        // check if the igniter button state has changed and if it's pressed
        Serial.print("IGNITER: ");
        if (igniterMode == 1) {
          igniterMode = 0;
          Serial.println("OFF");
        }
        else if (igniterMode == 0) {
          igniterMode = 1;
          Serial.println("ON");
        }
      }
      igS = igB;

      if (pumpB != pumpS && pumpB == 1) { 
        // check if the pump button state has changed and if it's pressed
        Serial.print("PUMP: ");
        if (pumpMode == 1) {
          pumpMode = 0;
          Serial.println("OFF");
        }
        else if (pumpMode == 0) {
          pumpMode = 1;
          Serial.println("ON");
        }
      }
      pumpS = pumpB;
    }

    if (resetMode == 0) {
      if (xbee.getResponse().isAvailable()) {
        if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
          xbee.getResponse().getZBRxResponse(rx);
          tube = rx.getData(0); // tube number
          dur = rx.getData(1); // duration length
          cycle = rx.getData(2); // number of cycle
          valve = rx.getData(3); // valve option
          mode = rx.getData(4); // mode option (8 / 9);

          // This is a Valve TEST
          if (mode == 0) {
            // Pump and Igniter Switches are OFF
            if (pumpMode == 0 && igniterMode == 0) {
              // Print this message only once
              if (testNum != numberOfPrint) {
                Serial.println("TEST MODE"); // make it only print once by using bool
                testNum++;
              }

              // If any of tube status 1, 2, 3, run this code
              if (tube == 1 || tube == 2 || tube == 3) {
                // each tube has different valves 
                // If the Start Switch is pressed, run this code
                // purgeB = !ZBioSample5.isDigitalOn(switchList[3]);
                if (purgeB == 1) {
                  // Print this message only once
                  if (checkValve != numberOfPrint) {
                    Serial.println("TUBE " + String(tube) + " : CHECKING VALVE"); // make it only print once by using bool
                    checkValve++;
                  }
                  
                  for (cnt = 0; cnt < 6; cnt++) {
                    if (valve == cnt) {
                      // Depending on which valve it is set to, run the corresponding code
                      valveDetermine(cnt);
                      digitalWrite(valveList[cnt], HIGH);
                    }
                  }
                }
              }
              // If we want to test the valve of ALL Tubes
              else if (tube == 4) {
                // When Start Switch is pressed, run this code
                if (startMode == 1) {
                  for (cnt = 0; cnt < 6; cnt++) {
                    // Check which valve the Switch is set to
                    if (valve == cnt) {
                      // Print this message once
                      if (checkValve != numberOfPrint) {
                        Serial.println("VALVE " + String(cnt) + " : TESTING"); // STUD for valve selector signals!!
                        checkValve++;
                      }
                    }
                  }
                }
              }
            }
          }
          // This is a COLD RUN
          else if (mode == 1) {
            // 1 = RUN
            // Igniter Switch is OFF and Pump Switch is ON
            if (igniterMode == 0 && pumpMode == 1) {
              if (coldRun != numberOfPrint) {
                Serial.println("VALVE " + String(cnt) + " : TESTING"); // STUD for valve selector signals!!
                coldRun++;
              }

              if (startMode == 1) {
                if (tube > 0 && tube < 4) {
                  for (cnt = 1; cnt < 4; cnt++) {
                    if (tube == cnt)
                      tubeIndi(cnt);
                  }
                }
                else if (tube == 4) {
                  for (cnt = 1; cnt < 4; cnt++) 
                    tubeAll(cnt);
                }
              }
            }
            else if (igniterMode == 1 && pumpMode == 1) {
              if (firingRun != numberOfPrint) {
                Serial.println("VALVE " + String(cnt) + " : TESTING"); // STUD for valve selector signals!!
                firingRun++;
              }

              if (startMode == 1) {
                if (tube > 0 && tube < 4) {
                  for (cnt = 1; cnt < 4; cnt++) {
                    if (tube == cnt)
                      tubeIndi(cnt);
                  }
                }
                else if (tube == 4) {
                  for (cnt = 1; cnt < 4; cnt++)
                    tubeAll(cnt);
                }
              }
            }
          }
        }
      }
    }
  }
}

