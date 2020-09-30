

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

int DCS(){
    //Take values from dcs via udp
    Serial.print("DCS call workes");
}

int sensor_read(){
    //read all sensors, temp, encoder, accelerometer/gyro etc. (Odrive for encoder)
    Serial.print("sensor_read call workes");
}

int controll(){
    //take in dcs value, sensor and calc controll
    Serial.print("controll call workes");
}

int actuate(){
    //send values to Odrive
    Serial.print("actuate call workes");
}

void loop() {
  // here is where you'd put code that needs to be running all the time.

  // check to see if it's time to blink the LED; that is, if the difference
  // between the current time and last time you blinked the LED is bigger than
  // the interval at which you want to blink the LED.
  DCS();
  sensor_read();
  controll();
  actuate();
  
}