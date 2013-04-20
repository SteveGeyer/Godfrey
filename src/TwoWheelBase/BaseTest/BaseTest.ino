// Godfrey Motor Base Test program
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
// This code controls Godfrey's wheels velocity and all the sensors around
// the wheels. The sensor data includes measuring the motors current drain,
// the shaft encoders, and the IMU data.
//
// This program reports all the measured data as ten signed decimal values
// in the format:
//   accelerator-x
//   accelerator-y
//   accelerator-z
//   gyroscope-x
//   gyroscope-y
//   gyroscope-z
//   left-motor-milliamps
//   right-motor-milliamps
//   left-rotary-count
//   right-rotary-count
//
// It accepts several commands:
//   m <left-motor> <right-motor> Set left and right motor speed. The values
//                                are decimal values between -400 and 400.
//                                Missing arguments default as zero.
//   f <report-divider>           Set the frequency of reports in multiples
//                                of 50Hz. It defaults to reporting every
//                                five seconds.
//   r                            Toggle status reports on and off.
//
// It can report several error conditions:
//   ?E1 - IMU had bad input
//   ?E2 - input line too long
//   ?E3 - left motors faults
//   ?E4 - right motors faults

#include <Arduino.h>

// Motor controller pins.
static const unsigned char M_EN1DIAG1 = 6;
static const unsigned char M_INA1     = 2;
static const unsigned char M_INB1     = 4;
static const unsigned char M_PWM1     = 9;
static const unsigned char M_CS1      = A0;
static const unsigned char M_CS2      = A1;
static const unsigned char M_PWM2     = 10;
static const unsigned char M_INB2     = 8;
static const unsigned char M_INA2     = 7;
static const unsigned char M_EN2DIAG2 = 12;

// Rotary encoder pins.
static const unsigned char M_ROTARY_LEFT_A  = 13;
static const unsigned char M_ROTARY_LEFT_B  = 11;
static const unsigned char M_ROTARY_RIGHT_A =  5;
static const unsigned char M_ROTARY_RIGHT_B =  3;

static void          motor_init(void);
static void          left_set_speed(int speed);
static unsigned int  left_milliamps(void);
static unsigned char left_fault(void);
static void          right_set_speed(int speed);
static unsigned int  right_milliamps(void);
static unsigned char right_fault(void);
static void          process_command_input(char* buffer);
static void          process_imu_input(char* buffer);
static void          rotary_encoder_init(void);
static void          rotary_encoder_update(void);

// Buffer management.

static const int LINE_MAX = 100;
typedef void (*process_line_t)(char* data);

typedef struct
{
    boolean scanning_for_newline;
    char    buffer[LINE_MAX+1];
    int     index;
    process_line_t process_line;
} buffer_t;

#define NUMBER_TO_AVERAGE 50

typedef struct
{
    int index;
    unsigned int values[NUMBER_TO_AVERAGE];
    unsigned int sum;
} ave_t;

static void ave_clear(ave_t* ave);
static unsigned int ave(ave_t* ave, unsigned int new_value);
static void buf_initialize(buffer_t* bufp, process_line_t process_line);
static void buf_add_char(buffer_t* bufp, char ch);

// Quadrature Encoder Matrix
static int g_qem[16] = { 0, -1,  1,  0,
                         1,  0,  0, -1,
                        -1,  0,  0,  1,
                         0,  1, -1,  0};


static boolean g_report_info = true;

// How frequently should we report information. This is a divider of the
// 50Hz messaging we get from the IMU.

static int g_report_divider = 5*50;
static int g_report_count = 0;

static buffer_t g_imu;
static buffer_t g_command_input;

static int g_left_rotary_count  = 0;
static int g_left_old_value     = 0;
static int g_right_rotary_count = 0;
static int g_right_old_value    = 0;

static ave_t g_left_ave_current;
static ave_t g_right_ave_current;

void setup()
{
    Serial.begin(115200);
    Serial1.begin(57600);

    while (!Serial)
       ;
    Serial.println("Base Control");
    g_report_info = true;
    motor_init();
    buf_initialize(&g_imu, process_imu_input);
    buf_initialize(&g_command_input, process_command_input);
    rotary_encoder_init();
    ave_clear(&g_left_ave_current);
    ave_clear(&g_right_ave_current);
}

void loop()
{
    // Process command input.
    while (Serial.available())
        buf_add_char(&g_command_input, Serial.read());

    // Process IMU input.
    while (Serial1.available())
        buf_add_char(&g_imu, Serial1.read());

    // Process rotary encoder.
    rotary_encoder_update();

    // Check motor status.
   if (right_fault())
       Serial.println("?E3 - left motor fault");
   if (left_fault())
       Serial.println("?E4 - right motor fault");
}

static void rotary_encoder_init(void)
{
    pinMode(M_ROTARY_LEFT_A, INPUT);
    pinMode(M_ROTARY_LEFT_B, INPUT);
    pinMode(M_ROTARY_RIGHT_A, INPUT);
    pinMode(M_ROTARY_RIGHT_B, INPUT);
}

static void rotary_encoder_update(void)
{
    int new_value;
    new_value = (digitalRead(M_ROTARY_LEFT_B) << 1) +
                 digitalRead(M_ROTARY_LEFT_A);
    g_left_rotary_count += g_qem[(g_left_old_value << 2) + new_value];
    g_left_old_value = new_value;

    new_value = (digitalRead(M_ROTARY_RIGHT_B) << 1) +
                 digitalRead(M_ROTARY_RIGHT_A);
    g_right_rotary_count += g_qem[(g_right_old_value << 2) + new_value];
    g_right_old_value = new_value;
}

static void process_command_input(char* buffer)
{
    char ch = buffer[0];
    if ((ch == 'm') || (ch == 'M'))
    {
        int left = 0;
        int right = 0;
        sscanf(&buffer[1], "%d %d", &left, &right);
        Serial.print("M left:");Serial.print(left);
        Serial.print(" right:");Serial.println(right);
        left_set_speed(left);
        right_set_speed(right);
    }
    else if ((ch == 'f') || (ch == 'F'))
    {
        sscanf(&buffer[1], "%d", &g_report_divider);
        Serial.println("Frequency: ");Serial.println(g_report_divider);
    }
    else if ((ch == 'r') || (ch == 'R'))
        g_report_info = !g_report_info;
}

static void process_imu_input(char* buffer)
{
    unsigned int left_ma = left_milliamps();
    unsigned int right_ma = right_milliamps();
    unsigned int left_ave = ave(&g_left_ave_current, left_ma);
    unsigned int right_ave = ave(&g_right_ave_current, right_ma);

    if (g_report_info)
    {
        if ((g_report_count++ % g_report_divider) == 0)
        {
            Serial.print(buffer);
            Serial.print(left_ave);
            Serial.print(" ");
            Serial.print(right_ave);
            Serial.print(" ");
            // Serial.print(g_left_rotary_count);
            // Serial.print(" ");
            // Serial.print(g_right_rotary_count);
            Serial.println();
            g_left_rotary_count = 0;
            g_right_rotary_count = 0;
        }
    }
}

static void report_line_length_error(void)
{
    Serial.println("?E2 - input line too long");
}


// ----------------------------------------------------------------------
// Buffer management.
// ----------------------------------------------------------------------

static void buf_initialize(buffer_t* bufp, process_line_t process_line)
{
    bufp->index = 0;
    bufp->scanning_for_newline = false;
    bufp->process_line = process_line;
}

static void buf_add_char(buffer_t* bufp, char ch)
{
    if (bufp->scanning_for_newline)
    {
        if (ch == '\n')
        {
            bufp->scanning_for_newline = false;
            bufp->index = 0;
        }
    }
    else if (bufp->index > LINE_MAX)
    {
        report_line_length_error();
        bufp->scanning_for_newline = true;
    }
    else if (ch == '\r')
    {
        // Skip returns.
    }
    else if (ch == '\n')
    {
        bufp->buffer[bufp->index] = '\0';
        bufp->process_line(bufp->buffer);
        bufp->index = 0;
    }
    else
    {
        bufp->buffer[bufp->index++] = ch;
    }
}


// ----------------------------------------------------------------------
// Motor Interface
// ----------------------------------------------------------------------

static void motor_init(void)
{
    // Define pinMode for the pins and set the frequency for timer1.
    pinMode(M_INA1, OUTPUT);
    pinMode(M_INB1, OUTPUT);
    pinMode(M_PWM1, OUTPUT);
    pinMode(M_EN1DIAG1, INPUT);
    pinMode(M_CS1, INPUT);
    pinMode(M_INA2, OUTPUT);
    pinMode(M_INB2, OUTPUT);
    pinMode(M_PWM2, OUTPUT);
    pinMode(M_EN2DIAG2, INPUT);
    pinMode(M_CS2, INPUT);

    // Setting up PWM logic.
    //   CS is 1 meaning prescaler is '1'.
    //   WGM is mode 8 or "PWM, Phase and Frequency Corrrect" with ICRn as top.
    //   We set ICR1 to 400 for top, therefore PWM frequency calculation is:
    //     16MHz / 1 (prescaler) / 2 (phase-correct) / 400 (top) = 20kHz
    //   COM1[ABC] is mode 2 or Clear OCn[ABC] on compare match upcounting
    //                          and set OCn[ABC] on copmare match downcounting.
    //   With these settings, OCR1[ABC] has a range from 0 to 400.
    TCCR1A = 0b10101000;
    TCCR1B = 0b00010001;
    ICR1 = 400;
}

static void left_set_speed(int speed)
{
    unsigned char reverse = 0;

    if (speed < 0)
    {
        speed = -speed;
        reverse = 1;
    }
    if (speed > 400)
        speed = 400;
    OCR1B = speed;

    if (speed == 0)
    {
        digitalWrite(M_INA2, LOW);
        digitalWrite(M_INB2, LOW);
    }
    else if (reverse)
    {
        digitalWrite(M_INA2, HIGH);
        digitalWrite(M_INB2, LOW);
    }
    else
    {
        digitalWrite(M_INA2, LOW);
        digitalWrite(M_INB2, HIGH);
    }
}

static unsigned int left_milliamps(void)
{
    // 5V / 1024 ADC counts / 130 mV per A = 38 mA per count
    return analogRead(M_CS2) * 38;
}

static unsigned char left_fault(void)
{
    return !digitalRead(M_EN2DIAG2);
}

static void right_set_speed(int speed)
{
    unsigned char reverse = 0;
  
    if (speed < 0)
    {
        speed = -speed;
        reverse = 1;
    }
    if (speed > 400)
        speed = 400;
    OCR1A = speed;
    if (speed == 0)
    {
        digitalWrite(M_INA1, LOW);
        digitalWrite(M_INB1, LOW);
    }
    else if (reverse)
    {
        digitalWrite(M_INA1, HIGH);
        digitalWrite(M_INB1, LOW);
    }
    else
    {
        digitalWrite(M_INA1, LOW);
        digitalWrite(M_INB1, HIGH);
    }
}

static unsigned int right_milliamps(void)
{
    // 5V / 1024 ADC counts / 130 mV per A = 38 mA per count
    return analogRead(M_CS1) * 38;
}

static unsigned char right_fault(void)
{
    return !digitalRead(M_EN1DIAG1);
}

static void ave_clear(ave_t* ave)
{
    int i;
    ave->index = 0;
    for (i = 0; i < NUMBER_TO_AVERAGE; i++)
        ave->values[i] = 0;
    ave->sum = 0;
}

static unsigned int ave(ave_t* ave, unsigned int new_value)
{
    ave->sum += new_value;
    ave->sum -= ave->values[ave->index];
    ave->values[ave->index] = new_value;
    ave->index = (ave->index + 1) % NUMBER_TO_AVERAGE;
    return ave->sum / NUMBER_TO_AVERAGE;
}
