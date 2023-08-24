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
long int last_command_receive = 0;
long int command_data_timeout = 0;
int consecutive_command_data = 0;
bool led_state = false;

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
  last_command_receive = millis();
  led_lastMillis = millis();
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  long currentMillis = millis();
  command_data_timeout = currentMillis - last_command_receive;
  FRMotor.update();
  FLMotor.update();
  RRMotor.update();
  RLMotor.update();

  if(command_data_timeout > 100)
  {
    FRMotor.move(0);
    FLMotor.move(0);
    RRMotor.move(0);
    RLMotor.move(0);
    consecutive_command_data = 0;
  }

  if(Serial.available() > 0)
  {
    char buffer[255];
    int inBufferSize;
    inBufferSize = Serial.readBytesUntil('\n', buffer, 255);

    bool start_data = false;
    bool end_data = false;
    bool command_data = false;
    int in_data_index = 0;
    char in_data_stream[255];
    int commandSpeed[4] = {0,0,0,0};
    int motor_command_index = 0;

    for(uint8_t i = 0; i < inBufferSize; i++)
    {
      if(buffer[i] == SOP)
      {
        start_data = true;
        end_data = false;

      }
      else if(buffer[i] == EOP)
      {
        start_data = false;
        end_data = true;
        if(consecutive_command_data > 10)
        {
          FRMotor.move(commandSpeed[0]);
          FLMotor.move(commandSpeed[1]);
          RRMotor.move(commandSpeed[2]);
          RLMotor.move(commandSpeed[3]);
          consecutive_command_data = 10;
        }
       break;
      }
      else
      {
        if(start_data && !end_data)
        {
          if(buffer[i] == 'r')
          {
            start_data = false;
            end_data = true;
            Serial.print('<');
            Serial.print(FRMotor.counter());
            Serial.print(',');
            Serial.print(FLMotor.counter());
            Serial.print(',');
            Serial.print(RRMotor.counter());
            Serial.print(',');
            Serial.print(RLMotor.counter());
            Serial.println('>');
            break;
          }
          else if(buffer[i] == ',')
          {
            char *pNext;
            commandSpeed[motor_command_index] = strtol(in_data_stream, &pNext, 10);
            in_data_index = 0;
            motor_command_index++;
            for(uint8_t j = 0; j < 255; j++)
            {
              in_data_stream[j] = 0;
            }
          }
          else
          {
            command_data = true;
            in_data_stream[in_data_index] = buffer[i];
            in_data_index++;
          }
        }

      }
    }
    if(command_data)
    {
      last_command_receive = currentMillis;
      consecutive_command_data++;
    }
  }

  if((currentMillis - led_lastMillis) >= led_interval)
  {
    led_lastMillis = currentMillis;
    led_state = !led_state;
    digitalWrite(LED_BUILTIN, led_state);
  }

}