#include <SoftwareSerial.h>

#define pirSensor1  2
#define pirSensor2  3
#define pirSensor3  4
#define pirSensor4  5
#define buttonPin   6

int pirState1 = LOW;
int pirState2 = LOW;
int pirState3 = LOW;
int pirState4 = LOW;
int buttonState = LOW;

SoftwareSerial sim800l(7, 8);

const char apn[] = "your_apn";
const char apnUser[] = "";
const char apnPass[] = "";

const char server[] = "your.server.com";
const char path[] = "/update";


void setup() {
  Serial.begin(9600);

  pinMode(pirSensor1, INPUT);
  pinMode(pirSensor2, INPUT);
  pinMode(pirSensor3, INPUT);
  pinMode(pirSensor4, INPUT);

  pinMode(buttonPin, INPUT_PULLUP);
  initializeSIM800L();
}

void loop() {
  pirState1 = digitalRead(pirSensor1);
  pirState2 = digitalRead(pirSensor2);
  pirState3 = digitalRead(pirSensor3);
  pirState4 = digitalRead(pirSensor4);

  buttonState = digitalRead(buttonPin);

  sendStatesToServer();

  delay(10000);

}

void initializeSIM800L() {
  // Wait for SIM800L to be ready
  delay(1000);
  sim800l.println("AT");
  waitForResponse("OK");

  // Set up GPRS
  sim800l.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
  waitForResponse("OK");
  sim800l.print("AT+SAPBR=3,1,\"APN\",\"");
  sim800l.print(apn);
  sim800l.println("\"");
  waitForResponse("OK");

  if (strlen(apnUser) > 0 && strlen(apnPass) > 0) {
    sim800l.print("AT+SAPBR=3,1,\"USER\",\"");
    sim800l.print(apnUser);
    sim800l.println("\"");
    waitForResponse("OK");
    sim800l.print("AT+SAPBR=3,1,\"PWD\",\"");
    sim800l.print(apnPass);
    sim800l.println("\"");
    waitForResponse("OK");
  }

  sim800l.println("AT+SAPBR=1,1");
  waitForResponse("OK");
  sim800l.println("AT+SAPBR=2,1");
  waitForResponse("OK");
}

void sendStatesToServer() {
  String data = "pir1=" + String(pirState1) + "&pir2=" + String(pirState2) + "&pir3=" + String(pirState3) + "&pir4=" + String(pirState4) + "&button=" + String(buttonState);

  sim800l.println("AT+HTTPINIT");
  waitForResponse("OK");

  sim800l.println("AT+HTTPPARA=\"CID\",1");
  waitForResponse("OK");
  sim800l.print("AT+HTTPPARA=\"URL\",\"");
  sim800l.print(server);
  sim800l.print(path);
  sim800l.println("\"");
  waitForResponse("OK");

  sim800l.print("AT+HTTPDATA=");
  sim800l.print(data.length());
  sim800l.println(",10000");
  waitForResponse("DOWNLOAD");
  sim800l.println(data);
  waitForResponse("OK");

  sim800l.println("AT+HTTPACTION=1");
  waitForResponse("200");

  sim800l.println("AT+HTTPREAD");
  waitForResponse("OK");

  sim800l.println("AT+HTTPTERM");
  waitForResponse("OK");
}

void waitForResponse(String expected) {
  while (true) {
    if (sim800l.available()) {
      String response = sim800l.readString();
      if (response.indexOf(expected) != -1) {
        break;
      }
    }
  }
}
