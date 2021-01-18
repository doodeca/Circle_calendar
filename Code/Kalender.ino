#include <LowPower.h>
#include <Stepper.h>
#include <RTClib.h>

RTC_DS3231 rtc;        // RTC module uses DS3231

#define OMWENTELING 2038  // number of steps needed to make one revolution

#define HALL_dagNummer 3  // schematic: HALL_0, on PCB: U2, RV2
#define HALL_maandNaam 4  // schematic: HALL_1, on PCB: U1, RV1

const int COUNTERCLOCKWISE = -1;

// stepper sequence = orange, purple, yellow, blue
Stepper dagNummer = Stepper(OMWENTELING, 12, 10, 11, 9);  // STEPPER 0
Stepper maandNaam = Stepper(OMWENTELING, 5, 7, 6, 8);     // STEPPER_1

// struct for keeping this and next day
typedef struct {
  uint8_t Nummer;
  uint8_t Maand;
  uint8_t volgendNummer;
  uint8_t volgendMaand;
} Datum;

  Datum datum;

void setup() {
  rtc.begin();
  
// Uncomment to set RTC date/time
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
// get current time from RTC
  DateTime now  = rtc.now();
  datum.Nummer  = now.day();
  datum.Maand   = now.month();
  
  pinMode(HALL_dagNummer, INPUT_PULLUP);
  pinMode(HALL_maandNaam, INPUT_PULLUP);
  
// set stepper speed
  dagNummer.setSpeed(15);
  maandNaam.setSpeed(15);
}

void loop(){
  // get current day
  DateTime now = rtc.now();
  datum.volgendNummer = now.day();
  
  if(datum.volgendNummer != datum.Nummer){
    // if the date has changed
    datum.volgendMaand = now.month();
    
    if(datum.volgendNummer == 1 && datum.Nummer >= 28){    // if the new month started
      int aantal_plekken = 31 - datum.Nummer;    // number of places that should be skipped
      
      while(aantal_plekken != 0){
        int bewegen = 1;
        
        while(bewegen){
          maandNaam.step(COUNTERCLOCKWISE);
          
          if(digitalRead(HALL_maandNaam) == LOW){
            while(digitalRead(HALL_maandNaam) == LOW){
              maandNaam.step(COUNTERCLOCKWISE);
            }
            aantal_plekken--;
            bewegen = 0; 
          }
        }
      }
    }
    
    if(datum.volgendMaand != datum.Maand){
      int bewegen = 1;
      
      while(bewegen){
        maandNaam.step(COUNTERCLOCKWISE);
        
        if(digitalRead(HALL_maandNaam) == LOW){
          while(digitalRead(HALL_maandNaam) == LOW){
            maandNaam.step(COUNTERCLOCKWISE);
          }
          bewegen = 0; 
        }
      }
    }
  }
  // reset variables
  datum.Nummer = datum.volgendNummer;
  datum.Maand  = datum.volgendMaand;
  
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);   // sleep for 8 seconds
}
