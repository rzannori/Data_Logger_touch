/*PIN out
 * GPS : RX=D6, TX = D7 
 * Pulsante touch : D0
 * SD : MISO = D12,MOSI=D11,SCK=D13, CS= D5
 * OLED; SCL=A5,SDA=A4 
 */

#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include<SD.h>

#define I2C_ADDRESS 0x3C
#define RST_PIN -1 // RST_PIN se necessario.
const int cs_sd=3; // pin di CS della SD
static const int RXPin = 6, TXPin = 7; //comunicazione GPS
static const uint32_t GPSBaud = 9600;

const int touchPin = 0;// input pin pulsante touch
const int touchDelay = 100;//millisecond di ritardo tra tocchi
int count=0;// variabile counter
int Bstate;// stato pulsante

int mode;
int label;
int DatiVoloArray[] ={0,0,0}; // array dove registro i valori massimi di: 1 vmax, 2 hmax, 3 satelliti
int maxspeed, maxalt, maxsatellite;
int SatH, SpeedH, AltH;

TinyGPSPlus gps;// oggetto TinyGPSPlus 
SoftwareSerial ss(RXPin, TXPin);// connessione seriale al GPS

SSD1306AsciiWire oled;
//------------------------------------------------------------------------------
void setup() {
 Serial.begin(115200);
 Wire.begin();
 Wire.setClock(400000L);
 ss.begin(GPSBaud);
 pinMode(touchPin,INPUT);// pin pulsante e condizione
  
 #if RST_PIN >= 0
  oled.begin(&Adafruit128x32, I2C_ADDRESS, RST_PIN);
 #else // RST_PIN >= 0
  oled.begin(&Adafruit128x32, I2C_ADDRESS);
 #endif // RST_PIN >= 0

  oled.setFont(System5x7);
  IntroDisplay();
  while ( !SD.begin(cs_sd)){
     sdFailDisplay();
   }
  sdAvailDisplay(); 
  File data = SD.open("DatiVolo.txt",FILE_WRITE);              // crea cartella dati volo nella sd
  data.println("");
  data.println("Dati Registrati");    
  data.close(); 
  sysReadyDisplay();
  }
//------------------------------------------------------------------------------
void loop() {
  //costruisco e aggiorno array con valori massimi
 int speed1 = (gps.speed.kmph());
  if ( speed1 > maxspeed) {
    maxspeed = speed1;
    DatiVoloArray[1] = maxspeed;
    }
    SpeedH = DatiVoloArray[1];
      
 int alt1 = (gps.altitude.meters());
  if ( alt1 > maxalt) {
    maxalt=alt1;
    DatiVoloArray[2]= maxalt;
  }
  AltH=DatiVoloArray[2];

 int satellite = (gps.satellites.value());
  if ( satellite > maxsatellite) {
     maxsatellite = satellite;
     DatiVoloArray[3] = maxsatellite;
    } 
    SatH=DatiVoloArray[3];
    
if (satellite > 0){ 
    String Time=String(gps.time.hour()+1)+(":")+(gps.time.minute())+(":")+(gps.time.second());
    String Date=String(gps.date.day())+("/")+(gps.date.month())+("/")+(gps.date.year());
    
 int touchValue = digitalRead(touchPin);//verifico stato del pulsante
   if(touchValue == HIGH)  {
   count++;
   if (count < 4)
    {
     Bstate=count;
     }
   else 
    {
    count = 0;
    Bstate=count;
    }
    oled.clear ();
   }
   
 switch (Bstate) { 
    case 1:  {       
      // scrivo su sd
      File data=SD.open("DatiVolo",FILE_WRITE);
      data.println(Date + " , " + Time + " , " + String(gps.location.lat(), 6)+ " , " + String(gps.location.lng(), 6)+ " , " + String(gps.altitude.meters(),0)+ " , " + String(gps.speed.kmph(),0) + " , " + String(gps.satellites.value())); 
      data.close();
	//   oled.setScrollMode(SCROLL_MODE_AUTO);
	   oled.clear();
	   oled.set2X();
     oled.setCursor(30 ,0);
     oled.println("Write");
     oled.println();
     oled.setCursor(30 ,50);
     oled.println("on SD");
      }
      break;
    case 2:{  
     // leggo da sd
     oled.set1X();
     oled.setCursor(0,0);
     oled.print(F("V max"));
     oled.setCursor(50 ,0);
     oled.print(F("H max"));
     oled.setCursor(100 ,0);
     oled.print(F("Sat"));
     oled.println();
     oled.println();
     oled.setCursor(0,50);
     oled.print(F("   "));
     oled.setCursor(0,50);
     oled.print(SpeedH);
     oled.setCursor(50,50);
     oled.print(F("   "));
     oled.setCursor(50 , 50);
     oled.print(AltH);
     oled.setCursor(100,50);
     oled.print(F("  "));
     oled.setCursor(100 , 50);
     oled.print(SatH);
     }
      break;
    case 3:{       
      // menu satelliti
     oled.set1X();
     oled.setCursor(0,0);
     oled.print(Date);
     oled.setCursor(60,100);
     oled.print(Time);
     oled.println();
     oled.println();
     oled.setCursor(20,50);
     oled.set2X();
     oled.print(F("Sat ="));
     oled.print(F(" "));
     oled.print(satellite);
      }
      break;
    default:{
      oled.set1X();
      oled.setCursor(0,0);
      oled.print(Date);
     oled.setCursor(60,100);
     oled.print(Time);
     oled.println();
     oled.println(F(" 1 = Write"));
     oled.println(F(" 2 = Read"));
     oled.println(F(" 3 =Status"));
      }
      break; 
    }
   } 
   else {
  waitSatDisplay();
  }
  DelayGPS(500);
   
  }
 
 //==========================================sub routine=====================================
  static void DelayGPS(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}
 //==========================================IntroDiplay=====================================
void IntroDisplay(){
  oled.clear();
  oled.set2X();
  oled.setCursor(30 ,0);
  oled.println(F("RZ DATA"));
  oled.println();
  oled.setCursor(30 ,50);
  oled.println(F("LOGGER"));
  delay(4000);
  oled.clear();
}
 //==========================================sdFailDisplay=====================================
void sdFailDisplay(){
     oled.set2X();
     oled.setCursor(20 ,0);
     oled.print(F("SD Card"));
     oled.println();
     oled.println();
     oled.setCursor(20,50);
     oled.print(F("Failed"));
     delay(2000);
     oled.clear();
     }
//==========================================sdAvailDisplay=====================================
void  sdAvailDisplay() {
     oled.set2X();
     oled.setCursor(20 ,0);
     oled.print(F("SD Card"));
     oled.println();
     oled.println();
     oled.setCursor(10,50);
     oled.print(F("available"));
     delay(2000);
     oled.clear();
 } 

//==========================================sysReadyDisplay=====================================
 void sysReadyDisplay(){
  oled.println(" "); 
  oled.set2X();
  oled.setCursor(30 ,0);
  oled.println("System");
  oled.println();
  oled.setCursor(40 ,50);
  oled.println("ready");
  delay (2000);
  oled.clear();
  } 

//==========================================waitSatDisplay=====================================
void waitSatDisplay(){
  oled.println(" "); 
  oled.set2X();
  oled.setCursor(30 ,0);
  oled.println("Waiting");
  oled.setCursor(50 ,30);
  oled.println("for");
  oled.setCursor(20 ,60);
  oled.println("Satellite");
  delay (100);
  oled.clear();
  }
    
