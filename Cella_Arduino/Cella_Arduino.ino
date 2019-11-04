#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);    //RS, ENABLE, D7, D6, D5, D4

#define SENSORE_CELLA           A1  // ingresso analogico 
#define LED_ROSSO               7   //LED Rosso
#define LED_GIALLO              8   //LED Giallo
#define LED_VERDE               9   //LED Verde
#define RETROILLUMINAZIONE      10  //Pin modalità riposo

enum {
  INIT,             //Inizializzazione
  GENDER,           //CHIEDI  GENDER
  ALTEZZA,       //CHIEDI ALTEZZA.
  LETTURA_CELLA,    //Lettura del peso
  ALLARME           //Emergenza
};

int PESO_ATT_PRE;    //Peso misurato di preselezione, da 0kg a 150kg
int PESO_ATT;        //Peso misurato, da 0kg a 150kg
int N_ADC;           //Numero corrispondente al ADC (O-1023)
byte stato;
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

}

void loop() {

  PESO_ATT_PRE=200;
  PESO_ATT=200;

  //Serial.print(PESO_ATT);
  //Serial.print(N_ADC);
  //controlla che riceva dati dal monitor seriale
  if (Serial.available() > 0) {

  entrata=Serial.readString();
  
  }
  


  switch (stato) {

    //verifico se qualcuno è sulla bilancia
    case INIT:
    
      digitalWrite(RETROILLUMINAZIONE,HIGH), digitalWrite(LED_ROSSO, LOW),digitalWrite(LED_VERDE, LOW),digitalWrite(LED_GIALLO, LOW);

      N_ADC = analogRead(SENSORE_CELLA);  //Leggo il valore dell'ingresso analogico.
      //PESO_ATT_PRE = (150 * N_ADC) / 1024; //Trovo il valore del peso attuale tramite la proporzione.
      PESO_ATT_PRE=200;
      if(PESO_ATT_PRE>50){
        Serial.println("Inserisci gender(maschio/femmina): ");
        stato=GENDER;
      }
      else{
        lcd.clear();
      }
    break;

    case GENDER:
      digitalWrite(RETROILLUMINAZIONE,LOW);
      
        if(entrata!=""){
          sesso=entrata;
          if(sesso.equals("maschio\n")||sesso.equals("femmina\n")){
            Serial.println("Inserisci altezza: ");
            entrata="";
            stato = ALTEZZA;
          }

        }

    break;

    // chiede l'altezza (cm)
    case ALTEZZA:
      
      if(entrata!=""){
        stato = 4;
        altezza=190;
        
        //altezza=entrata;
        
      }

      break;

    case LETTURA_CELLA:
      Serial.println(stato);
      N_ADC = analogRead(SENSORE_CELLA);  //Leggo il valore dell'ingresso analogico.
      PESO_ATT = (150 * N_ADC) / 1024; //Trovo il valore del peso attuale tramite la proporzione.
      
         
      Serial.println("IL TUO PESO E': " + PESO_ATT);
      lcd.setCursor(1,2);
      lcd.print("IL TUO PESO E': " + String(PESO_ATT));
      double BMI= PESO_ATT/(int(altezza)^2);  

      if(sesso=="maschio"){
          //NORMOPESO
          if(BMI>=18.50 && BMI<24.99){
            digitalWrite(LED_VERDE, HIGH);
            digitalWrite(LED_ROSSO, LOW);
            digitalWrite(LED_GIALLO, LOW);
          }
          //SOTTOPESO
          if(BMI<18.50){
            digitalWrite(LED_VERDE, LOW);
            digitalWrite(LED_ROSSO, LOW);
            digitalWrite(LED_GIALLO, HIGH);
          }
          //SOVRAPPESO
          else{
            digitalWrite(LED_VERDE, LOW);
            digitalWrite(LED_ROSSO, HIGH);
            digitalWrite(LED_GIALLO, LOW);
          }
      }
      else{
        //NORMOPESO
          if(BMI>=18.50 && BMI<24.99){
            digitalWrite(LED_VERDE, HIGH);
            digitalWrite(LED_ROSSO, LOW);
            digitalWrite(LED_GIALLO, LOW);
          }
          //SOTTOPESO
          if(BMI<18.50){
            digitalWrite(LED_VERDE, LOW);
            digitalWrite(LED_ROSSO, LOW);
            digitalWrite(LED_GIALLO, HIGH);
          }
          //SOVRAPPESO
          else{
            digitalWrite(LED_VERDE, LOW);
            digitalWrite(LED_ROSSO, HIGH);
            digitalWrite(LED_GIALLO, LOW);
          }
      }
      
        
      
      
      
      delay(10000);
      //stato=INIT;

      break;
  }
}
