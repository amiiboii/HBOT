#include <Ethernet.h>
#include <EthernetUdp.h>
#include "ClearCore.h"
#include "string_catch.h"
#include "move_stepper.h"

#define ARRAY_SIZE 2
double myangle[ARRAY_SIZE];
double angle;

int j, i = 0;

#define UDP_TX_PACKET_MAX_SIZE 38

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(169 , 254, 88, 53);

//EthernetServer server(80);
unsigned int localPort = 8888;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];
int packetSize;
EthernetUDP Udp;

double gear_ratio = 3600;

float Velocity = 12; //25 max -> 1500rpm
float Acceleration = 10;

double pre_angle = 0;

unsigned long startTime; // variable to store the start time

unsigned long elapsedTime_pre = 0;

char cstr[16];
int time_delay;

unsigned long previousMillis = 0;
const long interval = 500;

#define motor ConnectorM0
#define motor2 ConnectorM1

// Select the baud rate to match the target serial device
#define baudRate 9600

// Define the velocity and acceleration limits to be used for each move
int velocityLimit = 10000; // pulses per sec
int accelerationLimit = 100000; // pulses per sec^2

void setup()
{

  ConnectorM0.EnableRequest(true);
  ConnectorM1.EnableRequest(true);
  Serial.begin(2000000);
  Ethernet.begin(mac, ip);
  Udp.begin(localPort);

  delay(1500);

  // connect and configure the stepper motors to their IO pins

  stepperX.connectToPins(MOTOR_X_STEP_PIN, MOTOR_X_DIRECTION_PIN);
  stepperY.connectToPins(MOTOR_Y_STEP_PIN, MOTOR_Y_DIRECTION_PIN);

  Serial.println();
  strData = NULL;                           //make sure it is NULL to start

  Serial.print("Ethernet Connected");

  startTime = millis(); // store the start time

}

void loop()
{
  unsigned long elapsedTime = millis() - startTime; // calculate the elapsed time
  packetSize = Udp.parsePacket();

  unsigned long currentTime = millis();

  if (packetSize > 0) {

    Serial.println(" \n");
    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    String datReq(packetBuffer);
    //Serial.println(datReq);
    //Serial.println(" \n");

    // if (datReq =="Blue") {                               //See if blue was requested
    //Serial.print("data requested...");
    //Serial.print(Udp.remoteIP());
    //Serial.print(Udp.remotePort());
    //Serial.println(" \n");

    String str, a1, b1, c1, F1, A1, MA, MA_T;
    int N = 0;
    str = datReq;
    Serial.println(str);
    N = separate (str, sPtr, SPTR_SIZE,  &strData);

    a1 = sPtr[0];
    a1.remove(0, 1); //Remove 1 characters starting from position 0

    b1 = sPtr[1];
    b1.remove(0, 1); //Remove 1 characters starting from position 0

//    c1 = sPtr[2];
//    c1.remove(0, 1); //Remove 1 characters starting from position 0

    F1 = sPtr[2];
    F1.remove(0, 1); //Remove 1 characters starting from position 0

    A1 = sPtr[3];
    A1.remove(0, 1); //Remove 1 characters starting from position 0

    MA = sPtr[0];
    MA_T = sPtr[1];

    if (MA == "AF") {
      Serial.println(MA_T.toDouble());
      MA_T.toDouble();
      Serial.println("DATA :");
      //delay(15);
      myangle[1] = MA_T.toDouble();
      j++;
      if (j >= ARRAY_SIZE) {
        j = 0;
      }

//      if (currentTime - startTime >= map(myangle[1] , 0 , 180 , 0 , 1200)) {
//        startTime = currentTime;
//        myservo.write(myangle[1]); // move the servo to the new position
//        Serial.print(" angle Loop : ");
//        Serial.print(myangle[1]);
//        i++;
//        if (i >= ARRAY_SIZE) {
//          i = 0;
//        }
//        Serial.println();
//      }

    } else {

      double DK_a = a1.toDouble();
      double DK_b = b1.toDouble();
      double DK_c = 360;
      double FeedRate = F1.toDouble();
      double AccelerationRate = A1.toDouble();

      // GOTO Position XYZ
      //  Move robot motors to DK.a, DK.b and DK.c CODE

//      Commands.comment('a', DK_a);
//      Commands.comment('b', DK_b);
//      Commands.comment('c', DK_c);
//      Commands.comment('F', FeedRate);
//      Commands.comment('A', AccelerationRate);

      if (F1 != NULL) {
        Velocity = FeedRate;
        Serial.println(Velocity);
      }

      if (A1 != NULL) {
        Acceleration = AccelerationRate;
        Serial.println(AccelerationRate);
      }

      if (moveXYWithCoordination(abs_value_x(DK_a * gear_ratio / 360), abs_value_y(DK_b * gear_ratio / 360), abs_value_z(DK_c * gear_ratio / 360 ), Velocity * 3600, Acceleration * 3600) == true) {

        Serial.println("");
        time_delay = elapsedTime - elapsedTime_pre;
        Serial.println(time_delay);
        elapsedTime_pre = elapsedTime;
        itoa(time_delay, cstr, 10);

        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort()); //Initialize packet send
        Udp.print(cstr); //Send the timedate data
        Udp.endPacket(); //End the packet
      }

    }
    //per Second round -> 60rpm -> 6400
    //RPM -> 1rpm -> 6400/60 -> 106.666667

    //600rpm -> original rate
    

  }
  memset(packetBuffer, 0, UDP_TX_PACKET_MAX_SIZE);
}
