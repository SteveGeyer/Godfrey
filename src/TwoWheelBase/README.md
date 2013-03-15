![Motor Base](https://github.com/SteveGeyer/Godfrey/blob/master/image/BasicMotorBase.png?raw=true)

Godfrey's phase I motor base is a two-wheel balancing design. It contains all the elements necessary to detect the robot's position and balance and move the robot in its environment.

At its heart is an Arduino Leonardo board. Connected to this board's serial port is a 9 Degrees of Freedom IMU programmed to continuously report the accelerator and gyroscope data. Also connected to the the Arduino through PWM lines is a two channel motor driver which itself is connected to the drive motors. The motor current reporting component of the motor drivers are feed back to two Arduino analog ports. The motors have built in rotary encoders and their signals are feed to the Arduino.

With all of this hardware, the base can:

1. It can move with left and right wheel independently in both the forward and backwards direction.
2. It accurately measures wheel rotation. This is used to accurately determine the ground path. It also helps balance the natural variations in motor speed. 
3. The base uses it gyroscopes to determine how fast it is pitching in any direction. This is use to balance the robot.
4. It measures acceleration in all directions. Its primary use is to determine when Godfrey is upright. The gyroscopes are the primary actors in balancing, but this drift. The accelerometer measure down in a stable manner.
5. It is able to communicate with other computer elements through its serial port.
6. It measures the current usage of each motor. I am not sure I will use this.

The schematic for the two wheel base can be found [here](http://upverter.com/stevegeyer/1aa19b3adfc781d9/GodfreyTwoWheelBase/). 

The IMU program initializes the IMU hardware and once done continuously reports the accelerator and gyroscope values.

The BaseTest program exercises and tests the hardware.


Parts
=====

| Item | Vendor | Num | Price | Total |
| ---- | ------ | --- | ----- | ----- |
| Arduino Leonardo (with headers) | Sparkfun DEV-11286 | 1 | $24.95 | $24.95 |
| 9 Degrees of Freedom - Razor IMU | Sparkfun SEN-10736 | 1 | $124.95 | $124.95 |
| Dual VNH2SP30 Motor Driver Carrier MD03A | Pololu #708 | 1 | $59.95 | $59.95 |
| 37D mm Metal Gearmotor Bracket Pair | Pololu #1084 | 1 | $23.85 | $23.85 |
| 29:1 Metal Gearmotor 37Dx52L mm with 64 CPR Encoder 12V 365rpm | Amazon | 2 | $24.50 | $49.00 |
| BaneBots Hub, Hex, Series 40, Set Screw, 6mm Bore, 2 Wide | BaneBots | 2 | $4.50 | $9.00 |
| BaneBots Wheel, 4-7 / 8" x 0.8", 1 / 2" Hex Mount, 50A, Black / Blue | BaneBots | 2 | $6.80 | $13.60 |

For a total of $305.30


Lessons Learned
===============

1. The motors have considerable variation in rotational speed for the same input control signals. This necessitates the use of the rotary encoders along with some software controller to map desires speeds into actual ones.
2. I could have probably used a simple hardware design. Instead of starting with the Arduino with Dual VNH2SP30 Motor Driver Carrier MD03A and a smart IMU, I could have used a Sparkfun Monster Moto Shield DEV-10182 and the Sparkfun 9 Degrees of Freedom - Sensor Stick SEN-10724. It would have been cheaper and easier to package.
