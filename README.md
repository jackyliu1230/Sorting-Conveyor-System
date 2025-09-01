In this project, we will design a mechanical sorting system. The sorting system is expected to detect and classify four different kinds of materials by sensors and actuators. Those four materials include Aluminum, Steel, Black plastic and White plastic. To make the sorting system work properly, it is required to know circuit wiring, the functions of ADC (Analog-to-Digital Converter), and different kinds of interrupts (rising edge and falling edge), PWM (Pulse Width Modulation), stepper motor, and DC motor. Furthermore, an embedded programming technique is required, including consideration of stepper motor hissing effects and sensor debouncing.

Algorithm of the main structure:
Firstly, I would talk about the main structure of our algorithm. To begin with, since we are designing an embedded program. The first thing done in the main program is the initialization. The initialization includes all the hardware settings, like the system clock, all the interrupts, which are OR sensor, Ex sensor, the Pause button and the Ramp-down button. and the configuration of ADC. After that, the program would go into a “while loop” statement. In the while loop statement, the system would keep checking three flags to monitor if the Pause/Resume button and the ramp down button are pressed. Or the adc_flag is triggered, and the system will compare the “min” value of the ADC value to adjust what kind of objects are scanned.

Data structure for storing elements:
For our data storage structure, we choose to use “array” to store the “min” value, which stands for different kinds of objects. After one “min” value is achieved in the period of the “OR” sensor, it will be added to the next position of the array, and there is no limit on the number of arrays. This progress will happen in the ADC interrupt.

Stepper motor:
After the “min” value is stored in the array, it can be used to calculate the rotate_check number, which is used to decide how the stepper motor will rotate. We set two parameters, which are “curpos” and “furpos,” and the rotate_check will be their difference. In this case, both “curpos” and “furpos” will be the value stored in the array; “curpos” is the current position and “furpos” is the future position. And there are 4 numbers for positions, which are 0,1,2,3, representing black, aluminum, white and steel, and when the difference between two positions is 0, the stepper motor will not move; when the difference is -2, and 2, the stepper motor will rotate 180 degrees; when the difference is 1 and -3, the stepper motor will rotate 90 degrees clockwise, and when the difference is -1 and 3, the stepper motor will rotate 90 degrees counterclockwise. And this calculation will happen in the EX interrupt, after the EX sensor detects the target, PWM will be stopped and the stepper motor will rotate as the calculation result.

Overall, the main logic of our program is that using an array to store the “min” value measured by the “IR” and “OR” sensor and calculate the parameter rotate_check to decide how the stepper motor will rotate.
System Performance Specification

There are 3 major parts to determine the system performance. The parameters include the PWM, ADC sensor reflectiveness, the acceleration-deceleration profile and DC motor phase windings.

Firstly, the PWM is a periodic square signal which controls the speed of the DC motor. If the duty cycle (in percentage) is large in a PWM period, the DC motor would have a high velocity. However, if the DC motor runs too fast, the inertia at the end of the conveyor belt would be too large. As a result, the object may fall into the wrong sorting tray. This requires coordination between both the stepper motor and DC motor. 

Based on our experiment, when we set the duty cycle to 125/255, the classification correct rates decrease. When we choose the duty cycle to be 85/255, the classification of materials is safe. The reason is lower speed can prevent the system from being interrupted by the sensors and lower the inertia of the conveyor belt. 
Secondly, ADC reflectiveness parameters, which are the main part of identifying the materials. When the material passes through the OR sensor it triggers the IR sensor, which connects to ADC, and the IR sensor will read through the object's surface area. However, the largest value in the data obtained from different materials might be the same, so we just want to store the minimum reads from the object by resetting the minimum value in the ADC.

The range of the material is listed below:
Aluminum: Aluminum <= 300
Steel: 400 <= Steel <= 800
White Plastic: 830 <= White <= 963
Black Plastic: 964 <= Black <= 1000

The last important parameter is the acceleration, which determines the speed of the sorting system. In our project, we use an array to adjust the delay function in each rotation step. And we are using a two-phase stepper motor to increase the rotation speed. For example, when we want the stepper to turn 90 degrees, which means it needs to turn 50 steps because each step rotates 1.8 degrees. To make the stepper accelerate, we decrease the delay time for each step, but we found that the delay time cannot be very small. Otherwise, the motor will not work.

Testing and Calibration Procedure
In order to make the testing and calibration procedure efficient, we separate a few parts to test on different days.

1 st milestone:
● Make sure the hall effect sensor works properly to initialize the stepper motor

● Make sure the PWM is working to adjust the speed of the DC motor

● Make sure the ADC setting is correct and able to read all the materials

● Make sure the LCD can display the data we want from the code

● All the interrupts are configured in the correct way

2 nd milestone:
● Make an array to store the reading of materials
● Make sure the pause-resume button is implemented
● Make sure the sensors can communicate and cooperate

3rd milestone:
● The synchronization of the whole project
● Ramp down implemented
● System can sort materials correctly
● Optimize the system (improve the speed and the accuracy of the sorting system)

The benefit of separating different tasks in the project is to minimize errors and fix errors more efficiently. For example, before making the array to store the reading of materials in the 2nd milestone, we have to make sure the ADC is working, so that it can read and reset the minimum value every time, so that when the other material has a higher value than the previous materials, the reading will not be lost. And to decide the range of materials to read, we have to make sure the LCD is displaying the materials read by the IR sensor.
To ensure the subsections of the system are accurate, there are two main debugging techniques: using LEDs and LCDs. For example, when we want to test if the EX sensor
is implemented correctly, we can set the LED to light up when the EX sensor is
triggered. On the other hand, the fastest way to test a sensor and the kind of sensor
interruption, we can use a multimeter to measure the voltage between the sensors. If it
changes from high (5V) to low (0V), it is a falling edge interrupt, otherwise, it is a rising
edge interrupt.
