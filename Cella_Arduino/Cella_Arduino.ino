#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);    //RS, ENABLE, D7, D6, D5, D4

#define SENSORE_CELLA           A1  // ingresso analogico 
#define LED_ROSSO               7   //LED Rosso
#define LED_GIALLO              8   //LED Giallo
#define LED_VERDE               9   //LED Verde
#define RETROILLUMINAZIONE      10  //Pin modalità riposo



int PESO_ATT_PRE;    //Peso misurato di preselezione, da 0kg a 150kg
int PESO_ATT;        //Peso misurato, da 0kg a 150kg
int N_ADC;           //Numero corrispondente al ADC (O-1023)
byte stato = 0;
char altezza = ""; //input da serial monitor



void setup() {
  // put your setup code here, to run once:
  lcd.begin(16,2);     //inizializziamo l'oggetto LCD indicando il numero di colonne e righe
  pinMode (LED_ROSSO, OUTPUT);
  pinMode (LED_VERDE, OUTPUT);
  pinMode (LED_GIALLO, OUTPUT);
  Serial.begin(9600);

}

void loop() {

  //controlla che riceva dati dal monitor seriale
  if(Serial.available()>0){
    //salvo nella variabile l'input
    altezza=Serial.read();
  }

  switch (stato) {

    //verifico se qualcuno è sulla bilancia
    case 0:
    
      digitalWrite(RETROILLUMINAZIONE,LOW), digitalWrite(LED_ROSSO, LOW),digitalWrite(LED_VERDE, LOW),digitalWrite(LED_GIALLO, LOW);

      N_ADC = analogRead(SENSORE_CELLA);  //Leggo il valore dell'ingresso analogico.
      PESO_ATT_PRE = (150 * N_ADC) / 1024; //Trovo il valore del peso attuale tramite la proporzione.

      if(PESO_ATT_PRE>50){
        delay(1000);
        stato=1;
      }
      else{
        lcd.setCursor(1,2);
        lcd.print("                                                              ");
      }
    break;
      

    // chiede l'altezza (cm)
    case 1:
      digitalWrite(RETROILLUMINAZIONE,HIGH);
      N_ADC = analogRead(SENSORE_CELLA);  //Leggo il valore dell'ingresso analogico.
      PESO_ATT = (150 * N_ADC) / 1024; //Trovo il valore del peso attuale tramite la proporzione.
      Serial.println("Inserisci altezza: ");
      if(altezza!=""){
        stato = 2;
      }

      break;

    case 2:
      double BMI= PESO_ATT/(int(altezza))^2;     
      Serial.println("IL TUO PESO E': " + PESO_ATT);
      lcd.setCursor(1,2);
      lcd.print("IL TUO PESO E': " + String(PESO_ATT));

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
      
      delay(10000);
      stato=0;

      
  }
}
