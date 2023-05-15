#include "mbed.h"
#include <cmath>
#include <cstdint>
#include <math.h>

#define PI 3.141592
#define I_MAX 10000
#define IMU_ADRS 0x68

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

I2C imu(PB_7,PB_6);

Thread run_pid(osPriorityRealtime,1024);
Thread print_info(osPriorityNormal,1024);
Semaphore running(1);
Ticker loop1ms;

bool motor_is_disable = true;

void ControlMotor(float duty_r,float duty_l){
    if(motor_is_disable){
        MOT1_IN1.write(0);
        MOT1_IN2.write(0);
        MOT2_IN1.write(0);
        MOT2_IN2.write(0);
        return;
    }

    if(duty_l>0.){
        MOT1_IN1.write(0);
        MOT1_IN2.write(duty_l);
    }else{
        MOT1_IN1.write(-duty_l);
        MOT1_IN2.write(0);
    }
    if(duty_r>0.){
        MOT2_IN1.write(duty_r);
        MOT2_IN2.write(0);
    }else{
        MOT2_IN1.write(0);
        MOT2_IN2.write(-duty_r);
    }
}

uint16_t line_buf[4][9];
uint16_t line_l1,line_l2,line_r1,line_r2;

void FetchLineSensor(){
    for(int i=0;i<9;i++){
        line_buf[0][i] = adc1.read_u16();
        line_buf[1][i] = adc2.read_u16();
        line_buf[2][i] = adc3.read_u16();
        line_buf[3][i] = adc4.read_u16();
    }
    for(int i=0;i<9;i++){
        for(int j=9;j>i;j--){
            for(int k=0;k<4;k++){
                if(line_buf[k][j-1] > line_buf[k][j]){
                    uint16_t tmp = line_buf[k][j];
                    line_buf[k][j] = line_buf[k][j-1];
                    line_buf[k][j-1] = tmp;
                }
            }

        }
    }
    line_r2 = line_buf[0][4];
    line_r1 = line_buf[1][4];
    line_l1 = line_buf[2][4];
    line_l2 = line_buf[3][4];
}

const float PGain = 0.01,IGain = 0.,DGain = 0.0;
float speed = 0.;
int32_t I_diff = 0.,past_diff = 0.;

void pid(){
    float line_avr_r = line_r2 + line_r1;
    float line_avr_l = line_l2 + line_l1;

    int32_t diff = line_avr_r - line_avr_l;

    float duty_l = speed + (diff*PGain+I_diff*IGain+(diff - past_diff)*DGain)*0.;
    float duty_r = speed - (diff*PGain+I_diff*IGain+(diff - past_diff)*DGain)*0.;

    past_diff = diff;
    I_diff+=diff;
    if(I_diff > I_MAX) I_diff = I_MAX;
    else if(I_diff < -I_MAX) I_diff = -I_MAX;

    ControlMotor(duty_r,duty_l);
}

//ライントレースの処理
void linetrace(){
    while(1){
        running.acquire();
        FetchLineSensor();
        pid();
    }
}

//ログ出力処理
void showinfo(){
    while(1){
        printf("%d\t%d\t%d\t%d\r\n",line_r2,line_r1,line_l1,line_l2);
        ThisThread::sleep_for(250);
    }
}

void flip1ms(){
    running.release();
}

int main(){
    motor_is_disable = false;
    speed = 0.0;

    sw.mode(PullNone);

    //モータの設定
    MOT1_IN1.period_us(100);
    MOT1_IN2.period_us(100);
    MOT2_IN1.period_us(100);
    MOT2_IN2.period_us(100);

    printf("Initialize Complete!\r\n");

    while(sw);

    osStatus st_print = print_info.start(showinfo);
    osStatus st_runpid = run_pid.start(linetrace);
    loop1ms.attach_us(&flip1ms,1000);

    while(true){
        ThisThread::sleep_for(1);
    }

  }