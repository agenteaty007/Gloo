/*
Gloo
Version: 6

Author: Alberto Tam Yong
Partner: Stanley Chang
Date: 12/10/15

Teensy 3.1
TFT Color Display connected to SPI
2 Servo motors connected to
1 RGB LED connected to 3 pins

Raspberry Pi B+ connected to UART Serial3
Baudrate 115200
*/

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <Servo.h>

#define TFT_DC 9
#define TFT_CS 10

#define button1_pin 2
#define button2_pin 3

#define SERVO_BASE_PIN 5
#define SERVO_TILT_PIN 6

#define RGB_R 22
#define RGB_G 23
#define RGB_B 21

#define NORMAL 0
#define HAPPY 1
#define SAD 2
#define EXCITED 3

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
// If using the breakout, change pins as desired
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

Servo servo_base;
Servo servo_tilt;

int servo_base_pos = 90;
int servo_tilt_pos = 60;
int prevServo_base_pos = 90;
int prevServo_tilt_pos = 60;

int servo_base_pos_upper = 110;
int servo_base_pos_lower = 50;
int servo_tilt_pos_upper = 90;
int servo_tilt_pos_lower = 45;

//Video Size
int video_width = 160;
int video_height = 120;
int camera_x = 512;
int camera_y = 512;
int camera_h = 0;
int camera_t = 0;
int prev_camera_x = 512;
int prev_camera_y = 512;
int delta_camera_x = 0;
int delta_camera_y = 0;
int x_threshold = 5;
int y_threshold = 5;
boolean follow_camera = 0;
boolean camera_flag = 1;
boolean servo_flag = 1;
boolean debug_mode = 0;
boolean first_reading_taken = 0;

boolean activity = 0;
int activity_counter = 0;
int activity_interval = 300; // 15 seconds

boolean change_of_eyes = 0;
boolean change_of_mouth = 0;

int mood = NORMAL;
int prev_mood = NORMAL;

int blink_counter = 0;
int blink_interval = 50;

boolean breathing = 0;
int intensity_breath = 0;
int dir_breath = 1;

unsigned int count_last_time_seen = 0;

unsigned int x_proportion = video_width/20;
unsigned int y_proportion = video_height/10;

int color = 0;

int button1 = 1;
int button2 = 1;
int serial3_size;
char serial3_incoming[128];

//List of commands for serial Menu
char command_list[][16] =
{"help","init_rpi", "stop_rpi", "led", "lcd", "servo","login_rpi","cam_rpi","toggle_servo","face_rpi","shutdown_rpi","debug",'0'};
int command_size[] = {4, 8, 8, 3, 3, 5, 9, 7, 12, 8, 12, 5, 1};

char serial_incoming[128];

unsigned int led_manual_flag = 1;

int command_check(char data[128])
{
  unsigned int i = 0;
  Serial.println(data);
  while (command_list[0][i] != '0')
  {
    for (int j = 0; j < command_size[i] ; j++)
    {
      if (data[j] == command_list[i][j])
      {
        if (j == command_size[i] - 1)
        {
          return i;
        }
      }
      else
      {
        break;
      }
    }
    i++;
  }
}

//RGB Cycle Pattern
//Test module
void rgb_set(int r, int g, int b)
{
  color == 0 ? analogWrite(RGB_R, 255) : analogWrite(RGB_R, 0);
  color == 1 ? analogWrite(RGB_G, 255) : analogWrite(RGB_G, 0);
  color == 2 ? analogWrite(RGB_B, 255) : analogWrite(RGB_B, 0);
  color++;
  if (color >= 3)
    color = 0;
}

void rgb_color(int r, int g, int b)
{
  analogWrite(RGB_R,r);
  analogWrite(RGB_G,g);
  analogWrite(RGB_B,b);
}

void rgb_random()
{
  analogWrite(RGB_R,random(0,255));
  analogWrite(RGB_G,random(0,255));
  analogWrite(RGB_B,random(0,255));
}

//----- MENU FUNCTIONS -------

void help_function(char data[128])
{
  Serial.println(data);
  Serial.println("--------------------------------------------------");
  Serial.println("Current Program: gloo_6");
  Serial.println("Author: Alberto Tam Yong");
  Serial.println("--------------------------------------------------");
  Serial.println("debug");
  Serial.println("led(<r>,<g>,<b>)");
  Serial.println("lcd");
  Serial.println("servo(<servo number>,<pos>");
  Serial.println("cam_rpi: enable/disable camera readings");
  Serial.println("toggle_servo: enable/disable servos");
  Serial.println("RASPBERRY PI COMMANDS:");
  Serial.println("login_rpi: login to rpi");
  Serial.println("init_rpi: initiate facial recognition");
  Serial.println("face_rpi: run facial recognition (only use after stop_rpi)");
  Serial.println("stop_rpi: stop facial recognition");
  Serial.println("shutdown_rpi");
  Serial.println("--------------------------------------------------");
}

void clear_array(char data[128])
{
  for (int i = 0; i < 128; i++)
  {
    data[i] = 0;
  }
}

void login_rpi()
{
  Serial3.println("pi");
  Serial.println("pi");
  delay(1000);
  Serial3.println("raspberry");
  Serial.println("password: *********");
  delay(8000);
}

void init_rpi()
{
  Serial.println("Initiating Facial Recognition");
  delay(10);
  Serial3.println("source ~/.profile");
  Serial.println("source ~/.profile");
  delay(8000);
  Serial3.println("workon cv");
  Serial.println("workon cv");
  delay(8000);
  run_facial_recognition_rpi();
}

void run_facial_recognition_rpi()
{
  //Serial3.println("python /home/pi/Documents/python27/detect_face_spi_noGUI_1.py");
  //Serial.println("python /home/pi/Documents/python27/detect_face_spi_noGUI_1.py");
  Serial.println("python /home/pi/Documents/python27/color_track_4.py");
  Serial3.println("python /home/pi/Documents/python27/color_track_4.py");
  delay(5000);
}

void stop_rpi()
{
  Serial3.write('\x03');
  Serial.write("^C");
  Serial.println();
  delay(15);
}

void shutdown_rpi()
{
  Serial3.write('\x03');
  delay(2000);
  Serial3.write("sudo shutdown now");
  Serial.println("sudo shutdown now");
  delay(2000);
}

void cam_rpi()
{
  camera_flag = !camera_flag;
  Serial.print("Camera Status: ");
  Serial.println(camera_flag);
}

void ERROR_NOT_FOUND(char data[128])
{
  Serial.print("ERROR: ");
  Serial.println(data);
}

void servo_toggle()
{
  servo_flag = !servo_flag;
  Serial.print("Servo Status: ");
  Serial.println(servo_flag);
}

void debug_mode_init()
{
  debug_mode = !debug_mode;
  Serial.print("Debug mode: ");
  Serial.println(debug_mode);
}

void led(char data[128])
{
  unsigned int r = 0;
  unsigned int g = 0;
  unsigned int b = 0;
  unsigned int array_pos = 0;

  int parenthesis = search_char_on_array(data, '(');
  array_pos = parenthesis;
  r = parse_parameter(data, parenthesis + 1);
  array_pos += count_digits(r);
  array_pos += 2;
  g = parse_parameter(data, array_pos);
  array_pos += count_digits(g);
  array_pos += 2;
  b = parse_parameter(data, array_pos);

  if (r  > 255 || g > 255 || b > 255)
  {
    led_manual_flag = 0;
  }
  else
  {
    led_manual_flag = 1;
    analogWrite(RGB_R, r);
    analogWrite(RGB_G, g);
    analogWrite(RGB_B, b);
  }
}

void lcd(char data[128])
{
  unsigned int param1 = 0;
  unsigned int array_pos = 0;

  int parenthesis = search_char_on_array(data, '(');
  array_pos = parenthesis;
  param1 = parse_parameter(data, parenthesis + 1);
  array_pos += count_digits(param1);
}

void servo(char data[128])
{
  unsigned int param1 = 0;
  unsigned int param2 = 0;
  unsigned int array_pos = 0;

  int parenthesis = search_char_on_array(data, '(');
  array_pos = parenthesis;
  param1 = parse_parameter(data, parenthesis + 1);
  array_pos += count_digits(param1);
  array_pos += 2;
  param2 = parse_parameter(data, array_pos);
  array_pos += count_digits(param2);
  
  camera_flag = false;
  follow_camera = false;
  
  if (param1 == 1)
  {
    servo_base_pos = param2;
    servo_base.write(servo_base_pos);
    Serial.print("Servo Base: ");
    Serial.println(servo_base_pos);
  }
  else if (param1 == 2)
  {
    servo_tilt_pos = param2;
    servo_tilt.write(servo_tilt_pos);
    Serial.print("Servo Tilt: ");
    Serial.println(servo_tilt_pos);
  }
}

void serial_command_check()
{
  unsigned char incomingCommand_size = 0;

  //Wait for incoming user input on Serial Terminal
  if (Serial.available())
  {
    incomingCommand_size = Serial.available(); //store the size of the input on a variable
    //Read and store all incoming data on UART on incomingCommand array
    for (int i = 0; i < incomingCommand_size; i++)
    {
      serial_incoming[i] = Serial.read(); //store the data on the buffer on an array
    }

    //print_array(serial_incoming);
    //Serial.println(command_check(serial_incoming));

    //command_check() evaluates and identifies if the command entered is on the list
    switch (command_check(serial_incoming))
    {
      case 0:
        help_function(serial_incoming);
        break;
      case 1:
        init_rpi();
        break;
      case 2:
        stop_rpi();
        break;
      case 3:
        led(serial_incoming);
        break;
      case 4:
        lcd(serial_incoming);
        break;
      case 5:
        servo(serial_incoming);
        break;
      case 6:
        login_rpi();
        break;
      case 7:
        cam_rpi();
        break;
      case 8:
        servo_toggle();
        break;
      case 9:
        run_facial_recognition_rpi();
        break;
      case 10:
        shutdown_rpi();
        break;
      case 11:
        debug_mode_init();
        break;
      default:
        ERROR_NOT_FOUND(serial_incoming);
        break;
    }

    clear_array(serial_incoming); //clear array storing command buffer
  }
}

//--------- END OF MENU FUNCTIONS ------

//-------------------------------------
//--------- STATE MACHINES ------------
//-------------------------------------

//--------- State Machine 1 ------

//SM1_Tick() updates servo position
//Based on camera input

int SM1_Period = 50;
unsigned long prevMillis1 = 0;
unsigned long prevMillis3 = 0;
enum SM1_States {SM1_init, SM1_follow} SM1_state;

void SM1_Tick()
{
  delta_camera_x = prev_camera_x - camera_x;
  delta_camera_y = prev_camera_y - camera_y;
  
  //old code:
  //follow_camera = (abs(delta_camera_x)>x_threshold||abs(delta_camera_y)>y_threshold)?1:0;
  
  switch (SM1_state)
  {
    case SM1_init:
      break;
    case SM1_follow:
      /*
	  //Joystick Test
      if(abs(delta_camera_x) > x_threshold)
      {
        prev_camera_x = camera_x;
        //servo_base_pos = map(camera_x,0,1023,0,180); //for joystick test
        //servo_base_pos = 180 - servo_base_pos; //for joystick test
        //servo_base_pos = +delta_camera_x;


        if(servo_base_pos >= 180)
          servo_base_pos = 180;
        else if(servo_base_pos <= 0)
          servo_base_pos = 0;
        else if(servo_base_pos

        servo_base.write(servo_base_pos);
      }

      if(abs(delta_camera_y) > y_threshold)
      {
        prev_camera_y = camera_y;
        //servo_tilt_pos = map(camera_y,0,1023,0,180); //for joystick test
        //servo_tilt_pos += delta_camera_y;

        if(servo_tilt_pos >= 180)
          servo_tilt_pos = 180;
        else if(servo_tilt_pos <= 0)
          servo_tilt_pos = 0;

        servo_tilt.write(servo_tilt_pos);
      }
	  */
	  
	  //Debug code to show servo position on TFT
      tft.fillRect(0,40,80,40,ILI9341_BLACK);
      tft.setCursor(0,40);
      tft.setTextColor(ILI9341_WHITE);
      tft.println(servo_base_pos);
      tft.println(servo_tilt_pos);
      */

      /*
	  //Old movement algorithm
      if(camera_x < (video_width-camera_h)/3)
        servo_base_pos++;
      else if(camera_x > ((video_width-camera_h)*2)/3)
        servo_base_pos--;
      else
        servo_base_pos = 70 + (camera_x/10);
      */

      if (camera_x < 20)
        servo_base_pos += 1;
      else if (camera_x > 60)
        servo_base_pos -= 1;

      if (camera_y < 15)
        servo_tilt_pos -= 1;
      else if (camera_y > 45)
        servo_tilt_pos += 1;

      if (servo_base_pos >= servo_base_pos_upper)
        servo_base_pos = servo_base_pos_upper;
      else if (servo_base_pos <= servo_base_pos_lower)
        servo_base_pos = servo_base_pos_lower;

      /*
	  //Old movement algorithm
      if(camera_y < (video_height-camera_h)/3)
        servo_tilt_pos--;
      else if(camera_y > ((video_height-camera_h)*2)/3)
        servo_tilt_pos++;
      else
        servo_tilt_pos = 70 + (camera_y/10);
      */

      if (servo_tilt_pos >= servo_tilt_pos_upper)
        servo_tilt_pos = servo_tilt_pos_upper;
      else if (servo_tilt_pos <= servo_tilt_pos_lower)
        servo_tilt_pos = servo_tilt_pos_lower;
        
      /*
	  //Code for debugging
      Serial.print("Width: ");
      Serial.print((video_width - camera_h) / 3);
      Serial.print("\t");
      Serial.print("Height: ");
      Serial.print((video_height - camera_h) / 3);
      Serial.print("\t");
      Serial.print("Base: ");
      Serial.print(servo_base_pos);
      Serial.print("\t");
      Serial.print("Tilt: ");
      Serial.println(servo_tilt_pos);
      */
      
      if(servo_base_pos != prevServo_base_pos)
      {
        servo_base.write(servo_base_pos);
        prevServo_base_pos = servo_base_pos;
      } 
      if(servo_tilt_pos != prevServo_tilt_pos)
      {
        servo_tilt.write(servo_tilt_pos);
        prevServo_tilt_pos = servo_tilt_pos;
      }

      break;
    default:
      break;
  }
  switch (SM1_state)
  {
    case SM1_init:
      if (follow_camera && servo_flag == 1)
        SM1_state = SM1_follow;
      else
        SM1_state = SM1_init;
      break;
    case SM1_follow:
      if (follow_camera)
        SM1_state = SM1_follow;
      else
        SM1_state = SM1_init;
    default:
      break;
  }
}

//----- end of State Machine 1 ------

//------- TFT Display Functions

int mouth_h = 50;
int eyelit_h = (240/2) + 50;
int eyelit_l = (320/2)-80;
int eyelit_r = (320/2)+80;

void drawArc(int x1, int y1, int x2, int y, int r, int pixel_color)
{
  int y_temp = 0;
  int x_temp = 0;
  x_temp = ((x2-x1)/2)+x1;
  if(r >= sqrt((y1-y)*(y1-y)+(x_temp-x1)*(x_temp-x1)))
  {
    for(int i = x1; i < x_temp; i++)
    {
      y_temp = sqrt((r)*(r)-((x_temp-i)*(x_temp-i)));
      tft.drawPixel(y-y_temp,i,pixel_color);
    }
    for(int i = x_temp; i < x2; i++)
    {
      y_temp = sqrt((r)*(r)-((i-x_temp)*(i-x_temp)));
      tft.drawPixel(y-y_temp,i,pixel_color);
    }
  }
}

void drawArc_up(int x1, int y1, int x2, int y, int r, int pixel_color)
{
  int y_temp = 0;
  int x_temp = 0;
  x_temp = ((x2-x1)/2)+x1;
  if(r >= sqrt((y-y1)*(y-y1)+(x_temp-x1)*(x_temp-x1)))
  {
    for(int i = x1; i < x_temp; i++)
    {
      y_temp = sqrt((r)*(r)-((x_temp-i)*(x_temp-i)));
      tft.drawPixel(y+y_temp,i,pixel_color);
    }
    for(int i = x_temp; i < x2; i++)
    {
      y_temp = sqrt((r)*(r)-((i-x_temp)*(i-x_temp)));
      tft.drawPixel(y+y_temp,i,pixel_color);
    }
  }
}

void mouth()
{
  tft.drawLine(mouth_h,(320/2)-50,mouth_h,(320/2)+50,ILI9341_WHITE);
}

void eyelit()
{
  tft.drawCircle(eyelit_h,eyelit_r, 40, ILI9341_WHITE);
  tft.drawCircle(eyelit_h,eyelit_l, 40, ILI9341_WHITE);
  
  /*for(int i = -20; i < 20; i=i+5)
  {
    retina_l = i;
    retina_r = i;
    tft.fillCircle((240/2)+retina_l-5,(320/2)+80, 10, ILI9341_BLACK);
    tft.fillCircle((240/2)+retina_r-5,(320/2)-80, 10, ILI9341_BLACK);
    tft.fillCircle((240/2)+retina_l,(320/2)+80, 10, ILI9341_WHITE);
    tft.fillCircle((240/2)+retina_r,(320/2)-80, 10, ILI9341_WHITE);
    //delay(1);
  }
  for(int i = 20; i > -20; i=i-5)
  {
    retina_l = i;
    retina_r = i;
    tft.fillCircle((240/2)+retina_l+5,(320/2)+80, 10, ILI9341_BLACK);
    tft.fillCircle((240/2)+retina_r+5,(320/2)-80, 10, ILI9341_BLACK);
    tft.fillCircle((240/2)+retina_l,(320/2)+80, 10, ILI9341_WHITE);
    tft.fillCircle((240/2)+retina_r,(320/2)-80, 10, ILI9341_WHITE);
    //delay(1);
  }*/
}

void eyelit_undo()
{
  tft.drawCircle(eyelit_h,eyelit_r, 40, ILI9341_BLACK);
  tft.drawCircle(eyelit_h,eyelit_l, 40, ILI9341_BLACK);
}

void blinking()
{
  //tft.drawCircle((240/2),(320/2)+80, 40, ILI9341_BLACK);
  //tft.drawCircle((240/2),(320/2)-80, 40, ILI9341_BLACK);
  tft.drawLine(eyelit_h,(320/2)+80-40, eyelit_h,(320/2)+80+40, ILI9341_WHITE);
  tft.drawLine(eyelit_h,(320/2)-80-40, eyelit_h,(320/2)-80+40, ILI9341_WHITE);
}

void blinking_undo()
{
  tft.drawLine(eyelit_h,(320/2)+80-40, eyelit_h,(320/2)+80+40, ILI9341_BLACK);
  tft.drawLine(eyelit_h,(320/2)-80-40, eyelit_h,(320/2)-80+40, ILI9341_BLACK);
}

void face_sleeping()
{
  tft.drawCircle(mouth_h,(320/2), 10, ILI9341_WHITE);
  drawArc(eyelit_r-40,eyelit_h,eyelit_r+40,eyelit_h+20,60,ILI9341_WHITE);
  drawArc(eyelit_l-40,eyelit_h,eyelit_l+40,eyelit_h+20,60,ILI9341_WHITE);
}

void face_sleeping_undo()
{
  tft.drawCircle(mouth_h,(320/2), 10, ILI9341_BLACK);
  drawArc(eyelit_r-40,eyelit_h,eyelit_r+40,eyelit_h+20,60,ILI9341_BLACK);
  drawArc(eyelit_l-40,eyelit_h,eyelit_l+40,eyelit_h+20,60,ILI9341_BLACK);
}

void face_stand_by()
{
  eyelit();
  mouth();
}

void face_smile()
{
  drawArc((320/2)-50,mouth_h+20,(320/2)+50,mouth_h+50,60,ILI9341_WHITE);
  eyelit();
}

void face_sad()
{
  drawArc_up((320/2)-50,mouth_h+20,(320/2)+50,mouth_h-10,60,ILI9341_WHITE);
  eyelit();
}

void face_excited()
{
  tft.drawLine(mouth_h+30,(320/2)-60,mouth_h+30,(320/2)+60,ILI9341_WHITE);
  drawArc((320/2)-60,mouth_h+40,(320/2)+60,mouth_h+40,60,ILI9341_WHITE);
  eyelit();
}

void clear_eyes()
{
  tft.fillCircle(eyelit_h,eyelit_r, 50, ILI9341_BLACK);
  tft.fillCircle(eyelit_h,eyelit_l, 50, ILI9341_BLACK);
}

void clear_mouth()
{
  tft.fillRect(mouth_h-30,(320/2)-100,80,200, ILI9341_BLACK);
}

//---------- State Machine 2 ------

int SM2_Period = 50;
unsigned long prevMillis2 = 0;
enum SM2_Status {SM2_init,SM2_sleep,SM2_blink,SM2_expression} SM2_status;

void SM2_Tick()
{
  switch(SM2_status)
  {
    case SM2_init:
      break;
    case SM2_sleep:
    if(change_of_eyes)
      {
        clear_eyes();
        change_of_eyes = 0;
      }
      if(change_of_mouth)
      {
        clear_mouth();
        change_of_mouth = 0;
      }
      breathing = 1;
      face_sleeping();
      servo_flag = 0;
      break;
    case SM2_blink:
      clear_eyes();
      blink_counter = 0;
      blinking();
      change_of_eyes = 1;
      change_of_mouth = 1;
      break;
    case SM2_expression:
      if(mood != prev_mood)
        change_of_mouth = 1;
      
      if(change_of_eyes)
      {
        clear_eyes();
        change_of_eyes = 0;
      }
      if(change_of_mouth)
      {
        clear_mouth();
        change_of_mouth = 0;
      }
      if(activity_counter > activity_interval)
        activity = 0;
      servo_flag = 1;
      if(mood == NORMAL)
        face_stand_by();
      else if(mood == HAPPY)
        face_smile();
      else if(mood == SAD)
        face_sad();
      else if(mood == EXCITED)
        face_excited();
        
      prev_mood = mood;
      break;
    default:
      break;
  }
  switch(SM2_status)
  {
    case SM2_init:
      SM2_status = SM2_sleep;
      break;
    case SM2_sleep:
      if(activity == 0)
        SM2_status = SM2_sleep;
      else
      {
        SM2_status = SM2_expression;
        change_of_eyes = 1;
        change_of_mouth = 1;
        breathing = 0;
      }
      break;
    case SM2_blink:
      SM2_status = SM2_expression;
      break;
    case SM2_expression:
       if(activity == 0)
       {
         SM2_status = SM2_sleep;
         change_of_eyes = 1;
          change_of_mouth = 1;
       }
       else if(blink_counter > blink_interval)
         SM2_status = SM2_blink;
       else
         SM2_status = SM2_expression;
      break;
    default:
      break;
  }
  blink_counter++;
  activity_counter++;
}

//----- end of State Machine 2 ------

//------ Functions to Parse Serial Data Streams ----

void init_array(char array[], int array_size)
{
  for (int i = 0; i < array_size; i++)
  {
    array[i] = 0;
  }
}

bool input_cam()
{
  if (Serial3.available())
  {
    init_array(serial3_incoming, 128);
    serial3_size = Serial3.available();
    for (int i = 0; i < serial3_size; i++)
    {
      serial3_incoming[i] = Serial3.read();
    }
    //delay(20);
    //Serial.print(serial3_incoming);
    //delay(20);
    return true;
  }
  else
    return false;
}

int search_int_on_array(char data[128], unsigned int start_search)
{
  unsigned int i = start_search;
  while (data[i] < '0' || data[i] > '9')
  {
    i++;
  }
  return i;
}

int search_char_on_array(char data[128], char char_goal)
{
  unsigned int i = 0;
  while (data[i] != char_goal)
  {
    i++;
  }
  return i;
}

int parse_parameter(char data[128], int parenthesis)
{
  unsigned int number_param;
  int i = parenthesis;
  while (data[i] <= '9' && data[i] >= '0')
  {
    //Designed to be 2-digits max
    if (i == parenthesis)
    {
      //first digit
      number_param = (data[i] - 48); //store first digit
    }
    else if (i > parenthesis)
    {
      number_param *= 10; //first digit, multiplied by 10
      number_param += (data[i] - 48); //add second digit to ServoID
    }
    i++;
  }
  return number_param;
}

int count_digits(int number)
{
  int i = 0;
  while (number > 0)
  {
    number /= 10;
    i++;
  }
  return i;
}

//-------- END OF FUNCTIONS TO PARSE DATA STREAMS ----

//----------- State Machine 4 --------------

int SM4_Period = 50;
unsigned long prevMillis4 = 0;
enum SM4_Status {SM4_init, SM4_update_cam, SM4_search, SM4_missing_you} SM4_status;

unsigned int x, y, h, t;
unsigned int x_size = 0, y_size = 0, h_size = 0, t_size = 0;
unsigned int int_start = 0;
unsigned int temp_int_start;

void SM4_Tick()
{
  switch (SM4_status)
  {
    case SM4_init:
      count_last_time_seen++;
      break;
    case SM4_update_cam:
      if(mood != EXCITED)
        mood = HAPPY;
      activity = 1;
      activity_counter = 0;
      first_reading_taken = 1;
      input_cam();
      int_start = search_int_on_array(serial3_incoming, 0);
      x = parse_parameter(serial3_incoming, int_start);
      x_size = count_digits(x);
      int_start = search_int_on_array(serial3_incoming, int_start + x_size + 1);
      y = parse_parameter(serial3_incoming, int_start);
      y_size = count_digits(y);
      int_start = search_int_on_array(serial3_incoming, int_start + y_size + 1);
      h = parse_parameter(serial3_incoming, int_start);
      h_size = count_digits(h);
      int_start = search_int_on_array(serial3_incoming, int_start + h_size + 1);
      t = parse_parameter(serial3_incoming, int_start);
      t_size = count_digits(t);

      camera_x = x;
      camera_y = y;
      camera_h = h;
      camera_t = t;
      
      if(debug_mode == 1)
      {
        Serial.print(x);
        Serial.print("\t");
        Serial.print(y);
        Serial.print("\t");
        Serial.print(h);
        Serial.println("\t");
        
        for(int i=0; i < 10; i++)
        {
          for(int j = 0; j < 20; j++)
          {
            if(i==0 || i == 9)
              Serial.print("-");
            /*else if(j <= (20 - (x/x_proportion) - (h/x_proportion)) && j >= (20 - (x/x_proportion))
                        && i >= (y/y_proportion) && i <= ((y/y_proportion) + (h/y_proportion)))*/
            else if(j == (20 - (x/x_proportion) - (h/x_proportion)/2) && i == ((y/y_proportion) + (h/y_proportion)/2))
              Serial.print("*");
            else
              Serial.print(" ");
          }
          Serial.println();
        }
      }
      led_manual_flag = 1;
      
      if(mood == EXCITED)
      {
        t = 3;
      }
      switch(t)
      {
        case 0:
          rgb_color(0,255,0);
          break;
        case 1:
          rgb_color(255,0,0);
          break;
        case 2:
          rgb_color(0,0,255);
          break;
        case 3:
          rgb_random();
          break;
        default:
          rgb_color(0,0,0);
          break;
      }
      
      count_last_time_seen = 0;
      
      follow_camera = 1;

      break;
    case SM4_search:
      led_manual_flag = 1;
      
      rgb_color(0,0,0);
      
      mood = SAD;
      count_last_time_seen++;
      break;
    case SM4_missing_you:
      led_manual_flag = 1;
      //Serial.println("missing you :(");
      
      mood = NORMAL;
      break;
    default:
      break;
  }
  switch (SM4_status)
  {
    case SM4_init:
      if (input_cam() && camera_flag == 1)
      {
        SM4_status = SM4_update_cam;
        follow_camera = 0;
      }
      else if(count_last_time_seen >= 200 && first_reading_taken)
        SM4_status = SM4_missing_you;
      else if(count_last_time_seen >= 100 && first_reading_taken)
      {
        SM4_status = SM4_search;
      }
      else
        SM4_status = SM4_init;
      break;
    case SM4_update_cam:
      SM4_status = SM4_init;
      break;
    case SM4_search:
      if (input_cam() && camera_flag == 1)
      {
        SM4_status = SM4_update_cam;
        mood = EXCITED;
        follow_camera = 0;
      }
      else if(count_last_time_seen >= 100 && count_last_time_seen < 200)
        SM4_status = SM4_search;
      else
      {
        SM4_status = SM4_missing_you;
        Serial.println("I miss you :(");
      }
      break;
    case SM4_missing_you:
      SM4_status = SM4_init;
      break;
    default:
      break;
  }
  /*
  //Temporary
  switch (SM4_status)
  {
    case SM4_init:
      if (input_cam() && camera_flag == 1)
      {
        SM4_status = SM4_update_cam;
        follow_camera = 0;
      }
      else
        SM4_status = SM4_init;
      break;
    case SM4_update_cam:
      SM4_status = SM4_init;
      break;
    default:
      break;
  }*/
}

//-------- end of state machine 4 -------

//---------- State Machine 5 --------

int SM5_Period = 100;
unsigned long prevMillis5 = 0;
enum SM5_RGB_Status {SM5_init,SM5_breath} SM5_status;

void SM5_Tick()
{
  switch(SM5_status)
  {
    case SM5_init:
      break;
    case SM5_breath:
      rgb_color(0,0,intensity_breath);
      if(intensity_breath >= 250)
        dir_breath = 0;
      else if(intensity_breath <= 0)
        dir_breath = 1;
        
      if(dir_breath)
        intensity_breath += 25;
      else if(dir_breath == 0)
        intensity_breath -= 25;
        
      break;
    default:
      break;
  }
  switch(SM5_status)
  {
    case SM5_init:
      if(breathing)
      {
        SM5_status = SM5_breath;
        intensity_breath = 0;
        dir_breath = 1;
      }
      else
        SM5_status = SM5_init;
      break;
    case SM5_breath:
      if(breathing)
        SM5_status = SM5_breath;
      else
        SM5_status = SM5_init;
      break;
    default:
      break;
  }
}

//------------ end of State Machine 5 ---------

//--------- State Machine for menu inputs -----

int SM_serial_check_period = 50;
unsigned long prevMillis_serial_check = 0;
enum SM_Serial_Check_Status {SM_serial_check_init, SM_serial_check_run} SM_serial_check_status;

void SM_Serial_Check_Tick()
{
  switch (SM_serial_check_status)
  {
    case SM_serial_check_run:
      serial_command_check();
      break;
    default:
      break;
  }
  switch (SM_serial_check_status)
  {
    case SM_serial_check_init:
      SM_serial_check_status = SM_serial_check_run;
      break;
    case SM_serial_check_run:
      SM_serial_check_status = SM_serial_check_run;
      break;
    default:
      break;
  }
}

//-------- end of state machine for menu inputs ----

//---- Main System Functions ----

void setup() {
  Serial.begin(115200); //for Menu and debugging

  Serial3.begin(115200); //for communication with Raspberry Pi

  pinMode(button1_pin, INPUT_PULLUP);
  pinMode(button2_pin, INPUT_PULLUP);

  servo_base.attach(SERVO_BASE_PIN);
  servo_tilt.attach(SERVO_TILT_PIN);

  pinMode(RGB_R, OUTPUT);
  pinMode(RGB_G, OUTPUT);
  pinMode(RGB_B, OUTPUT);

  tft.begin();
  
  //Creating a lot of rought movements
  //Initiation position
  servo_base.write(servo_base_pos);
  servo_tilt.write(servo_tilt_pos);
  
  tft.begin();
  tft.fillScreen(ILI9341_BLACK);
  
  //Init Raspberry Pi
  rgb_color(0,0,255);
  delay(30000);
  rgb_color(0,255,0);
  login_rpi();
  rgb_color(0,0,255);
  init_rpi();
  rgb_color(255,0,0);
  delay(1000);
  
  Serial.print("Ready");
}

void loop() {
  button1 = digitalRead(2);
  button2 = digitalRead(3);
  if (millis() - prevMillis1 >= SM1_Period)
  {
    SM1_Tick();
    prevMillis1 = millis();
  }
  if (millis() - prevMillis2 >= SM2_Period)
  {
    SM2_Tick();
    prevMillis2 = millis();
  }
  if (button1 == LOW)
  {
    init_rpi();
  }
  if (button2 == LOW)
  {
    stop_rpi();
  }
  if (millis() - prevMillis4 >= SM4_Period)
  {
    SM4_Tick();
    prevMillis4 = millis();
  }
  if(millis() - prevMillis5 >= SM5_Period)
  {
    SM5_Tick();
    prevMillis5 = millis();
  }
  if (millis() - prevMillis_serial_check >= SM_serial_check_period)
  {
    SM_Serial_Check_Tick();
    prevMillis_serial_check = millis();
  }
}
