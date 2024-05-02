#include "pid.h"

void Pid::initPid(int right_propY_val, int left_propY_val, int right_propX_val, int left_propX_val, int min_pwm, int max_pwm) // werte festlegen, für setup
{
    right_propY.attach(right_propY_val); // verbinden des rechten motors pin 3 ...
    left_propY.attach(left_propY_val);   // verbinden des linken  motors pin 5 ...
    right_propX.attach(right_propX_val); // verbinden des rechten motors pin 3 ...
    left_propX.attach(left_propX_val);   // verbinden des linken  motors pin 5 ...
    pid_pY = 0;
    pid_iY = 0;
    pid_dY = 0;
    pid_pX = 0;
    pid_iX = 0;
    pid_dX = 0;
    /////////////////PIDY CONSTANTS/////////////////
    kp = 3.55;  // 3.55 --> example value
    ki = 0.001; // 0.003 --> example value, normally very small
    kd = 2.05;  // 2.05 --> example value
    ///////////////////////////////////////////////
    throttle = 1050;
    desired_angleY = 0;
    desired_angleX = 0;
    time = millis();          // zeit in millis hochzählen
    left_propY.write(min_pwm); // die kleinst möglichen values fürs erste übergeben für esc ...
    right_propY.write(min_pwm);
    left_propX.write(min_pwm);
    right_propX.write(min_pwm);
    delay(3000); //...
}

void Pid::updatePid(float Gyr_rawX, float Gyr_rawY, float Gyr_rawZ, float Acc_rawX, float Acc_rawY, float Acc_rawZ) // winkel und errorY ausrechnen
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
    to implement the PIDY with. That means that the x axis of the IMU has to be paralel to
    the balance*/

    /*First calculate the errorY between the desired angle and
     *the real measured angle*/

    errorY = Total_angle[1] - desired_angleY;
    errorX = Total_angle[0] - desired_angleX;
    // Serial.println(errorY);

    /*Next the proportional value of the PIDY is just a proportional constant
     *multiplied by the errorY*/

    pid_pY = kp * errorY;
    pid_pX = kp * errorX;

    /*The integral part should only act if we are close to the
    desired position but we want to fine tune the errorY. That's
    why I've made a if operation for an errorY between -2 and 2 degree.
    To integrate we just sum the previous integral value with the
    errorY multiplied by  the integral constant. This will integrate (increase)
    the value each loop till we reach the 0 point*/
    if (-3 < errorY < 3)
    {
        pid_iY = pid_iY + (ki * errorY);
        pid_iX = pid_iX + (ki* errorX);
    }

    /*The last part is the derivate. The derivate acts upon the speed of the errorY.
    As we know the speed is the amount of errorY that produced in a certain amount of
    time divided by that time. For taht we will use a variable called previous_errorY.
    We substract that value from the actual errorY and divide all by the elapsed time.
    Finnaly we multiply the result by the derivate constant*/

    pid_dY = kd * ((errorY - previous_errorY) / elapsedTime);
    pid_dX = kd * ((errorX - previous_errorX) / elapsedTime);

    /*The final PIDY values is the sum of each of this 3 parts*/
    PIDY = pid_pY + pid_iY + pid_dY;
    PIDX = pid_pX + pid_iX + pid_dX;

    /*We know taht the min value of PWM signal is 1000us and the max is 2000. So that
    tells us that the PIDY value can/s oscilate more than -1000 and 1000 because when we
    have a value of 2000us the maximum value taht we could sybstract is 1000 and when
    we have a value of 1000us for the PWM sihnal, the maximum value that we could add is 1000
    to reach the maximum 2000us*/
    if (PIDY < -200)
    {
        PIDY = -200;
    }
    if (PIDY > 200)
    {
        PIDY = 200;
    }
    if (PIDX < -200)
    {
        PIDX = -200;
    }
    if (PIDX > 200)
    {
        PIDX = 200;
    }

    /*Finnaly we calculate the PWM width. We sum the desired throttle and the PIDY value*/
    pwmLeftY = throttle + PIDY;
    pwmRightY = throttle - PIDY;
    pwmLeftX = throttle + PIDX; //muss schauen ob der richtige Motor erwischt wurde
    pwmRightX = throttle + PIDX; //muss schauen ob -||-

    /*Once again we map the PWM values to be sure that we won't pass the min
    and max values. Yes, we've already maped the PIDY values. But for example, for
    throttle value of 1300, if we sum the max PIDY value we would have 2300us and
    that will mess up the ESC.*/
    // Right
    if (pwmRightY < 850)
    {
        pwmRightY = 850;
    }
    if (pwmRightY > 1250)
    {
        pwmRightY = 1250;
    }
    // Left
    if (pwmLeftY < 850)
    {
        pwmLeftY = 850;
    }
    if (pwmLeftY > 1250)
    {
        pwmLeftY = 1250;
    }
    // Right
    if (pwmRightX < 850)
    {
        pwmRightX = 850;
    }
    if (pwmRightX > 1250)
    {
        pwmRightX = 1250;
    }
    // Left
    if (pwmLeftX < 850)
    {
        pwmLeftX = 850;
    }
    if (pwmLeftX > 1250)
    {
        pwmLeftX = 1250;
    }
    /*Finnaly using the servo function we create the PWM pulses with the calculated
    width for each pulse*/
    left_propY.write(pwmLeftY);
    right_propY.write(pwmRightY);
    left_propX.write(pwmLeftX);
    right_propX.write(pwmRightX);

    Serial.print("LeftY: ");
    Serial.print(pwmLeftY);
    Serial.print(" RightY: ");
    Serial.print(pwmRightY);
    Serial.print(" LeftX: ");
    Serial.print(pwmLeftX);
    Serial.print(" RightX: ");
    Serial.println(pwmRightX);

    previous_errorY = errorY; // Remember to store the previous errorY.
    previous_errorX = errorX; //Same hier
} // end of loop void
