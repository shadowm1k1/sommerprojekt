#include "pid.h"
#include <iostream>

void Pid::initPid(int right_prop_val, int left_prop_val, int min_pwm, int max_pwm) // werte festlegen, für setup
{
    right_prop.attach(right_prop_val); // verbinden des rechten motors pin 3 ...
    left_prop.attach(left_prop_val);   // verbinden des linken  motors pin 5 ...
    pid_p = 0;
    pid_i = 0;
    pid_d = 0;
    /////////////////PIDY CONSTANTS/////////////////
    kp = 40;    // 3.55 --> example value+
    ki = 0.003; // 0.003 --> example value, normally very small
    kd = 4;     // 2.05 --> example value
    ///////////////////////////////////////////////
    throttle = 2000;
    desired_angle = 0;
    time = millis(); // zeit in millis hochzählen
                     // die kleinst möglichen values fürs erste übergeben für esc ...
    left_prop.write(min_pwm);
    right_prop.write(min_pwm);
    delay(3000);
}

void Pid::updatePid(float Gyr_rawX, float Gyr_rawY, float Gyr_rawZ, float Acc_rawX, float Acc_rawY, float Acc_rawZ) // winkel und errorY ausrechnen
{
    float starttime = millis();
    timePrev = time; // the previous time is stored before the actual time read
    time = millis(); // actual time read
    elapsedTime = (time - timePrev) / 1000;

    Gyr_rawX = Gyr_rawX * rad_to_deg;
    Gyr_rawY = Gyr_rawY * rad_to_deg;
    Gyr_rawZ = Gyr_rawZ * rad_to_deg;

    /*---X---*/
    Acceleration_angle[0] = atan((Acc_rawY / 16384.0) / sqrt(pow((Acc_rawX / 16384.0), 2) + pow((Acc_rawZ / 16384.0), 2))) * rad_to_deg;
    /*---Y---*/
    Acceleration_angle[1] = atan(-1 * (Acc_rawX / 16384.0) / sqrt(pow((Acc_rawY / 16384.0), 2) + pow((Acc_rawZ / 16384.0), 2))) * rad_to_deg;
    /*---X---*/
    Gyro_angle[0] = Gyr_rawX / 16.4;
    /*---Y---*/
    Gyro_angle[1] = Gyr_rawY / 16.4;

    /*Now in order to obtain degrees we have to multiply the degree/seconds
     *value by the elapsedTime.*/
    /*Finnaly we can apply the final filter where we add the acceleration
     *part that afects the angles and ofcourse multiply by 0.98 */

    /*---X axis angle---*/
    Total_angle[0] = 0.98 * (Total_angle[0] + Gyro_angle[0] * elapsedTime) + 0.02 * Acceleration_angle[0];
    /*---Y axis angle---*/
    Total_angle[1] = 0.98 * (Total_angle[1] + Gyro_angle[1] * elapsedTime) + 0.02 * Acceleration_angle[1];

    /*///////////////////////////P I D///////////////////////////////////*/
    /*Remember that for the balance we will use just one axis. I've choose the x angle
    to implement the PIDY with. That means that the x axis of the IMU has to be paralel to
    the balance*/

    /*First calculate the errorY between the desired angle and
     *the real measured angle*/

    error = Total_angle[1] - desired_angle;
    // Serial.println(errorY);

    /*Next the proportional value of the PIDY is just a proportional constant
     *multiplied by the errorY*/
    pid_p = kp * error;

    /*The integral part should only act if we are close to the
    desired position but we want to fine tune the errorY. That's
    why I've made a if operation for an errorY between -2 and 2 degree.
    To integrate we just sum the previous integral value with the
    errorY multiplied by  the integral constant. This will integrate (increase)
    the value each loop till we reach the 0 point*/
    if (-3 < error < 3)
    {
        pid_i = pid_i + (ki * error);
    }

    /*The last part is the derivate. The derivate acts upon the speed of the errorY.
    As we know the speed is the amount of errorY that produced in a certain amount of
    time divided by that time. For taht we will use a variable called previous_errorY.
    We substract that value from the actual errorY and divide all by the elapsed time.
    Finnaly we multiply the result by the derivate constant*/

    pid_d = kd * ((error - previous_error) / elapsedTime);

    /*The final PIDY values is the sum of each of this 3 parts*/
    PID = pid_p + pid_i + pid_d;

    /*We know taht the min value of PWM signal is 1000us and the max is 2000. So that
    tells us that the PIDY value can/s oscilate more than -1000 and 1000 because when we
    have a value of 2000us the maximum value taht we could sybstract is 1000 and when
    we have a value of 1000us for the PWM sihnal, the maximum value that we could add is 1000
    to reach the maximum 2000us*/
    if (PID < -500)
    {
        PID = -500;
    }
    if (PID > 500)
    {
        PID = 500;
    }

    /*Finnaly we calculate the PWM width. We sum the desired throttle and the PIDY value*/
    pwmLeft = throttle + PID;  // muss schauen ob der richtige Motor erwischt wurde
    pwmRight = throttle - PID; // muss schauen ob -||-

    /*Once again we map the PWM values to be sure that we won't pass the min
    and max values. Yes, we've already maped the PIDY values. But for example, for
    throttle value of 1300, if we sum the max PIDY value we would have 2300us and
    that will mess up the ESC.*/
    // Right

    // Right
    if (pwmRight < 1000)
    {
        pwmRight = 1000;
    }
    if (pwmRight > 2000)
    {
        pwmRight = 2000;
    }
    // Left
    if (pwmLeft < 1000)
    {
        pwmLeft = 1000;
    }
    if (pwmLeft > 2000)
    {
        pwmLeft = 2000;
    }
    /*Finnaly using the servo function we create the PWM pulses with the calculated
    width for each pulse*/
    left_prop.write(pwmLeft);
    right_prop.write(pwmRight);

    // Remember to store the previous errorY.
    previous_error = error; // Same hier
} // end of loop void
