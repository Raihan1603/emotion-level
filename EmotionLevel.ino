
#define BLYNK_TEMPLATE_ID "TMPLgrNryjGU" //Template ID dan Device Name
#define BLYNK_DEVICE_NAME "Emotion Level"

#define BLYNK_FIRMWARE_VERSION        "0.1.0" //Versi Blynk

#define BLYNK_PRINT Serial //Output Data Blynk
//#define BLYNK_DEBUG

#define APP_DEBUG //Library Blynk
#define USE_NODE_MCU_BOARD //Board NodeMCU
#include "BlynkEdgent.h"
#define USE_ARDUINO_INTERRUPTS true //Pulse Sensor  

#define relay D0 //Pin D0 Relay
#define suara D5 //Pin D5 Sensor Suara

#include <PulseSensorPlayground.h> //Library Pulse

const int PulseWire = 0;   
int Signal;     //Sinyal value 0-1024
int Threshold = 550; //Threshold Default

PulseSensorPlayground pulseSensor;

const int sampleTime = 200; //sampleTime = Delay
unsigned int sample;

BlynkTimer timer; //Inisialisasi Blynk secara Real Time

BLYNK_WRITE(V0){ //31-39 Kirim Data dari Blynk ke NodeMCU
  if(param.asInt() == HIGH){
    digitalWrite(relay, LOW);
    Serial.write("0");
  } else {
    digitalWrite(relay, HIGH);
    Serial.write("1");
  }
}

WidgetLCD lcd(V3); //Widget LCD Blynk

void myTimerEvent(){
  //Sensor Detak Jantung
  Signal = analogRead(PulseWire);  //Baca Pulse Sensor value.
  Serial.println(Signal);                    //Mengirim Sinyal ke Serial Plotter.
  
  delay(10);
  //Sensor Suara
  unsigned long millisStart = millis();
  float peakToPeak = 0;

  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;

  while(millis() - millisStart < sampleTime){
    sample = analogRead(suara);
    if(sample < 1024){
      if(sample > signalMax){
        signalMax = sample;
      }
      else if(sample < signalMin){
        signalMin = sample;
      }
    }
  }

  peakToPeak = signalMax - signalMin;
  int db = map(peakToPeak, 30, 900, 50, 90);

  Serial.print("dB:"); //Output Serial
  Serial.println(db);
  
  Blynk.virtualWrite(V2, db); //Output Blynk
  Blynk.virtualWrite(V1, Signal); //Output Blynk
  
  if(db <= 60 || Signal <= 850 ){
    //lcd.setCursor(2,1);
    Serial.println("Emosi:Normal");
    lcd.print(0, 0, "Deteksi Emosi");
    lcd.print(0, 1, "Emosi:Normal");
    //digitalWrite(relay, HIGH);
  }
  else if(db >= 60 && db <= 85 || Signal >= 850 && Signal <= 950){
    //lcd.setCursor(2,1);
    Serial.println("Emosi:Sedang");
    lcd.print(0, 0, "Deteksi Emosi");
    lcd.print(0, 1, "Emosi:Sedang");
  }
  else if(db >= 85 || Signal >= 950){
    //lcd.setCursor(2,1);
    Serial.println("Emosi:Tinggi");
    lcd.print(0, 0, "Deteksi Emosi");
    lcd.print(0, 1, "Emosi:Tinggi");
    digitalWrite(relay, HIGH);
  }
  delay(500);
}

void setup()
{
  Serial.begin(115200);
  delay(100);
  
  pinMode(suara, INPUT);
  pinMode(relay, OUTPUT);
  BlynkEdgent.begin(); //Menjalankan Blynk
  timer.setInterval(1000L, myTimerEvent); //Menjalankan Blynk
}

void loop() {
  BlynkEdgent.run(); //Looping
  timer.run(); //Looping
}
