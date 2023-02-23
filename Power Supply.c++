//Librerias
#include <Arduino.h>
#include <TimerOne.h>
#include <PWM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RotaryEncoder.h>
LiquidCrystal_I2C lcd(0x3F, 16, 2);

//principales
#define MOSFET 8
#define RELE 10
#define ENCENDIDO 9
#define BUZZER 12

//tension, corriente
float CUENTA_2;
float CUENTA_3;
float TENSION_shunt;
float TENSION_entrada;
float TENSION_salida;
float POTENCIA;
float corriente1;
float opamp = A0;
float corriente = 0.0;
const float resistencia = 0.059;

//temperatura
float TEMPERATURA;
float CUENTA;
int SENSOR = A2;

//Pwm
int pwm = 4;
int32_t frequency;
int result = 0;
int CONTROL;

//switch
int contador = 0;

//encoder
volatile int POSICION = 0; 
volatile int POSICION_2 = 0;
int counter_2 = 0;
int duration = 0;

int Counter = 0, LastCount = 0;
int sw = 13;

//cronometro
unsigned long tiempo = 0;
unsigned long tiempo2 = 0;

int btnState;
bool clear = true;

 void RotaryChanged();
 RotaryEncoder Rotary(&RotaryChanged, 2, 3, 13);
 int led;

void setup()
  {
    pinMode(MOSFET, OUTPUT);
    pinMode (RELE, OUTPUT);
    pinMode (pwm, OUTPUT);
    pinMode (BUZZER, OUTPUT);
    pinMode (ENCENDIDO, INPUT);
    pinMode(13,INPUT_PULLUP);
    pinMode(pwm, OUTPUT);
    digitalWrite(pwm, HIGH); 
    analogReference(EXTERNAL);
    lcd.init();
    lcd.home();
    lcd.backlight();
    InitTimersSafe();
    Rotary.setup();   

      if (digitalRead(ENCENDIDO) == HIGH)
        {
            digitalWrite (BUZZER, HIGH);
            delay(50);
            digitalWrite (BUZZER, LOW);
            delay(50);
            digitalWrite (BUZZER, HIGH);
            delay(50);
            digitalWrite (BUZZER, LOW);
            delay(50);
        }

          lcd.setCursor(0, 0);
          lcd.print("Tec. Electronico");
          lcd.setCursor(3, 1);
          lcd.print("Enzo Devit");
          delay(1000);
          lcd.clear();
          delay(20);
  }


void refrigeracion()
  {
    if(digitalRead(RELE)== LOW)
        {
            lcd.clear();
            do
            {
                lcd.setCursor(2, 0);
                lcd.print("REFRIGERANDO");
                lcd.setCursor(5, 1);
                lcd.print(TEMPERATURA);
                CUENTA = analogRead(SENSOR);
                TEMPERATURA = (2.7 * CUENTA * 100.0) / 1024.0;
            }
            while (TEMPERATURA >= 60.0);
         }
  }

void datos()
  {

      bool success = SetPinFrequencySafe(led, frequency);
      CUENTA = analogRead(SENSOR);
      TEMPERATURA = (2.7 * CUENTA * 100.0) / 1024.0;
      CUENTA_2 = analogRead(A0);
      TENSION_shunt = ((((2.7 * CUENTA_2) / 1024) * 2) / 10);
      CUENTA_3 = analogRead(A1);
      TENSION_entrada = ((CUENTA_3 * 2.7) / 1024);

      for(int i = 0; i < 150; i++)
          {    
            TEMPERATURA = (2.7 * CUENTA * 100.0) / 1024.0 + TEMPERATURA;
          }
        TEMPERATURA = (TEMPERATURA/150);

      for (int i = 0; i < 150; i++)
          {
            corriente = (TENSION_shunt / resistencia) + corriente;
          }
        corriente = (corriente / 150);

      for (int j = 0; j < 150; j++)
          {
            TENSION_salida = ((TENSION_entrada * 9600) / 1000) + TENSION_salida;
          }
        TENSION_salida = (TENSION_salida / 150);
    
      if (TEMPERATURA >= 40.0 || corriente >= 4.500)
        { 
          digitalWrite (MOSFET, HIGH);
        }

      if (TEMPERATURA <= 20.0 && corriente <= 1.500)
        {
          digitalWrite (MOSFET, LOW);
        }
        
    
      if (TEMPERATURA >= 65.0)
        {
          digitalWrite (RELE, LOW);
        }
    
      if (TEMPERATURA <= 45.0)
        {
          digitalWrite (RELE, HIGH);
        }
        
      analogWrite (pwm, POSICION);
      POTENCIA = (TENSION_salida * corriente);
  }


 void pantalla_1()
  {
    if(contador == 0)
      {
        lcd.setCursor(0, 0);
        lcd.print(TENSION_salida, 3);
        lcd.print("V   ");
        lcd.setCursor(9, 0);
        lcd.print(corriente, 3);
        lcd.print("A   ");
        lcd.setCursor(0, 1);
        lcd.print(POTENCIA, 3);
        lcd.print("W   ");
        lcd.setCursor(9, 1);
        lcd.print(TEMPERATURA, 2);
        lcd.setCursor(14, 1);
        lcd.print("C  ");
        corriente = 0.0;
        delay(50);
      }

    if(clear == false)
      {
        lcd.clear();
      }

    if(contador == 1)
      {
        lcd.setCursor(6, 0);
        lcd.print(frequency);
        lcd.print("Hz  ");
        lcd.setCursor(0, 1);
        lcd.print((POSICION * 100) / 255);
        lcd.print("%  ");
        lcd.setCursor(9, 1);
        lcd.print(TEMPERATURA, 2);
        lcd.setCursor(14, 1);
        lcd.print("C  ");
        delay(50);
      }

    if(contador == 2)
      {
        lcd.setCursor(6, 0);
        lcd.print(frequency);
        lcd.print("Hz  ");
        lcd.setCursor(0, 1);
        lcd.print((POSICION * 100) / 255);
        lcd.print("%  ");
        lcd.setCursor(9, 1);
        lcd.print(TEMPERATURA, 2);
        lcd.setCursor(14, 1);
        lcd.print("C  ");
        delay(50);          
      }
  }

void RotaryChanged()
  {
    const unsigned int state = Rotary.GetState();
      if (state & DIR_CW)
        {
          if(contador ==2)
            Counter++;
          
          if(contador == 1)
            counter_2++;
        }

      if (state & DIR_CCW)
        {
          if(contador ==2)
            Counter--;

          if(contador == 1)
            counter_2--; 
        }

      if(POSICION_2 == 2000000)
        POSICION_2 = 2000000;

      if(POSICION_2 == 1)
        POSICION_2 = 1;
        
      if(POSICION == 0)
        POSICION = 0;

      if(POSICION == 255)
        POSICION = 255;
  }



void loop()
  {
    switch (contador)
    {
      case 0:
        datos();
        pantalla_1();
        refrigeracion();
          if (Rotary.GetButtonDown()) 
            {
              clear = false;
              contador++;
            }        

        btnState = digitalRead(13);

          if(contador == 0)
            {
              clear = true;
            }
        
      break;
        
      case 1:
        pantalla_1();
        RotaryChanged();
        datos();
        refrigeracion();
          if (Rotary.GetButtonDown()) 
              {
                clear = false;
                contador++;
              }

          if(frequency <= 0)
          frequency = 0;

          if(frequency >= 32000)
          frequency = 32000;

          if (contador == 1) 
              {
                POSICION_2 = counter_2;           
                frequency = POSICION_2;
              }

        SetPinFrequencySafe(pwm, frequency);
        btnState = digitalRead(13);

          if(contador  == 1)
              {
                clear = true;
              }

        break;

        case 2:
        RotaryChanged();
        datos();
        pantalla_1();
        refrigeracion();
          if (Rotary.GetButtonDown()) 
              {
                contador++;
                clear = false;
              }

        if(POSICION >= 255)
            POSICION = 255;

        if(POSICION <= 0)
            POSICION = 0;

        if (contador == 2) 
            {
              POSICION = Counter;
              analogWrite(pwm, POSICION);
            }
          
        if (contador == 1) 
            {                  
              frequency = POSICION_2;
            }

        SetPinFrequencySafe(pwm, frequency);
        btnState = digitalRead(13);

        if(contador == 2)
          {
            clear = true;
          }

        if(contador > 2)
          {
            contador = 0;
          }
        break;
    }

  }