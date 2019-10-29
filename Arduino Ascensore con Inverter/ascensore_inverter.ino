#include <Encoder.h> //lettura 4x
Encoder altezza (19,18); //pin dell'encoder
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 20, 2);
long z;
byte FN0 = 38;
byte FN1 = 40;
byte FN2 = 42;
byte P0 = 44;
byte P1 = 46;
byte P2 = 48;
byte PEM = 50;              //pulsante emergenza
byte HL0 = 9;               //lampada piano 0
byte HL1 = 8;               //lampada piano 1
byte HL2 = 7;               //lampada piano 2
byte LED_EM = 6;            //led emergenza
byte INV_ON = 5;            //inverter attivo
byte INV_DISC = 4;          //discesa-salita
byte INV_LENTO = 3;         //veloce-lento
byte stato = 4;
bool emergenza=0;
bool piano_centrale=0;
char scritta_lcd="";

#define dw digitalWrite 
#define dr digitalRead
#define pm pinMode
#define aw analogWrite
#define ar analogRead
#define sp Serial.print
#define spn Serial.println


void setup() {
 pm(FN0,INPUT);pm(FN1,INPUT);pm(FN2,INPUT);pm(P0,INPUT);pm(P1,INPUT);pm(P2,INPUT);pm(PEM,INPUT);
 pm(HL0,OUTPUT);pm(HL1,OUTPUT);pm(HL2,OUTPUT);pm(LED_EM,OUTPUT);pm(INV_ON,OUTPUT);pm(INV_DISC,OUTPUT);pm(INV_LENTO,OUTPUT);

 Serial.begin(9600);
 lcd.begin();
}


void loop(){

  lcd.backlight();

  if(dr(PEM)==0){     //se viene premuto il pulsante di emergenza si porta allo stato 30 (emergenza)
    stato=30;
  }

  z=altezza.read();

  //gestione velocità
  if(emergenza==0){
    if(z<=150){
      dw(INV_LENTO,HIGH);


    } 
    if(z>150 && z<=1100){
      dw(INV_LENTO,LOW);


      
    }
    //controlla se vado al piano 1 o no
    if(piano_centrale==1){
      if(z>1100 && z<=1500){
        dw(INV_LENTO,HIGH);

      }
    }
    if(z>1500 && z<=2600){
      dw(INV_LENTO,LOW);

    }
    if(z>2600){
      dw(INV_LENTO,HIGH);

      
    }
  }
  else{
    dw(INV_LENTO,HIGH);
    

  }


  switch(stato){
    //stato generale
    case 0:
      dw(INV_ON,LOW);  //spengo inverter
      piano_centrale=0;
      emergenza=0;
      
  
      //piano 1 discesa
      if(dr(P1)==HIGH && z>=1250) { 
        piano_centrale=1;
        stato=1;
        scritta_lcd="DISCESA PIANO 1";
      }
      
      //piano 1 salita
      if(dr(P1)==HIGH && z<1250) {
        piano_centrale=1;
        stato=2;
        scritta_lcd="SALITA PIANO 1";
      }
         
      //piano 2 
      if(dr(P2)==HIGH) {
        stato=3;
        scritta_lcd="SALITA PIANO 2";
      }
      
      
      //piano 0
      if(dr(P0)==HIGH) {
        stato=4;
        scritta_lcd="DISCESA PIANO 0";
      }   
      break;

      //piano 1 discesa
     case 1:
      dw(INV_ON,HIGH), dw(INV_DISC,LOW), dw(HL1,HIGH), dw(HL0,LOW), dw(HL2,LOW);  //accendo inverter discesa, spengo salita, accendo luce 1

      
      //tocca il finecorsa e torna al generale
      if(dr(FN1)==LOW){
        stato=0;
        scritta_lcd="PIANO 1";
      }
      break;

      //piano 1 salita
     case 2:
      dw(INV_ON,HIGH), dw(INV_DISC,HIGH), dw(HL1,HIGH), dw(HL0,LOW), dw(HL2,LOW); //accendo inverter salita, spengo discesa, accendo luce 1


      //tocca finecorsa e torna al generale 
      if(dr(FN1)==LOW){
        stato=0;
        scritta_lcd="PIANO 1";

        
      }
      break;

      //piano 2 salita 
     case 3:
      dw(INV_ON,HIGH), dw(INV_DISC,HIGH), dw(HL2,HIGH), dw(HL0,LOW), dw(HL1,LOW);
     
      piano_centrale=0;

      
      if(dr(FN2)==LOW){
        stato=0;
        scritta_lcd="PIANO 2";

        
      }
      break;

      //va al piano 0
      case 4:
      dw(INV_ON,HIGH), dw(INV_DISC,LOW), dw(HL2,LOW), dw(HL1,LOW),dw(HL0,HIGH);

      piano_centrale=0;

      
      
      
      if(dr(FN0)==LOW){
        stato=0;
        altezza.write(0);
        scritta_lcd="PIANO 0";

      }
      break;

      //stato di emergenza
      case 30:
        dw(INV_ON,LOW);
        scritta_lcd="EMERGENZA";
        
        
        emergenza=1;
    
        //piano 1 discesa
        if(dr(P1)==HIGH && z>=1250) {
          stato=1;
          scritta_lcd="DISCESA PIANO 1";
        }
        
        //piano 1 salita
        if(dr(P1)==HIGH && z<1250) {
          stato=2;
          scritta_lcd="SALITA PIANO 1";
        }
           
        //piano 2 
        if(dr(P2)==HIGH) {
          stato=3;
          scritta_lcd="SALITA PIANO 2";
        }
        
        //piano 0
        if(dr(P0)==HIGH) {
          stato=4;
          scritta_lcd="DISCESA PIANO 0";
        }   
      break;

      
  }
  sp("z: ");sp(z); sp(" stato: "); sp(stato);; sp(" Pulsante Emergenza: "); sp(dr(PEM)); sp(" Pulsante 0: "); sp(dr(P0)); sp(" Pulsante 1: "); sp(dr(P1)); sp(" Pulsante 2: "); spn(dr(P2));
  sp(" F0: ");sp(dr(FN0));sp(" F1: ");sp(dr(FN1));sp(" F2: ");sp(dr(FN2));

  lcd.print(scritta_lcd);
  
}
