#include <LowPower.h>
#include <Stepper.h>
#include <RTClib.h>

RTC_DS3231 rtc;        // de RTC module maakt gebruik van een DS3231 IC

#define OMWENTELING 2038  // aantal stappen dat de uitgaande as eenmaal rond laat draaien

#define HALL_dagNummer 4  // op print: HALL_0, U2, RV2
#define HALL_maandNaam 3  // op print: HALL_1, U1, RV1

const int COUNTERCLOCKWISE = -1;

// stepper sequence = oranje, paars, geel, blauw
Stepper dagNummer = Stepper(OMWENTELING, 12, 10, 11, 9);  // STEPPER 0
Stepper maandNaam = Stepper(OMWENTELING, 5, 7, 6, 8);   // STEPPER_1

// struct voor het bijhouden van huidige en komende dag
typedef struct {
  uint8_t Nummer;
  uint8_t Maand;
  uint8_t volgendNummer;
  uint8_t volgendMaand;
} Datum;

  Datum datum;

void schuifDag(){
  while(digitalRead(HALL_dagNummer)){
    dagNummer.step(COUNTERCLOCKWISE);}
  while(!digitalRead(HALL_dagNummer)){
    dagNummer.step(COUNTERCLOCKWISE);}
}

void schuifMaand(){
  while(digitalRead(HALL_maandNaam)){
    maandNaam.step(COUNTERCLOCKWISE);}
  while(!digitalRead(HALL_maandNaam)){
    maandNaam.step(COUNTERCLOCKWISE);}
}

void setup() {
  rtc.begin();
  
// Uncomment om tijd in RTC in te stellen
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
// huidige tijd ophalen uit RTC
  DateTime now  = rtc.now();
  datum.Nummer  = now.day();
  datum.Maand   = now.month();
  
  pinMode(HALL_dagNummer, INPUT_PULLUP);
  pinMode(HALL_maandNaam, INPUT_PULLUP);
  
// snelheid stappenmotoren
  dagNummer.setSpeed(15);
  maandNaam.setSpeed(15);

  while(digitalRead(HALL_maandNaam)){
    maandNaam.step(COUNTERCLOCKWISE);
  }
  while(digitalRead(HALL_dagNummer)){
    dagNummer.step(COUNTERCLOCKWISE);
  }

}

void loop(){
  
  while(1){
  // huidige dag ophalen
  DateTime now = rtc.now();
  datum.volgendNummer = now.day();
  
  if(datum.volgendNummer != datum.Nummer){
    // als datum veranderd is
    datum.volgendMaand = now.month();
    
    // als een nieuwe maand begint, nummer ring verder doorschuiven
    if(datum.volgendNummer == 1 && datum.Nummer >= 28){
      int aantal_plekken = 31 - datum.Nummer; // aantal nummers dat doorgeschoven moeten worden
      while(aantal_plekken != 0){
          schuifDag();           
          aantal_plekken--;
          }
       } 
   if(datum.volgendMaand != datum.Maand){
     schuifMaand();
   }
  }
    
  // variabelen resetten
  datum.Nummer = datum.volgendNummer;
  datum.Maand  = datum.volgendMaand;
  
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);   // LPM
}}
