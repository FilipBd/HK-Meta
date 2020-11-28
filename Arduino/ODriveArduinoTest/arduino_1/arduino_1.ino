// Arduino 1 code with functions:
// * 2 temp sensors controlling 2 fans
// * Emergency stop (relay): if no input from ethernet, no connection to Odrive or no sensor --> break
// * Odrive
// * Input from ethernet

//Libraries
#include <SPI.h>          // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>  // UDP library from: bjoern@cs.stanford.edu 12/30/2008
#include <HardwareSerial.h>
#include <SoftwareSerial.h>
#include <ODriveArduino.h>
// Printing with stream operator helper functions
template<class T> inline Print& operator <<(Print &obj,     T arg) { obj.print(arg);    return obj; }
template<>        inline Print& operator <<(Print &obj, float arg) { obj.print(arg, 4); return obj; }

// tempsensors
const int temp_1 = 0; // pin A0
const int temp_2 = 1; // pin A1
int16_t tempval_1 = 0;
int16_t tempval_2 = 0;

// Fans
const int fan_1 = 5; // pin D5
const int fan_2 = 6; // pin D6
const int small_fan = 11; // pin D11
const int big_fan = 3; // pin D3
int big_fan_val = 0;

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
  // emergency stop function
}

void tempsensors_func() {
  tempval_1 = analogRead(temp_1);
  tempval_1 = map(tempval_1, 687.3, 2839.70, -70, 500);
  tempval_2 = analogRead(temp_2);
  tempval_2 = map(tempval_1, 687.3, 2839.70, -70, 500);
  
  // control fan 1
  if (tempval_1 >= 20){
    analogWrite(fan_1, 63); // 25% duty cycle
    big_fan_val = 63;
  }
  else if (tempval_1 >= 40){
    analogWrite(fan_1, 127); // 50% duty cycle
    big_fan_val = 127;
  }
  else if (tempval_1 >= 60){
    analogWrite(fan_1, 190); // 90% duty cycle
    big_fan_val = 190;
  }
  else if (tempval_1 >= 80){
    analogWrite(fan_1, 255); // 100% duty cycle
    big_fan_val = 255;
  }
  else {
    analogWrite(fan_1, 0); // safe!
    big_fan_val = 0;
  }

  // control fan 2
  if (tempval_2 >= 20){
    analogWrite(fan_2, 63); // 25% duty cycle
    if (big_fan_val < 63){
      big_fan_val = 63;
    }
  }
  else if (tempval_2 >= 40){
    analogWrite(fan_2, 127); // 50% duty cycle
    if (big_fan_val < 127){
      big_fan_val = 127;
    }
  }
  else if (tempval_2 >= 60){
    analogWrite(fan_2, 190); // 90% duty cycle
    if (big_fan_val < 190){
      big_fan_val = 190;
    }
  }
  else if (tempval_2 >= 80){
    analogWrite(fan_2, 255); // 100% duty cycle
    if (big_fan_val < 255){
      big_fan_val = 255;
    }
  }
  else {
    analogWrite(fan_2, 0); // safe!
    if (big_fan_val < 63){
      big_fan_val = 0;
    }
  }
  analogWrite(big_fan, big_fan_val);
  
  delay(50);
}

float odrive_func(float pos, float vel, float torque){
  
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

int emergency_stop_func(){ //NEEDS WORK
  
  Serial.println("Emergency Stop");
  while(digitalRead(emergency_button) == HIGH){
    if (digitalRead(emergency_button) == LOW){
      //digitalWrite(emergency_button,LOW);
      emergency = false;
      break;
    }
    else emergency_stop_func();
    delay(1000);
   }
}

float DCS_func(){
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
    emergency_stop_func();
    }
  if(millis() - tim >= per){
    tim = millis();
    
    i = i + 1;
    odrive_func(i,0,0);
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
