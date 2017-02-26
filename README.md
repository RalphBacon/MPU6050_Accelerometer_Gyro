# MPU6050_Accelerometer_Gyro
Three axis and 6 Degrees of Freedom - and cheap too

A cheap MPU6050 Accelerometer and Gyro can determine its orientation very well, and could be useful for quadcopters, drones or other model aeroplanes, as well as simpler experiments such as a digital spirit level like the one I create here in my demo.

Whilst there is an excellent library by Jeff Rowberg that allows all the functionality possible with this device, the demo sketch doesn't use any, and relies on the raw data being manipulated to provide a stable and useful x-axis reading (in degrees) that could easily be extended to include the y- and z-axes too.

Just to build on previous sketches I'm using some neopixels here to provide the visual indication of how level the x-axis is, and with some jiggery pokery have managed to to get it to show the centre led when balanced with the leds to the left (or right) illuminated depending on how far off-balance it is.

I'll be interested whether anyone kind think of any practical uses over and above stabilising a quadcopter or making a spirit level.

-----------------------------------------------------
Here are the links you might need:
-----------------------------------------------------

eBay item GY-521 3 Axis Accelerometer Gyroscope Module 6 DOF Module MPU-6050 for Arduino
http://www.ebay.co.uk/itm/291194615968

Joop Brokking: MPU-6050 6dof IMU tutorial for auto-leveling quadcopters with Arduino source code
Website: http://www.brokking.net/imu.html (also contains links to his videos)
Part 1: https://www.youtube.com/watch?v=4BoIE8YQwM8
Part 2: https://www.youtube.com/watch?v=j-kE0AMEWy4

Sparkfun Guide to Accelerometers, Gyro and IMU (Inertial Measurement Unit)
https://www.sparkfun.com/pages/accel_gyro_guide

Arduino Playground on the MPU6050
http://playground.arduino.cc/Main/MPU-6050

Jeff Rowberg's i2cdevlib for advanced users
https://github.com/jrowberg/i2cdevlib

The Java IDE called "Processing v3.3"
https://processing.org/download/?processing

Instructions on how to set up "Processing v3.3" for the TeaPot (aeroplane) demo
http://www.c-sharpcorner.com/article/3d-visualisation-of-imu-motion-sensor-in-processor-ide/

InvenSense (manufacturer) web page on the MPU6050
https://www.invensense.com/products/motion-tracking/6-axis/mpu-6050/

If you like this video please give it a thumbs up, share it and if you're not already subscribed please do so :)

My channel is here:
------------------------------------------------------------------
https://www.youtube.com/c/RalphBacon
------------------------------------------------------------------ 
How can I remember this? Memory tip: "See" Ralph Bacon, geddit?
