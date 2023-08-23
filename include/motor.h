#include "Arduino.h"

class Motor
{
    int pinMA;
    int pinMB;
    int pinEA;
    int pinEB;
    long int _counter = 0;
    bool A_state = false;
    bool B_state = false;
    bool A_lstate = false;

    void forward(int speed)
    {
        analogWrite(pinMA, speed);
        analogWrite(pinMB, 0);
    }
    void reverse(int speed)
    {
        analogWrite(pinMB, speed);
        analogWrite(pinMA, 0);
    }
    void stop()
    {
        analogWrite(pinMA, 0);
        analogWrite(pinMB, 0);
    }

    public:
        Motor(){}
        Motor(int MA, int MB, int EA, int EB){
            pinMA = MA;
            pinMB = MB;
            pinEA = EA;
            pinEB = EB;
        }
        long int counter(){
            return _counter;
        }
        void invertMotorPin(){
            int temp = pinMA;
            pinMA = pinMB;
            pinMB = temp;
        }
        void init(){
            pinMode(pinMA, OUTPUT);
            pinMode(pinMB, OUTPUT);
            digitalWrite(pinMA, LOW);
            digitalWrite(pinMB, LOW);
            pinMode(pinEA, INPUT);
            pinMode(pinEB, INPUT);
            A_state = digitalRead(pinEA);
            A_lstate = A_state;
            B_state = digitalRead(pinEB);
        }
        void update(){
            A_state = digitalRead(pinEA);
            B_state = digitalRead(pinEB);

            if(A_state != A_lstate)
            {
                A_lstate = A_state;

                if(B_state != A_state)
                {
                    _counter++;
                }
                else
                {
                    _counter--;
                }
            }
        }
        void move(int speed)
        {
            if(speed > 255)
                speed = 255;

            if(speed < -255)
                speed = -255;

            if(speed > 0)
            {
                forward(speed);
            }
            else if(speed < 0)
            {
                reverse(abs(speed));
            }
            else
            {
                stop();
            }

        }

};