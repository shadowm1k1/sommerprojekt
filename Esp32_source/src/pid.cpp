#include "pid.h"

void Pid::initPid(int right_prop_val, int left_prop_val, int min_pwm, int max_pwm) // werte festlegen, für setup
{
    right_prop.attach(right_prop_val); // verbinden des rechten motors pin 3 ...
    left_prop.attach(left_prop_val);   // verbinden des linken  motors pin 5 ...
    pid_p = 0;
    pid_i = 0;
    pid_d = 0;
    /////////////////PID CONSTANTS/////////////////
    kp = 4;    // 3.55 --> example value
    ki = 0.01; // 0.003 --> example value, normally very small
    kd = 1.5;  // 2.05 --> example value
    ///////////////////////////////////////////////
    throttle = 1000;
    desired_angle = 0;
    time = millis();          // zeit in millis hochzählen
    left_prop.write(min_pwm); // die kleinst möglichen values fürs erste übergeben für esc ...
    right_prop.write(min_pwm);
    Serial.print(min_pwm);
    delay(3000); //...
}

void Pid::updatePid(float Gyr_rawX, float Gyr_rawY, float Gyr_rawZ, float Acc_rawX, float Acc_rawY, float Acc_rawZ) // winkel und error ausrechnen
{
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
    to implement the PID with. That means that the x axis of the IMU has to be paralel to
    the balance*/

    /*First calculate the error between the desired angle and
     *the real measured angle*/
    error = Total_angle[1] - desired_angle;
    Serial.println(error);

    /*Next the proportional value of the PID is just a proportional constant
     *multiplied by the error*/

    pid_p = kp * error;

    /*The integral part should only act if we are close to the
    desired position but we want to fine tune the error. That's
    why I've made a if operation for an error between -2 and 2 degree.
    To integrate we just sum the previous integral value with the
    error multiplied by  the integral constant. This will integrate (increase)
    the value each loop till we reach the 0 point*/
    if (-3 < error < 3)
    {
        pid_i = pid_i + (ki * error);
    }

    /*The last part is the derivate. The derivate acts upon the speed of the error.
    As we know the speed is the amount of error that produced in a certain amount of
    time divided by that time. For taht we will use a variable called previous_error.
    We substract that value from the actual error and divide all by the elapsed time.
    Finnaly we multiply the result by the derivate constant*/

    pid_d = kd * ((error - previous_error) / elapsedTime);

    /*The final PID values is the sum of each of this 3 parts*/
    PID = pid_p + pid_i + pid_d;

    /*We know taht the min value of PWM signal is 1000us and the max is 2000. So that
    tells us that the PID value can/s oscilate more than -1000 and 1000 because when we
    have a value of 2000us the maximum value taht we could sybstract is 1000 and when
    we have a value of 1000us for the PWM sihnal, the maximum value that we could add is 1000
    to reach the maximum 2000us*/
    if (PID < -250)
    {
        PID = -250;
    }
    if (PID > 250)
    {
        PID = 250;
    }

    /*Finnaly we calculate the PWM width. We sum the desired throttle and the PID value*/
    pwmLeft = throttle + PID;
    pwmRight = throttle - PID;

    /*Once again we map the PWM values to be sure that we won't pass the min
    and max values. Yes, we've already maped the PID values. But for example, for
    throttle value of 1300, if we sum the max PID value we would have 2300us and
    that will mess up the ESC.*/
    // Right
    if (pwmRight < 750)
    {
        pwmRight = 750;
    }
    if (pwmRight > 1250)
    {
        pwmRight = 1250;
    }
    // Left
    if (pwmLeft < 750)
    {
        pwmLeft = 750;
    }
    if (pwmLeft > 1250)
    {
        pwmLeft = 1250;
    }
    /*Finnaly using the servo function we create the PWM pulses with the calculated
    width for each pulse*/
    left_prop.write(pwmLeft);
    right_prop.write(pwmRight);

    previous_error = error; // Remember to store the previous error.
} // end of loop void
