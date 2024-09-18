#include "mbed.h"
#include <UCL_Encoder.h>

DigitalIn bIn1(D15); // initialise hall effector pin for digital read

PwmOut motor1_PWM(D5);              // motor PWM
DigitalOut motor1_dir(D6);          // motor direction
DigitalOut Enable_Motor_Shield(A0); // pin for enable (1) or disable (0)
float motor1_voltage = 0.0;         // motor voltage, between 0.0f and 1.0f

DigitalOut led2(LED2);
DigitalIn user_btn(BUTTON1);

Timer printing_timer;
int gear_ratio = 19;  // motor gear ratio is 19:1
int motor1_CPT = 3;   // CPT of motor1 encoder
int motor1_count = 0; // motor encoder count
float motor1_rot = 0; // number of motor rotations
float pos_error = 0;

int main()
{
    bIn1.mode(PullUp); // initialise the pin with pull-down resistor
    Enable_Motor_Shield.write(1); // enable motor shield
    motor1_PWM.period_us(50);     // set PWM pulse period of 50 us
    printf("bIn1.read = %d\n", bIn1.read());
    while (bIn1.read() == 1)
    {
        motor1_voltage = 0.2; // set motor voltage
        // detect direction automatically
        if (motor1_voltage > 0)
            motor1_dir = 0;
        else
            motor1_dir = 1;
        // apply the voltage to the motor via motor driver
        motor1_PWM.write(abs(motor1_voltage));
        thread_sleep_for(1); // sleep very briefly
    }
    motor1_PWM.write(0); // stop the motor
    thread_sleep_for(3000); // sleep briefly

    

    QuadratureEncoder encoder(PA_6, PA_7);
    encoder.Initialise();
    printing_timer.start(); // start the timer for printing

    float set_point = 0.5; // desired motor position (rotations)
    float Kp = 0.01;    // P-gain
    float Ki = 0.004;   // I-gain
    float integral = 0; // integrated position error

    while (user_btn == 0)
    {
        motor1_count = -encoder.getCount(); // read encoder
        // calculate the number of motor rotations
        // do you understand this formula?
        motor1_rot = motor1_count / motor1_CPT / 4.0 / gear_ratio;
        if (printing_timer.read_ms() > 200)
        {
            printing_timer.reset();
            printf("Count: %d %d \n", motor1_count, (int)(motor1_rot*100));
        }
        // The simplest on-off control
        if (motor1_rot < set_point)
            motor1_voltage = 1; // set motor voltage
        else
            motor1_voltage = 0.0;
        // P-control
        // find position error
        pos_error = set_point * motor1_CPT * 4.0 * gear_ratio - motor1_count;
        motor1_voltage = Kp * pos_error; // P-control formula
        // PI-control
        integral += pos_error * 0.001;                   // the simplest integration
        motor1_voltage = Kp * pos_error + Ki * integral; // PI-control
                                                         // formula

        if (motor1_voltage > 0)
            motor1_dir = 1;
        else
            motor1_dir = 0;
        // apply the voltage to the motor
        motor1_PWM.write(abs(motor1_voltage));
        thread_sleep_for(1); // sleep very briefly
    }
    motor1_PWM.write(0); // stop the motor
    printing_timer.stop();
    thread_sleep_for(200); // sleep briefly
}
