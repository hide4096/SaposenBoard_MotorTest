#include "mbed.h"
#include <math.h>

#define PI 3.141592

PwmOut MOT1_IN1(PA_9);
PwmOut MOT1_IN2(PA_10);
PwmOut MOT2_IN1(PA_8);
PwmOut MOT2_IN2(PA_11_ALT0);
/*
PA_11は内部ではTIM1_CH1Nを指定しているのでPA_8(TIM1_CH1)と競合しちゃう
PA_11_ALT0を使うとTIM1_CH4を指定できる
https://os.mbed.com/questions/86929/Strange-PWM-issue/
*/

I2C imu(PB_7,PB_6);

// main() runs in its own thread in the OS
int main()
{
    MOT1_IN1.period_us(100);
    MOT1_IN2.period_us(100);
    MOT2_IN1.period_us(100);
    MOT2_IN2.period_us(100);

    float duty = 0.;
    int cnt = 0;

    while(true){
        duty = sin(cnt*(PI/180.));
        if(duty > 0){
            MOT1_IN1.write(0);
            MOT1_IN2.write(duty);
            MOT2_IN1.write(0);
            MOT2_IN2.write(duty);
        }else{
            MOT1_IN1.write(-duty);
            MOT1_IN2.write(0);
            MOT2_IN1.write(-duty);
            MOT2_IN2.write(0);
        }
        cnt=(cnt+1)%360;
        wait_us(10000);
  }
}

