#include <Arduino.h>
#include <ESP32Servo.h>

class Pid
{
public:
    void initPid(int right_prop_val, int left_prop_val, int min_pwm, int max_pwm);
    void updatePid(float Gyr_rawX, float Gyr_rawY, float Gyr_rawZ, float Acc_rawX, float Acc_rawY, float Acc_rawZ);
    Servo right_prop;
    Servo left_prop;
    int16_t Acc_rawX, Acc_rawY, Acc_rawZ, Gyr_rawX, Gyr_rawY, Gyr_rawZ; // fuer MPU 16bit value weil es 16bit hergibt
    float Acceleration_angle[2];
    float Gyro_angle[2];
    float Total_angle[2]; // für richtige winkel wird eine Formel benutzt in der gyro_ang und acceleration_ang benutzt wird --> 2 weil 2 motoren
    float elapsedTime, time, timePrev;
    int i;
    float rad_to_deg = 180 / 3.141592654; // konstante fuer weitere berechnungen
    float PID, pwmLeft, pwmRight, error, previous_error;
    float pid_p;
    float pid_i;
    float pid_d;
    /////////////////PID CONSTANTS/////////////////
    double kp; // 3.55 --> example value
    double ki; // 0.003 --> example value, normally very small
    double kd; // 2.05 --> example value
    ///////////////////////////////////////////////
    double throttle; // initial wert fuer throttle von motoren am anfang
    double throttleRight;
    // der angle wo es steady bleiben sollte
    float desired_angle; // gleiche nur fuer X
};
