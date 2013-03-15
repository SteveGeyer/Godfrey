// Godfrey IMU code
//
// ---------------------------------------------------------------------------
//
// Copyright 2013 Steve Geyer
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// ---------------------------------------------------------------------------
//
// This code is designed for the Sparkfun 9 Degrees of Freedom - Razor IMU
// product (SEN-10736 / https://www.sparkfun.com/products/10736). I chose
// this device because I had one when I started building Godfrey. However I
// don't think the magnetometer will work between two large electric motors
// and frankly the CPU is unnecessary because does nothing of interest other
// that normalize for endianness.
//
// This code very simple, it initializes the accelerator and gyroscope chips
// and then queries their values at 50 times a second and outputs the
// results. The values are output in the order accelerator X,Y,Z followed by
// gyroscope X,Y,Z. All values are signed 16 bit two's complement quantities.
//
// To burn this code into the device I used the FTDI Basic Breakout - 3.3V
// (DEV-09873 / https://www.sparkfun.com/products/9873) connecting the IMU
// to a serial port. Set the Arduino IDE device (>Tools>Board) to 'Arduino
// Pro or Pro Mini (3.3V, 8 MHz) w/ ATmega 328'.
//
// I had a problem programming my device with the FTDI Basic Breakout. I
// would hit upload in the IDE and it would hang or I would receive an error
// message. I discovered (through web searching and some experimentation)
// that I could program the device if I started the upload and then hit the
// reset button on the IMU.
//
// Use FTDI Basic Breakout and connect the device to a serial port and watch
// the values as you move the device. Set your terminal program to 57600
// baud to see the results.
//
// This code currently outputs data at 50Hz and 57600 baud. Both these may
// be too slow for the operational system.

#include <Wire.h>

#define ACCEL_ADDRESS ((int) 0x53)
#define GYRO_ADDRESS  ((int) 0x68)

// 50 Hz.
#define UPDATE_MS (1000/50)

unsigned long next_reading;

void setup()
{
    Serial.begin(57600);
    // Wait for serial port to connect. Needed for Leonardo only.
    while (!Serial)
        ;

    // Give sensors enough time to start
    delay(50);
    Wire.begin();

    // Initialize accelerator.
    // Measurement mode.
    Wire.beginTransmission(ACCEL_ADDRESS);
    Wire.write((byte)0x2D);
    Wire.write((byte)0x08);
    Wire.endTransmission();
    delay(5);
    // Set to full resolution
    Wire.beginTransmission(ACCEL_ADDRESS);
    Wire.write((byte)0x31);
    Wire.write((byte)0x08);              
    Wire.endTransmission();
    delay(5);
    // Set to 50Hz, normal operation
    Wire.beginTransmission(ACCEL_ADDRESS);
    Wire.write((byte)0x2C);
    Wire.write((byte)0x09);
    Wire.endTransmission();
    delay(5);

    // Initialise gyroscope.
    // Power up reset defaults
    Wire.beginTransmission(GYRO_ADDRESS);
    Wire.write((byte)0x3E);
    Wire.write((byte)0x80);
    Wire.endTransmission();
    delay(5);
    // LP filter bandwidth to 42Hz
    // Select full-scale range of the gyro sensors
    // DLPF_CFG = 3, FS_SEL = 3.
    Wire.beginTransmission(GYRO_ADDRESS);
    Wire.write((byte)0x16);
    Wire.write((byte)0x1B);
    Wire.endTransmission();
    delay(5);
    // Set sample rato to 50Hz / SMPLRT_DIV = 10 (50Hz)
    Wire.beginTransmission(GYRO_ADDRESS);
    Wire.write((byte)0x15);
    Wire.write((byte)0x0A);
    Wire.endTransmission();
    delay(5);
    // Internal oscillator. Clear reset.
    Wire.beginTransmission(GYRO_ADDRESS);
    Wire.write((byte)0x3E);
    Wire.write((byte)0x00);
    Wire.endTransmission();
    delay(5);

    next_reading = millis() + UPDATE_MS;
}

void output_int(int value)
{
    Serial.print(value, DEC);
    Serial.print(" ");
}

void loop()
{
    unsigned long now = millis();
    if (now < next_reading)
        return;
    next_reading = now + UPDATE_MS;

    byte accelerator[6];
    byte gyroscope[6];

    // Read six bytes from the accelerator starting at x accelaration.
    // This chip is little-endian.
    Wire.beginTransmission(ACCEL_ADDRESS);
    Wire.write((byte)0x32);
    Wire.endTransmission();
    Wire.beginTransmission(ACCEL_ADDRESS);
    Wire.requestFrom(ACCEL_ADDRESS, 6); // Request 6 bytes
    int a_i = 0;
    while (Wire.available())
    { 
        accelerator[a_i] = Wire.read();
        a_i++;
    }
    Wire.endTransmission();

    // Read six bytes from the gyroscope starting at x angular change.
    // This chip is big-endian.
    Wire.beginTransmission(GYRO_ADDRESS);
    Wire.write((byte)0x1D);
    Wire.endTransmission();
    Wire.beginTransmission(GYRO_ADDRESS);
    Wire.requestFrom(GYRO_ADDRESS, 6); // Request 6 bytes
    int g_i = 0;
    while (Wire.available())
    { 
        gyroscope[g_i] = Wire.read();
        g_i++;
    }
    Wire.endTransmission();
  
    if ((a_i != 6) || (g_i != 6))
        Serial.println("?E1 - IMU had bad input");
    else
    {
        output_int((((int)accelerator[1])<<8)+accelerator[0]);
        output_int((((int)accelerator[3])<<8)+accelerator[2]);
        output_int((((int)accelerator[5])<<8)+accelerator[4]);
        output_int((((int)gyroscope[0])<<8)+gyroscope[1]);
        output_int((((int)gyroscope[2])<<8)+gyroscope[3]);
        output_int((((int)gyroscope[4])<<8)+gyroscope[5]);
        Serial.println();
    }
}
