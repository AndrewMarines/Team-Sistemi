#include <LiquidCrystal.h>
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
int altezza;
String sesso;
String entrata = ""; //input da serial monitor
String incomingByte;


void setup() {
  // put your setup code here, to run once:
  lcd.begin(16,2);     //inizializziamo l'oggetto LCD indicando il numero di colonne e righe
  pinMode (LED_ROSSO, OUTPUT);
  pinMode (LED_VERDE, OUTPUT);
  pinMode (LED_GIALLO, OUTPUT);
  Serial.begin(9600);
  stato=INIT;
  lcd.clear();
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
      digitalWrite(RETROILLUMINAZIONE,HIGH), digitalWrite(LED_ROSSO, LOW),digitalWrite(LED_VERDE, LOW),digitalWrite(LED_GIALLO, LOW);

      N_ADC = analogRead(SENSORE_CELLA);  //Leggo il valore dell'ingresso analogico.
      PESO_ATT = 150 * N_ADC; //Trovo il valore del peso attuale tramite la proporzione.
      PESO_ATT=PESO_ATT/1024;
      
      if(PESO_ATT>20){
        Serial.println("Inserisci gender(maschio/femmina): ");
        lcd.setCursor(0,0);
        lcd.print("Inserisci gender(maschio/femmina): ");
        stato=GENDER;
      }
      else{
        lcd.clear();
      }
    break;

    case GENDER:
      digitalWrite(RETROILLUMINAZIONE,HIGH);
      
        if(entrata!=""){
          sesso=entrata;
          if(sesso.equals("maschio\n")||sesso.equals("femmina\n")){
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
      if(entrata!=""){
        stato = LETTURA_CELLA;

        altezza=entrata.toInt();
        
      }

      break;

    case LETTURA_CELLA:
      lcd.clear();
      N_ADC = analogRead(SENSORE_CELLA);  //Leggo il valore dell'ingresso analogico.
      PESO_ATT = 150 * N_ADC; //Trovo il valore del peso attuale tramite la proporzione.
      PESO_ATT=PESO_ATT/1024;
      
      lcd.setCursor(1,2);
      lcd.print("PESO: "+String(PESO_ATT)+" kg");
      long  BMI= PESO_ATT/(int(int(altezza)/100)^2); 
      


      if(sesso=="maschio\n"||sesso=="femmina\n"){
          //NORMOPESO
          if(BMI>=18 && BMI<24){
            digitalWrite(LED_VERDE, HIGH);
            digitalWrite(LED_ROSSO, LOW);
            digitalWrite(LED_GIALLO, LOW);
            lcd.setCursor(0,0);
            lcd.print("SEI IN FORMA");
            delay(1000);

          }
          //SOTTOPESO
          if(BMI<18){
            digitalWrite(LED_VERDE, LOW);
            digitalWrite(LED_ROSSO, LOW);
            digitalWrite(LED_GIALLO, HIGH);
            lcd.setCursor(0,0);
            lcd.print("SEI SOTTOPESO");
            delay(1000);


          }
          //SOVRAPPESO
          if(BMI>=24){
            digitalWrite(LED_VERDE, LOW);
            digitalWrite(LED_ROSSO, HIGH);
            digitalWrite(LED_GIALLO, LOW);
            lcd.setCursor(0,0);
            lcd.print("SEI SOVRAPPESO");
            delay(1000);

          }
      }
      if(PESO_ATT<30){
          delay(2000);
          stato=INIT;
        }

      
        
      
  }
}
