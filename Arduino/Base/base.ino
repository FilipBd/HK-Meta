//Libraries
#include <SPI.h>          // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>  // UDP library from: bjoern@cs.stanford.edu 12/30/2008
#include "BMI088.h"
#include <HardwareSerial.h>
#include <SoftwareSerial.h>
#include <ODriveArduino.h>
// Printing with stream operator helper functions
template<class T> inline Print& operator <<(Print &obj,     T arg) { obj.print(arg);    return obj; }
template<>        inline Print& operator <<(Print &obj, float arg) { obj.print(arg, 4); return obj; }


//Initialize variables/Ip/Mac etc.
//Variables-----------------------------
float G_force;
int led_pin = A0;
bool emergency = false;
int emergency_button = 7;
SoftwareSerial odrive_serial(8, 9);


//ODRIVE---------------------------------
ODriveArduino odrive(odrive_serial);



//ETHERNET-------------------------------
byte mac[] = {  
  0xA8, 0x61, 0x0A, 0xAE, 0x85, 0x83 };
  //0xA8, 0x61, 0x0A, 0xAE, 0x85, 0xE8};
IPAddress ip(169,254,212,11);

unsigned int localPort = 8888;              // local port to listen on

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  //buffer to hold incoming packet,
char  ReplyBuffer[] = "acknowledged";       // a string to send back

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;
//---------------------------------------

//accelerometer/gyro/temp----------------
float ax = 0, ay = 0, az = 0;
float gx = 0, gy = 0, gz = 0;
int16_t temp = 0;
//---------------------------------------



void setup() {
  // initialize serial communication at 9600 bits per second:

  pinMode(led_pin, OUTPUT);
  pinMode(emergency_button,INPUT);
  //digitalWrite(emergency_button,LOW);

  odrive_serial.begin(115200);
  Serial.println("ODriveArduino");
  Serial.println("Setting parameters...");


  for (int axis = 0; axis < 2; ++axis) {
    odrive_serial << "w axis" << axis << ".controller.config.vel_limit " << 5.0f << '\n';
    odrive_serial << "w axis" << axis << ".motor.config.current_lim " << 10.0f << '\n';
    // This ends up writing something like "w axis0.motor.config.current_lim 10.0\n"
  }

  for (int motor = 0; motor<2;++motor){
    odrive_serial <<"w axis"<< motor << ".requested_state = AXIS_STATE_CLOSED_LOOP_CONTROL\n";
  }

 
  
  
  Ethernet.begin(mac,ip);
  Udp.begin(localPort);


    
  Serial.begin(115200);

  /*Wire.begin();
  while(!Serial);
    Serial.println("BMI088 Raw Data");

  while(1)
    {
        if(bmi088.isConnection())
        {
            bmi088.initialize();
            Serial.println("BMI088 is connected");
            break;
        }
        else Serial.println("BMI088 is not connected");
 
        delay(2000);
    }*/
}

float ODRIVE(float pos, float vel, float torque){
  
  odrive.SetPosition(0,pos);  //Pos of motor 0
  odrive.SetPosition(1,pos);  //Pos of motor 1

  odrive_serial << "r vbus_voltage\n";
  Serial << "Vbus voltage: " << odrive.readFloat() << '\n';

  /*static const unsigned long duration = 10000;
  unsigned long start = millis();
  while(millis() - start < duration) {
    for (int motor = 0; motor < 2; ++motor) {
      odrive_serial << "r axis" << motor << ".fet_thermistor.temperature\n";
      Serial << odrive.readFloat() << '\t';
    }
    Serial << '\n';
  }
  
  while(millis() - start < duration) {
    for (int motor = 0; motor < 2; ++motor) {
      odrive_serial << "r axis" << motor << ".encoder.pos_estimate\n";
      Serial << odrive.readFloat() << '\t';
    }
    Serial << '\n';
  }*/ 
}
int Emergency_Stopp(){ //NEEDS WORK
  
  Serial.println("Emergency Stopp");
  while(digitalRead(emergency_button) == HIGH){
    if (digitalRead(emergency_button) == LOW){
      //digitalWrite(emergency_button,LOW);
      emergency = false;
      break;
    }
    else Emergency_Stopp();
    delay(1000);
   }
}

float DCS(){
  //Take values from dcs via udp
  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if(packetSize)
  {
    /*Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remote = Udp.remoteIP();
    for (int i =0; i < 4; i++)
    {
      Serial.print(remote[i], DEC);
      if (i < 3)
      {
        Serial.print(".");
      }
    }
    Serial.print(", port ");
    Serial.println(Udp.remotePort());
    */
    // read the packet into packetBufffer
    Udp.read(packetBuffer,UDP_TX_PACKET_MAX_SIZE);
    Serial.print("Contents:");
    Serial.println(packetBuffer);
    float G_force_local = atol(packetBuffer);
    
    for(int i=0;i<UDP_TX_PACKET_MAX_SIZE;i++) packetBuffer[i] = 0;

    // send a reply, to the IP address and port that sent us the packet we received
    /*Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(ReplyBuffer);
    Udp.endPacket();*/
    return G_force_local/1000;
  }
  //delay(10);
}

float sensor_read(){
    //read all sensors, temp, encoder, accelerometer/gyro etc. (Odrive for encoder)
    //Serial.println("sensor_read call workes");
    bmi088.getAcceleration(&ax, &ay, &az);
    bmi088.getGyroscope(&gx, &gy, &gz);
    temp = bmi088.getTemperature();

    ax = ax/(9.82*100);
    ay = ay/(9.82*100);
    az = az/(9.82*100);

    Serial.print(ax); //N
    Serial.print(",");
    Serial.print(ay); //N
    Serial.print(",");
    Serial.print(az); //N
    Serial.print(",");
    return ax, ay, az;
}


void controller(float DCS_value, float sensor_value){
    //take in dcs value, sensor and calc control
    //Serial.println("controll call workes");

    float val = (DCS_value - sensor_value)*127;
    if (val >= 255){
      val = 255;
      }
    if (val <= 0){
      val = 0;
      }
    Serial.print("val =");
    Serial.println(val);

    return;
}
int i;
unsigned long tim = millis();
void loop() {
  int per = 1000;
  
  
  if (digitalRead(emergency_button) == HIGH){
    //digitalWrite(emergency_button,HIGH);
    emergency == true;
  }
  while(digitalRead(emergency_button) == HIGH){
    Emergency_Stopp();
    }
  if(millis() - tim >= per){
    tim = millis();
    
    i = i + 1;
    ODRIVE(i,0,0);
    Serial.println(tim);
    Serial.println(i);
    }

  /*G_force = DCS();
  Serial.print("G_force =");
  Serial.println(G_force);
  //ax, ay, az = sensor_read();
  */
  //controller(G_force, ax);
}
