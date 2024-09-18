#include "mbed.h"
#include <UCL_Encoder.h>


PwmOut motor2_PWM(D4);              // motor PWM
DigitalOut motor2_dir(D7);          // motor direction
DigitalOut Enable_Motor_Shield(A0); // pin for enable (1) or disable (0)
float motor2_voltage = 0.0;         // motor voltage, between 0.0f and 1.0f

DigitalOut led2(LED2);
DigitalIn user_btn(BUTTON1);

Timer printing_timer;
int gear_ratio = 19;  // motor gear ratio is 19:1
int motor2_CPT = 3;   // CPT of motor1 encoder
int motor2_count = 0; // motor encoder count
float motor2_rot = 0; // number of motor rotations
float pos_error = 0;

int main()
{
    Enable_Motor_Shield.write(1); // enable motor shield
    motor2_PWM.period_us(50);     // set PWM pulse period of 50 us
    

    QuadratureEncoder encoder(PB_9, PA_5);
    encoder.Initialise();
    printing_timer.start(); // start the timer for printing

    float set_point_counter = 30; // desired motor position (rotations)
    float Kp = 0.01;    // P-gain
    float Ki = 0.004;   // I-gain
    float integral = 0; // integrated position error

    while (user_btn == 0)
    {
        motor2_count = encoder.getCount(); // read encoder
        // calculate the number of motor rotations
        // do you understand this formula?
        motor2_rot = motor2_count / motor2_CPT / 4.0 / gear_ratio;
        if (printing_timer.read_ms() > 200)
        {
            printing_timer.reset();
            printf("Count: %d %d \n", motor2_count, (int)(motor2_rot*100));
        }
        // The simplest on-off control
        if (motor2_count < set_point_counter)
            motor2_voltage = 1; // set motor voltage
        else
            motor2_voltage = 0.0;
        // P-control
        // find position error
        pos_error = set_point_counter- motor2_count;
        motor2_voltage = Kp * pos_error; // P-control formula
        // PI-control
        integral += pos_error * 0.001;                   // the simplest integration
        motor2_voltage = Kp * pos_error + Ki * integral; // PI-control
                                                         // formula

        if (motor2_voltage > 0)
            motor2_dir = 0;
        else
            motor2_dir = 1;
        // apply the voltage to the motor
        motor2_PWM.write(abs(motor2_voltage));
        thread_sleep_for(1); // sleep very briefly
    }
    motor2_PWM.write(0); // stop the motor
    printing_timer.stop();
    thread_sleep_for(200); // sleep briefly
}
