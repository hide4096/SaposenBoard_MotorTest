#include "mbed.h"
#include <cmath>
#include <cstdint>
#include <math.h>

#define PI 3.141592
#define I_MAX 1000

/*
    モータ制御ピン
PA_11は内部ではTIM1_CH1Nを指定しているのでPA_8(TIM1_CH1)と競合しちゃう
PA_11_ALT0を使うとTIM1_CH4を指定できる
https://os.mbed.com/questions/86929/Strange-PWM-issue/
*/
PwmOut MOT1_IN1(PA_9);
PwmOut MOT1_IN2(PA_10);
PwmOut MOT2_IN1(PA_8);
PwmOut MOT2_IN2(PA_11_ALT0);

/*
    センサピン
---------------------
|   #   #   #   #   |
|   l2  l1  r1  r2  |
---------------------

adc1:r2
adc2:r1
adc3:l1
adc4:l2
*/
AnalogIn adc1(PA_0);
AnalogIn adc2(PA_3);
AnalogIn adc3(PB_1);
AnalogIn adc4(PB_0);

DigitalIn sw(PF_1);

Thread run_pid(osPriorityRealtime,1024);
Semaphore running(1);
Ticker loop1ms;

bool motor_is_disable = true;

void ControlMotor(int width_r,int width_l){
    if(motor_is_disable){
        MOT1_IN1.pulsewidth_us(0);
        MOT1_IN2.pulsewidth_us(0);
        MOT2_IN1.pulsewidth_us(0);
        MOT2_IN2.pulsewidth_us(0);
        return;
    }

    if(width_r > 1000) width_r = 1000;
    else if(width_r < -1000) width_r = -1000;
    if(width_l > 1000) width_l = 1000;
    else if(width_l < -1000) width_l = -1000;

    if(width_l>0){
        MOT1_IN1.pulsewidth_us(0);
        MOT1_IN2.pulsewidth_us(width_l);
    }else{
        MOT1_IN1.pulsewidth_us(-width_l);
        MOT1_IN2.pulsewidth_us(0);
    }
    if(width_r>0){
        MOT2_IN1.pulsewidth_us(width_r);
        MOT2_IN2.pulsewidth_us(0);
    }else{
        MOT2_IN1.pulsewidth_us(0);
        MOT2_IN2.pulsewidth_us(-width_r);
    }
}

uint16_t line_l1,line_l2,line_r1,line_r2;

void FetchLineSensor(){
    line_r2 = adc1.read_u16()>>4;
    line_r1 = adc2.read_u16()>>4;
    line_l1 = adc3.read_u16()>>4;
    line_l2 = adc4.read_u16()>>4;
}

//ここの値を書き換える
const float PGain = 2.0,IGain = 0.005,DGain = 0.1;

uint16_t speed = 0.;
long I_diff = 0.,past_diff = 0.;

void pid(){
    //左右それぞれ２個のセンサの平均値をとってる
    //重み付けをかえるといいかも
    int line_avr_r = (line_r2 + line_r1)/2;
    int line_avr_l = (line_l2 + line_l1)/2;

    int diff = line_avr_r - line_avr_l;

    int speed_l = speed + (diff*PGain+I_diff*IGain+(diff - past_diff)*DGain);
    int speed_r = speed - (diff*PGain+I_diff*IGain+(diff - past_diff)*DGain);

    past_diff = diff;
    I_diff+=diff;
    if(I_diff > I_MAX) I_diff = I_MAX;
    else if(I_diff < -I_MAX) I_diff = -I_MAX;

    ControlMotor(speed_r,speed_l);
}

//ライントレースの処理
void linetrace(){
    while(1){
        running.acquire();
        FetchLineSensor();
        pid();
    }
}

void flip1ms(){
    running.release();
}

int main(){

    sw.mode(PullNone);

    //モータの設定
    MOT1_IN1.period_us(1000);
    MOT1_IN2.period_us(1000);
    MOT2_IN1.period_us(1000);
    MOT2_IN2.period_us(1000);

    printf("Initialize Complete!\r\n");

    while(sw);

    motor_is_disable = false;
    speed = 180;

    osStatus st_runpid = run_pid.start(linetrace);
    loop1ms.attach_us(&flip1ms,1000);

    while(true){
        ThisThread::sleep_for(1);
    }

  }