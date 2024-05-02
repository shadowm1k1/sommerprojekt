#include <Arduino.h>
#include <ESP32Servo.h>

class Pid
{
public:
    void initPid(int right_prop_val, int left_prop_val, int right_propX_val, int left_propX_val, int min_pwm, int max_pwm);
    void updatePid(float Gyr_rawX, float Gyr_rawY, float Gyr_rawZ, float Acc_rawX, float Acc_rawY, float Acc_rawZ);
    Servo right_propY; // 2 Objects Servo deklaration --> pwm signal später
    Servo left_propY;
    Servo right_propX;
    Servo left_propX;
    int16_t Acc_rawX, Acc_rawY, Acc_rawZ, Gyr_rawX, Gyr_rawY, Gyr_rawZ; // fuer MPU 16bit value weil es 16bit hergibt
    float Acceleration_angle[2];
    float Gyro_angle[2];
    float Total_angle[2]; // für richtige winkel wird eine Formel benutzt in der gyro_ang und acceleration_ang benutzt wird --> 2 weil 2 motoren
    float elapsedTime, time, timePrev;
    int i;
    float rad_to_deg = 180 / 3.141592654; // konstante fuer weitere berechnungen
    float PIDY, pwmLeftY, pwmRightY, errorY, previous_errorY;
    float PIDX, pwmLeftX, pwmRightX, errorX, previous_errorX;
    float pid_pY;
    float pid_iY;
    float pid_dY;
    float pid_pX;
    float pid_iX;
    float pid_dX;
    /////////////////PID CONSTANTS/////////////////
    double kp; // 3.55 --> example value
    double ki; // 0.003 --> example value, normally very small
    double kd; // 2.05 --> example value
    ///////////////////////////////////////////////
    double throttle;     // initial wert fuer throttle von motoren am anfang
    float desired_angleY; // der angle wo es steady bleiben sollte
    float desired_angleX; //gleiche nur fuer X
};
