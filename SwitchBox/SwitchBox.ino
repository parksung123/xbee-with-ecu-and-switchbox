m#include "SoftwareSerial.h"
#include <XBee.h>

uint8_t Rx = 12;
uint8_t Tx = 13;
SoftwareSerial mySerial(Rx, Tx);
XBee xbee = XBee();

uint8_t payload[5] = {0, 0, 0, 0, 0};
XBeeAddress64 addr64 = XBeeAddress64(0x0013A200, 0x4109A66B);
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));

uint8_t duration = 2;
uint8_t numCycle = 10;
uint8_t tubeSel[6] = {3, 4, 5, 6, 7}; // OFF: 2, ALL: 7
uint8_t modeSel[2] = {8, 9}; // TEST: 8, RUN: 9
uint8_t valveSel[6] = {14, 15, 16, 17, 18, 19};
uint8_t tubeStatus, valveStatus, modeStatus;
uint8_t cnt, tubeNum, modeNum, valveNum;

void setup() {
  mySerial.begin(9600);
  xbee.setSerial(mySerial);

  for (cnt = 0; cnt < 6; cnt++)
  {
    pinMode(tubeSel[cnt], INPUT_PULLUP);
    pinMode(valveSel[cnt], INPUT_PULLUP);
  }

  pinMode(modeSel[0], INPUT_PULLUP);
  pinMode(modeSel[1], INPUT_PULLUP);

}

void loop() {
  // checks which tube / valve the switch is set to
  for (tubeNum = 0; tubeNum < 6; tubeNum++)
  {
    tubeStatus = !digitalRead(tubeSel[tubeNum]);

    // if tube is opened, send info to other XBee
    if (tubeStatus == 1)
    {
      payload[0] = tubeNum & 0xff; // index of the pin
      payload[1] = duration & 0xff;
      payload[2] = numCycle & 0xff;
    }
    xbee.send(zbTx);
  }
  delay(500);

  for (valveNum = 0; valveNum < 6; valveNum++)
  {
    valveStatus = !digitalRead(valveSel[valveNum]);

    // if valve is opened, send info to other XBee
    if (valveStatus == 1)
      payload[3] = valveNum & 0xff;

    xbee.send(zbTx);
  }
  delay(500);
  // checks the mode selector
  for (modeNum = 0; modeNum < 2; modeNum++)
  {
    modeStatus = !digitalRead(modeSel[modeNum]);

    // if mode is opened, send info to other XBee
    if (modeStatus == 1)
      payload[4] = modeNum & 0xff;

    xbee.send(zbTx);
  }
  delay(500);
}
