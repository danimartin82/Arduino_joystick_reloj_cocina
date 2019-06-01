#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

/**********************************************************/
// LCD connected to A4, A5, GND, 5V
// Vrx-A2 Vry ->A3. SW->A1. And GND, 5V.
/**********************************************************/

#define SECS_PER_CLICK 30 //secs per click de boton
#define MAIN_PERIOD 10    // [msecs]


const int analogInPin1 = A2; // Analog input pin that the Accelerometer's first pin is attached to
const int analogInPin2 = A3; // Analog input pin that the Accelerometer's second pin is attached to
const int analogInPin3 = A1; // Analog input pin that the Accelerometer's second pin is attached to
int joystick_Vrx = 0; // value read from the joystick's first pin
int joystick_Vry = 0; // value read from the joystick's second pin
int joystick_Sw = 0; // value read from the joystick's second pin
byte LCD_I2C_ADDRESS = 0x27;
LiquidCrystal_I2C             lcd(LCD_I2C_ADDRESS, 2, 1, 0, 4, 5, 6, 7);


typedef enum {LEFT, RIGHT, BUTTOM, NOTHING} possible_actions;
typedef enum {RUNNING, STOP} clock_possible_state;


possible_actions joystick_action = NOTHING;
possible_actions joystick_previous_action = NOTHING;
clock_possible_state clock_state=STOP;


int contador_right = 0;
int contador_left = 0;
int contador_buttom = 0;
int buttom_timer=0;

int minutos=0;
int segundos=0;
int tot_segundos=0;
int main_timer=0;

/**********************************************************/
//
/**********************************************************/
void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(57600);
  lcd.begin (16, 2);   // Inicializar el display con 16 caraceres 2 lineas
  lcd.setBacklightPin(3, POSITIVE);
  lcd.setBacklight(HIGH);
  print_lcd();
}
/**********************************************************/
//
/**********************************************************/
void loop() {
  process_joystick();
  decrease_timer();
  
  delay(MAIN_PERIOD);

}
/**********************************************************/
//
/**********************************************************/
void decrease_timer(void)
{
  if (clock_state==RUNNING)
  {
    main_timer++;
    if(MAIN_PERIOD*main_timer>=1000)
    {
      main_timer=0;
      decrease_secs();
    }
  }
}
/**********************************************************/
//
/**********************************************************/
void print_lcd(void)
{
 lcd.clear();

 if (minutos<10)
 {
  lcd.setCursor(0,0);
  lcd.print("0"); 
  lcd.setCursor(1,0);
  lcd.print(minutos);
 }
 else
 {
  lcd.setCursor(0,0);
  lcd.print(minutos);
 }
 
 lcd.setCursor(2,0);
 lcd.print(":");

 if (segundos<10)
 {
  lcd.setCursor(3,0);
  lcd.print("0"); 
  lcd.setCursor(4,0);
  lcd.print(segundos);
 }
 else
 {
  lcd.setCursor(3,0);
  lcd.print(segundos);
 }
 
}

/**********************************************************/
//
/**********************************************************/
void ring_bell(void)
{

  lcd.setCursor(0,0);
  lcd.print("################"); 
  lcd.setCursor(0,1);
  lcd.print("################"); 

  lcd.setBacklight(HIGH);
  delay(500);
  lcd.setBacklight(LOW);
  delay(500);
  lcd.setBacklight(HIGH);
  delay(500);
  lcd.setBacklight(LOW);
  delay(500);
  lcd.setBacklight(HIGH);
  delay(500);
  lcd.setBacklight(LOW);
  delay(500);
  lcd.setBacklight(HIGH);
  delay(500);
  lcd.setBacklight(LOW);
  delay(500);
  lcd.setBacklight(HIGH); 
  delay(500); 
  lcd.setBacklight(LOW);
  delay(500);
  lcd.setBacklight(HIGH);
  delay(500);
  lcd.setBacklight(LOW);
  delay(500);
  lcd.setBacklight(HIGH);
  delay(500);
  lcd.setBacklight(LOW);
  delay(500);
  lcd.setBacklight(HIGH);
  delay(500);
  lcd.setBacklight(LOW);
  delay(500);
  lcd.setBacklight(HIGH);   
  delay(500);
  
  print_lcd();
}
/**********************************************************/
//
/**********************************************************/
void calculate_min_secs(void)
{
   minutos=int(tot_segundos/60);
   segundos=int(tot_segundos%60);
}


/**********************************************************/
//
/**********************************************************/
void decrease_secs(void)
{
    if(tot_segundos>0)
    {
      tot_segundos--;
      calculate_min_secs();
      print_lcd();
    }
    else
    {
      tot_segundos=0;
      ring_bell();
      clock_state=STOP;
      
    }

}
/**********************************************************/
//
/**********************************************************/
void buttom_increase_secs(void)
{
  if (clock_state==STOP)
  {
    tot_segundos=tot_segundos+SECS_PER_CLICK;
  }
  calculate_min_secs();
  print_lcd();
}
/**********************************************************/
//
/**********************************************************/
void buttom_decrease_secs(void)
{
  if (clock_state==STOP)
  {
    if(tot_segundos>SECS_PER_CLICK)
    {
      tot_segundos=tot_segundos-SECS_PER_CLICK;
    }
    else
    {
      tot_segundos=0;
    }
   }

   calculate_min_secs();
   print_lcd();
}
/**********************************************************/
//
/**********************************************************/
void buttom_start_count(void)
{
  if (clock_state==STOP)
  {
  clock_state=RUNNING;
  Serial.print("RUNNING\n");
  }
  else
  {
  clock_state=STOP;
  Serial.print("STOP\n");
  }
}
/**********************************************************/
//
/**********************************************************/
void process_joystick(void)
{
 // read the both analog in values:
  joystick_Vrx = analogRead(analogInPin1);
  joystick_Vry = analogRead(analogInPin2);
  joystick_Sw  = analogRead(analogInPin3);
  
  // Detect actions
  if (joystick_Vrx > 1000)
  {
    joystick_action = LEFT;
  }
  else if (joystick_Vrx < 20)
  {
    joystick_action = RIGHT;
  }
  else if (joystick_Sw == 0)
  {
    joystick_action = BUTTOM;
  }
  else
  {
    joystick_action = NOTHING;
  }


  //Compare to previous actions to detect only once each action
  if (joystick_previous_action == joystick_action)
  {
    if (joystick_action==BUTTOM)
    {
      buttom_timer++;
    }
  }
  else
  {
    if(joystick_action==RIGHT)
    {
        // USER HAS PULSED RIGHT
        contador_right++;
        buttom_decrease_secs();
    }
    if (joystick_action==LEFT)
    {
        // USER HAS PULSED LEFT
        contador_left++;
        buttom_increase_secs();
    }    
    if ((joystick_action==NOTHING)&&(joystick_previous_action==BUTTOM)&&(buttom_timer>10))
    {
       // USER HAS PULSED BUTTOM
        buttom_timer=0;
        contador_buttom++;
        buttom_start_count();
    }    

    joystick_previous_action = joystick_action;
    joystick_action = NOTHING;
  }


}

