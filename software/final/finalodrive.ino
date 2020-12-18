// includes
#include <HardwareSerial.h>
#include <SoftwareSerial.h>
#include <ODriveArduino.h>


#include <SPI.h>          // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>  // UDP library from: bjoern@cs.stanford.edu 12/30/2008
// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {  
  0xA8, 0x61, 0x0A, 0xAE, 0x85, 0x83 };
IPAddress ip(169,254,170,171);

unsigned int localPort = 8888;              // local port to listen on

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  //buffer to hold incoming packet,
char  ReplyBuffer[] = "acknowledged";       // a string to send back

int tim;
// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

int fan_one = 3; 
int fan_two = 5;
int fan_three = 6;
//int fan_four = 10;//fan pin 3 5 6 10

double pos_prev = 0;
double offset = 0;
// Printing with stream operator helper functions
template<class T> inline Print& operator <<(Print &obj,     T arg) { obj.print(arg);    return obj; }
template<>        inline Print& operator <<(Print &obj, float arg) { obj.print(arg, 4); return obj; }


////////////////////////////////
// Set up serial pins to the ODrive
////////////////////////////////


// Arduino without spare serial ports (such as Arduino UNO) have to use software serial.
// Note that this is implemented poorly and can lead to wrong data sent or read.
// pin 8: RX - connect to ODrive TX
// pin 9: TX - connect to ODrive RX
SoftwareSerial odrive_serial(8, 9);


// ODrive object
ODriveArduino odrive(odrive_serial);

void fan()
{
  analogWrite(fan_one,255);
  analogWrite(fan_two,255);
  analogWrite(fan_three,255);
  //analogWrite(fan_four,127);   
  return;
}

void setup() {
  pinMode(fan_one,OUTPUT);
  pinMode(fan_two,OUTPUT);
  pinMode(fan_three,OUTPUT);
  //pinMode(fan_four,OUTPUT);

  fan();
  Ethernet.begin(mac,ip);
  Udp.begin(localPort);
  
  // ODrive uses 115200 baud
  odrive_serial.begin(115200);

  // Serial to PC
  Serial.begin(115200);
  while (!Serial) ; // wait for Arduino Serial Monitor to open

  Serial.println("ODriveArduino");
  Serial.println("Setting parameters...");

  // In this example we set the same parameters to both motors.
  // You can of course set them different if you want.
  // See the documentation or play around in odrivetool to see the available parameters
  for (int axis = 0; axis < 2; ++axis) {
    odrive_serial << "w axis" << axis << ".controller.config.vel_limit " << 20.0f << '\n';
    odrive_serial << "w axis" << axis << ".motor.config.current_lim " << 40.0f << '\n';
    // This ends up writing something like "w axis0.motor.config.current_lim 10.0\n"
  }
  delay(2000);
  Serial.println("Start to calibrate!");
  int m1 = '0';
  int m2 = '1';
  int requested_state;
  requested_state = ODriveArduino::AXIS_STATE_CLOSED_LOOP_CONTROL;
  Serial << "Axis" << m1 << ": Requesting state " << requested_state << '\n';
  Serial << "Axis" << m2 << ": Requesting state " << requested_state << '\n';
  if(!odrive.run_state(m1, requested_state, false )) {
    Serial.println("fail to calibrate m0");
    //stop();
  }
  if(!odrive.run_state(m2, requested_state, false )) {
    Serial.println("fail to calibrate m1");
    //stop();
  }
  
  odrive.SetPosition(0,0); //0:35,1:31.5
  odrive.SetPosition(1,0);
  Serial.println("Send the character '0' or '1' to calibrate respective motor (you must do this before you can command movement)");
  Serial.println("Send the character 's' to exectue test move");
  Serial.println("Send the character 'b' to read bus voltage");
  Serial.println("Send the character 'p' to read motor positions in a 10s loop");
  delay(5000);
}

void stop()
{
 while(1);
}



double getudp(){
  int packetSize = Udp.parsePacket();
  Serial.println(packetSize);
  if(packetSize)
  {
    // read the packet into packetBufffer
    Udp.read(packetBuffer,UDP_TX_PACKET_MAX_SIZE);
    double pos = atof(packetBuffer);
    Serial.println(packetBuffer);
    for(int i=0;i<UDP_TX_PACKET_MAX_SIZE;i++) packetBuffer[i] = 0;
    
    return pos;
    //Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    //Udp.write(millis());
    //Udp.endPacket();
    
    // send a reply, to the IP address and port that sent us the packet we received
    /*Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(ReplyBuffer);
    Udp.endPacket();*/
  }

}

void loop() {


  // Sinusoidal test move
   //Serial.println("Executing test move");
  /*for (double ph = 0.0f; ph < 6.28318530718f; ph += 0.01f) {
    //double pos_m0 = 5.0f * cos(ph);
    //double pos_m1 = 5.0f * sin(ph);*/
    //Serial.print(millis());
    //Serial.println(" Arduino is working");
    int packetSize = Udp.parsePacket();
    double pos;
    double diff_factor = 1/0.98; //differential factor
    
    if(packetSize)
    {
    // read the packet into packetBufffer
      Udp.read(packetBuffer,UDP_TX_PACKET_MAX_SIZE);
      pos = atof(packetBuffer) + offset;
      Serial.println(packetBuffer);
      double difference = pos - pos_prev; //differential of the two motor
      if (difference > 0){ //going to the front
        odrive.SetPosition(0, pos_prev + difference * diff_factor); 
        odrive.SetPosition(1, pos_prev + difference);
      } else if (difference < 0){//going to the back
        odrive.SetPosition(0, pos_prev + difference);
        odrive.SetPosition(1, pos_prev + difference * diff_factor);
      } else if (difference == 0){
        odrive.SetPosition(0, pos);
        odrive.SetPosition(1, pos);
      }
      
      for(int i=0;i<UDP_TX_PACKET_MAX_SIZE;i++) packetBuffer[i] = 0;
    }
    //odrive.SetPosition(0, 0);
    //odrive.SetPosition(1, 0);
    pos_prev = pos;
    //odrive.SetPosition(1, pos_m1);
    //delay(5);
  }
