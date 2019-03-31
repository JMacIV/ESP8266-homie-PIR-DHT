#include <Arduino.h>
#include <Homie.h>
#include <DHT.h>

//Firmware name and version that is sent to Homie
#define FW_NAME "dht-pir-esp-h301"
#define FW_VERSION  "1.1"

//DHT Section
//Pin that the DHT sensor is connected to on ESP chip. Change DHTTYPE to DHT11 if appropriate
#define DHTPIN 4
#define DHTTYPE DHT22

const int TEMPERATURE_INTERVAL = 60000; //In millis. Change to increase or decrease temp/humidity intervals
unsigned long last_temperature_sent = 0;

//PIR
int pirPin = 12;    // the digital pin connected to the PIR sensor's output
int ledPin = 16;    // the digital pin connected to built-in LED

//the time we give the PIR sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 30;        

//the time when the sensor outputs a low impulse
long unsigned int lowIn;         

//the amount of milliseconds the sensor has to be low 
//before we assume all motion has stopped
long unsigned int pause = 5000;  

// Start a counter for serial logging and set the initial value to no motion 
int counter = 0;
int previousReading = LOW;

boolean lockLow = true;
boolean takeLowTime;  

DHT dht(DHTPIN, DHTTYPE);

float humidity, temp;

HomieNode environmentNode("environment", "Environment", "environment");

void setupHandler() {
  // Do what you want to prepare your sensor
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(pirPin, HIGH);
  dht.begin();

  //give the sensor some time to calibrate
  Homie.getLogger().print("calibrating sensor ");
      for(int i = 0; i < calibrationTime; i++){
      Homie.getLogger().print(".");
      delay(500);
      }
  Homie.getLogger().print(" done");
  Homie.getLogger().print("SENSOR ACTIVE");
  delay(50);
}

void gettemphumid() {
  unsigned long currentMillis = millis(); // Wait 10 seconds between measurements
  if(currentMillis - last_temperature_sent >= TEMPERATURE_INTERVAL){
    last_temperature_sent = currentMillis;
  temp = dht.readTemperature(true);
  humidity = dht.readHumidity();
  if (isnan(temp) || isnan(humidity)){
    Homie.getLogger().print("Failed to read from DHT sensor!");
  }
  Homie.getLogger() << "Temperature: " << temp << " °F" << endl;
  Homie.getLogger() << "Humidity: " << humidity << " %" << endl;
  environmentNode.setProperty("temperature").send(String(temp));
  environmentNode.setProperty("humidity").send(String(humidity));
  }
}

void getMotion()
{
  if (digitalRead(pirPin) == HIGH)
  {
    digitalWrite(ledPin, LOW); //the led visualizes the sensors output pin state
    if (lockLow)
    {
      environmentNode.setProperty("motion").send("true");
      //makes sure we wait for a transition to LOW before any further output is made:
      lockLow = false;
      Homie.getLogger().print("---");
      Homie.getLogger() << "motion detected at " << millis() / 1000 << " sec" <<endl;
      delay(50);
    }
    takeLowTime = true;
  }

  if (digitalRead(pirPin) == LOW)
  {
    digitalWrite(ledPin, HIGH); //the led visualizes the sensors output pin state

    if (takeLowTime)
    {
      lowIn = millis();    //save the time of the transition from high to LOW
      takeLowTime = false; //make sure this is only done at the start of a LOW phase
    }
    //if the sensor is low for more than the given pause,
    //we assume that no more motion is going to happen
    if (!lockLow && millis() - lowIn > pause)
    {
      environmentNode.setProperty("motion").send("false");
      //makes sure this block of code is only executed again after
      //a new motion sequence has been detected
      lockLow = true;
      Homie.getLogger() << "motion ended at " << (millis() - pause) / 1000 << " sec" <<endl;
      delay(50);
    }
  }
}

void loopHandler() {
  getMotion();
  gettemphumid();
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial << endl << endl;
  Homie_setFirmware(FW_NAME, FW_VERSION);
  Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler);

  environmentNode.advertise("motion").setName("Motion")
                                  .setRetained("false")
                                  .setDatatype("boolean");                                  
  environmentNode.advertise("temperature").setName("Temperature")                                     
                                      .setUnit("ºF")
                                      .setDatatype("float");
  environmentNode.advertise("humidity").setName("Humidity")
                                   .setUnit("%")
                                   .setDatatype("float");


  Homie.setup();
}

void loop() {
  Homie.loop();
}