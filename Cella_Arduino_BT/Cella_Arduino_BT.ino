#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

const int rxPin = 6;
const int txPin = 9;
SoftwareSerial bluetooth(rxPin, txPin);

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);    //RS, ENABLE, D7, D6, D5, D4

#define SENSORE_CELLA           A1  // ingresso analogico 
#define LED_ROSSO               7   //LED Rosso
#define LED_GIALLO              8   //LED Giallo
#define LED_VERDE               13  //LED Verde
#define RETROILLUMINAZIONE      10  //Pin modalità riposo

enum {
  INIT,             //Inizializzazione
  GENDER,           //CHIEDI  GENDER
  ALTEZZA,          //CHIEDI ALTEZZA.
  LETTURA_CELLA,    //Lettura del peso
  ALLARME,           //Emergenza
  RITORNO
};

unsigned long PESO_ATT;        //Peso misurato, da 0kg a 150kg
long  N_ADC;           //Numero corrispondente al ADC (O-1023)
long stato;
float altezza;
String sesso;
String entrata = ""; //input da serial monitor
String message="";


void setup() {
  // put your setup code here, to run once:
  lcd.begin(16,2);     //inizializziamo l'oggetto LCD indicando il numero di colonne e righe
  pinMode (LED_ROSSO, OUTPUT);
  pinMode (LED_VERDE, OUTPUT);
  pinMode (LED_GIALLO, OUTPUT);
  Serial.begin(9600);
  bluetooth.begin(9600); //set baud rate
  stato=INIT;
  lcd.clear();
  delay(500);

}



void loop() {

  //controlla che riceva dati dal monitor seriale
  if (Serial.available() > 0) {

  entrata=Serial.readString();
  
  }
  

  switch (stato) {
    //verifico se qualcuno è sulla bilancia
    case INIT:
      lcd.clear();
      digitalWrite(RETROILLUMINAZIONE,LOW), digitalWrite(LED_ROSSO, LOW),digitalWrite(LED_VERDE, LOW),digitalWrite(LED_GIALLO, LOW);

      N_ADC = analogRead(SENSORE_CELLA);  //Leggo il valore dell'ingresso analogico.
      PESO_ATT = 150 * N_ADC; //Trovo il valore del peso attuale tramite la proporzione.
      PESO_ATT=PESO_ATT/1024;

      PESO_ATT = 70;
      
      if(PESO_ATT>20){
        Serial.println("Inserisci gender(maschio/femmina): ");
        lcd.setCursor(0,0);
        lcd.print("Inserisci gender(maschio/femmina): ");
        bluetooth.println("Inserisci gender(maschio/femmina): ");
        stato=GENDER;
      }
      else{
        lcd.clear();
      }
    break;

    case GENDER:
        digitalWrite(RETROILLUMINAZIONE,HIGH);
        if (bluetooth.available() > 0) {
              sesso = bluetooth.readString();
              bluetooth.println(sesso);
              if(sesso.equals("maschio\n") || sesso.equals("femmina\n")){
                lcd.clear();
                Serial.println("Inserisci altezza: ");
                bluetooth.println("Inserisci altezza: ");
                lcd.setCursor(0,0);
                lcd.print("Altezza?");
                entrata="";
                stato = ALTEZZA;
              }

          }
      
        if(entrata!=""){
             
          sesso=entrata;
          if(sesso.equals("maschio\n")||sesso.equals("femmina\n")){
            if (bluetooth.available() > 0) {
              bluetooth.print(sesso);
            }
            lcd.clear();
            Serial.println("Inserisci altezza: ");
            lcd.setCursor(0,0);
            lcd.print("Altezza?");
            entrata="";
            stato = ALTEZZA;
          }

        }

    break;

    // chiede l'altezza (cm)
    case ALTEZZA:
      if (bluetooth.available() > 0) {
              message = bluetooth.read();
              if(message!=""){
                altezza=message.toInt();
                message="";
                altezza=float(altezza/100);
                altezza=altezza*altezza;
                stato = LETTURA_CELLA;
                        
                }
          } 
    
      if(entrata!=""){
        altezza=entrata.toInt();
        altezza=float(altezza/100);
        altezza=altezza*altezza;
        if (bluetooth.available() > 0) {
        bluetooth.print(altezza);
        }
        stato = LETTURA_CELLA;

      }

      break;

    case LETTURA_CELLA:
      lcd.clear();
      N_ADC = analogRead(SENSORE_CELLA);  //Leggo il valore dell'ingresso analogico.
      PESO_ATT = 150 * N_ADC; //Trovo il valore del peso attuale tramite la proporzione.
      PESO_ATT=PESO_ATT/1024;
      
      lcd.setCursor(1,2);
      lcd.print("PESO: "+String(PESO_ATT)+" kg");
      

      long  BMI= PESO_ATT/altezza; 
      


      if(sesso=="maschio\n"||sesso=="femmina\n"){
          //NORMOPESO
          if(BMI>=19 && BMI<29.5){
            digitalWrite(LED_VERDE, HIGH);
            digitalWrite(LED_ROSSO, LOW);
            digitalWrite(LED_GIALLO, LOW);
            lcd.setCursor(0,0);
            lcd.print("SEI IN FORMA");
            if (bluetooth.available() > 0) {
               bluetooth.print("SEI IN FORMA... PESO: "+String(PESO_ATT)+" kg");
            }
            delay(100);

          }
          //SOTTOPESO
          if(BMI<19){
            digitalWrite(LED_VERDE, LOW);
            digitalWrite(LED_ROSSO, LOW);
            digitalWrite(LED_GIALLO, HIGH);
            lcd.setCursor(0,0);
            lcd.print("SEI SOTTOPESO");
            if (bluetooth.available() > 0) {
               bluetooth.print("SEI SOTTOPESO... PESO: "+String(PESO_ATT)+" kg");
            }
            delay(100);


          }
          //SOVRAPPESO
          if(BMI>=29.5){
            digitalWrite(LED_VERDE, LOW);
            digitalWrite(LED_ROSSO, HIGH);
            digitalWrite(LED_GIALLO, LOW);
            lcd.setCursor(0,0);
            lcd.print("SEI SOVRAPPESO");
            if (bluetooth.available() > 0) {
               bluetooth.print("SEI SOVRAPPESO... PESO: "+String(PESO_ATT)+" kg");
            }
            delay(100);

          }
      }
      if(PESO_ATT<30){
          delay(200);
          stato=INIT;
        }

      
        
      
  }
}
