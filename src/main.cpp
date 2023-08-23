#include "motor.h"
#include "constants.h"

#define SOP '<'
#define EOP '>'

Motor FRMotor(MD1_M1A, MD1_M1B, FRA, FRB);
Motor FLMotor(MD1_M2A, MD1_M2B, FLA, FLB);

Motor RRMotor(MD2_M1A, MD2_M1B, RRA, RRB);
Motor RLMotor(MD2_M2A, MD2_M2B, RLA, RLB);

long int interval = 10;
long int led_interval = 50;
long int lastMillis = 0;
long int led_lastMillis = 0;
bool led_state = false;

bool data_in_started = false;
bool data_in_ended = false;
char in_data_stream[255];
int in_data_index = 0;
int commandSpeed[4];
int motor_command_index = 0;
long int last_receive = 0;

void setup() {
  Serial.begin(1000000);
  Serial.setTimeout(0.05);
  FRMotor.invertMotorPin();
  FRMotor.init();
  FLMotor.init();
  RRMotor.invertMotorPin();
  RRMotor.init();
  RLMotor.init();
  lastMillis = millis();
  led_lastMillis = millis();
  pinMode(LED_BUILTIN, OUTPUT);
  commandSpeed[0] = 0;
  commandSpeed[1] = 0;
  commandSpeed[2] = 0;
  commandSpeed[3] = 0;
}

void loop() {
  long currentMillis = millis();
  FRMotor.update();
  FLMotor.update();
  RRMotor.update();
  RLMotor.update();

  while(Serial.available() > 0)
  {
   char inChar = Serial.read();
    if(inChar == SOP)
    {
      data_in_started = true;
      data_in_ended = false;
      in_data_index = 0;
   }
    else if(inChar == EOP || inChar == '\n')
    {
      data_in_started = false;
      data_in_ended = true;
      in_data_index = 0;
      motor_command_index = 0;

      FRMotor.move(commandSpeed[0]);
      FLMotor.move(commandSpeed[1]);
      RRMotor.move(commandSpeed[2]);
      RLMotor.move(commandSpeed[3]);
      break;
    }
    else
    {
      if(data_in_started && !data_in_ended)
      {
        if(inChar == ',')
        {
          char *pNext;
          commandSpeed[motor_command_index] = strtol(in_data_stream, &pNext, 10);
          in_data_index = 0;
          motor_command_index++;
          for(uint8_t i = 0; i < 10; i++)
          {
            in_data_stream[i] = 0;
          }
        }
        else if(inChar == 'r')
        {
          data_in_started = false;
          data_in_ended = true;
          in_data_index = 0;
          motor_command_index = 0;
          Serial.print('<');
          Serial.print(FRMotor.counter());
          Serial.print(',');
          Serial.print(FLMotor.counter());
          Serial.print(',');
          Serial.print(RRMotor.counter());
          Serial.print(',');
          Serial.print(RLMotor.counter());
          Serial.print(',');
          Serial.print(last_receive);
          Serial.println('>');
          break;
        }
        else
        {
          in_data_stream[in_data_index] = inChar;
          in_data_index++;
        }
      }
      else
      {

      }
    }
  }

/*
  if((currentMillis - lastMillis) >= interval)
  {
    lastMillis = currentMillis;
    Serial.print('<');
    Serial.print(FRMotor.counter());
    Serial.print(',');
    Serial.print(FLMotor.counter());
    Serial.print(',');
    Serial.print(RRMotor.counter());
    Serial.print(',');
    Serial.print(RLMotor.counter());
    Serial.println('>');

  }
  */

  if((currentMillis - led_lastMillis) >= led_interval)
  {
    led_lastMillis = currentMillis;
    led_state = !led_state;
    digitalWrite(LED_BUILTIN, led_state);
  }

}