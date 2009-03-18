#include <LiquidCrystal.h>
#include <Debounce.h>

/*Klugephrase
http://feelslikeburning.com/klugephrase

Adam Wolf
with patches from Matthew Beckler
*/

#define INITIAL_BUZZ_DELAY 750
#define ROUND_LENGTH  60000

#define RS 2
#define RW 3
#define E 4
#define DB4 5
#define DB5 8
#define DB6 7
#define DB7 6

#define BUTTON1 13 //up (start/stop) 
#define BUTTON2 11 //right
#define BUTTON3 10 //left
#define BUTTON4 9 //down
#define BUTTON5 12 //enter

#define SPEAKER 14

#define EEPROM_ADDRESS 0x50

#include "WProgram.h"
void setup();
void loop();
void button_setup();
void update_buttons();
void i2c_eeprom_read_16_block(int deviceaddress, unsigned int eeaddress, byte* buffer);
void i2c_eeprom_write_16_block(int deviceaddress, unsigned int eeaddress, byte* buffer);
void i2c_eeprom_write_int( int deviceaddress, unsigned int eeaddress, int data );
int i2c_eeprom_read_int( int deviceaddress, unsigned int eeaddress );
void i2c_eeprom_write_byte( int deviceaddress, unsigned int eeaddress, byte data );
void i2c_eeprom_write_page( int deviceaddress, unsigned int eeaddresspage, byte* data, byte length );
byte i2c_eeprom_read_byte( int deviceaddress, unsigned int eeaddress );
void i2c_eeprom_read_buffer( int deviceaddress, unsigned int eeaddress, byte *buffer, int length );
void eeprom_setup();
void game_startup();
void display_category(int category);
void display_word(int category, int word_index);
int next_category(int current_category);
int previous_category(int current_category);
int next_word_index(int category, int current_word_index);
int previous_word_index(int category, int current_word_index);
void output_string(int index, int output_serial, int output_lcd);
void output_string(char* buffer, int output_serial, int output_lcd);
void lcd_output_two_lines(char* line1, char* line2);
void lcd_output(char* line1);
void lcd_print(char* buffer);
void display_end_of_round(int round_score);
void buzz(int duration, int delay_time);
void beep();
void monitor_serial();
void parse_command(char* input);
void api_help();
void api_list_all();
void api_list_categories(char* input);
void set_category_count(int count);
int get_category_count();
void increment_category_count();
int get_category_address(int index);
void get_category_title(int category, char* buffer);
void set_category_title(int category, char* buffer);
int get_word_count(int category);
void set_word_count(int category, int count);
void increment_word_count(int category);
int get_first_word_address(int category);
void set_first_word_address(int category, int address);
int get_word_address(int category, int index);
int get_max_word_count(int category);
void set_max_word_count(int category, int count);
void get_word(int category, int index, char* buffer);
void set_word(int category, int index, char* buffer);
int append_word(int category, char* buffer);
void set_category(int index, char* title, int word_count, int first_word_address, int max_word_count);
int append_category(char* title, int max_word_count);
int s_add_category(char* title, int index);
LiquidCrystal lcd(RS, RW, E, DB4, DB5, DB6, DB7); 

//global button setup
//TODO:
//put buttons in array
Debounce button1 = Debounce( 20 , BUTTON1 ); 
Debounce button2 = Debounce( 20 , BUTTON2 ); 
Debounce button3 = Debounce( 20 , BUTTON3 ); 
Debounce button4 = Debounce( 20 , BUTTON4 ); 
Debounce button5 = Debounce( 20 , BUTTON5 );

//this button garbage needs to be integrated into the class...
int button1_depressed = 0;
int button1_released = 0;
int button2_depressed = 0;
int button2_released = 0;
int button3_depressed = 0;
int button3_released = 0;
int button4_depressed = 0;
int button4_released = 0;
int button5_depressed = 0;
int button5_released = 0;

//button map
//button 1 = start/stop or up
//button 2 = team 2 or right
//button 3 = team 1 or left
//button 4 = category or down
//button 5 = next or enter

//global game setup
char state = 'i';


//state table:
//i = initializing
//n = new game
//c = choosing category
//r = round
//p = paused
//x = no categories
//s = scoring
//u = usb


/*

void test()
{
  //lcd_test();
  //button_test();
  //speaker_test();
  //word_test();
  //"Tests complete."
  //output_string(13, 1, 1);
}

*/

void setup()
{
  Serial.begin(9600);
  eeprom_setup();
  button_setup();

    pinMode(SPEAKER, OUTPUT);
  //Serial.println(availableMemory());
  //test();
  state = 'n';
  randomSeed(analogRead(3));
  //word_dump();
  //api_list_all();
}


void loop()
{
  static int category = -1;
  static int word = -1;
  static int team1_score = -1;
  static int team2_score = -1;
  static int last_category = -1;
  static long round_start = -1;
  static long round_end = -1;
  static int round_score = -1;
  static int buzz_delay = -1;
  static long last_buzz = -1;
  static int buzz_count = -1;
  static long round_length = -1;
  long current_time = millis();
  static long pause_time = -1;

  
  update_buttons();
  
  //Serial.println(state);

  switch (state)
  {

  case 'n':
    game_startup();
    //the machine has just been turned on
    update_buttons();
    delay(50);
    update_buttons();
    
    
    if (button1.read() == 0)
    {
      output_string(10, 1, 1);
      state = 'u';
      break;
    }


    if (get_category_count() == 0)
    {
      Serial.println("no words");

      state = 'u';
    } 
    else {
      //start picking the categories
      category = 0;
      state = 'c';
      output_string(19, 1, 1);
      delay(2000);
      display_category(category);
      break;
    }
    break;

  case 'c':
    //we're picking categories
    if (button2_depressed or button4_depressed)
    {
      //next category
      category = next_category(category);
      display_category(category);
    } 
    else if (button3_depressed)
    {
      //previous category
      category = previous_category(category);
      display_category(category);
    } 
    else if (button1_depressed or button5_depressed)
    {
      //if there are any words in that category, let's start it!
      if (get_word_count(category) > 0)
      {
        state = 'r';
        round_score = 0;
        round_end = current_time + ROUND_LENGTH;
        word = random(get_word_count(category)-1);
        //word = 0;
        
        last_buzz = current_time;
        buzz_delay = INITIAL_BUZZ_DELAY;
        display_word(category, word);
        buzz_count = 0;
        break;
      } 
      else
      { 
        //no words in that category.
        output_string(11, 1, 1);
      }
    } 
    else {
      //nothing was pressed.
    }
    break;


  case 'r':
    //we're starting a round

    if (current_time >= round_end)
    {
      //round over!
      buzz(250, 4);
      display_end_of_round(round_score);


      state = 'c';
      output_string(19, 1, 1);
      //delay(1000);
     // display_category(category);
      break;
    }

    //should we buzz?
    if (current_time >= last_buzz + buzz_delay)
    { 
      //it's buzz time!
      last_buzz = current_time;
      buzz_count = buzz_count + 1;
      beep();

      if (buzz_count > 30)
      {
        buzz_count = 0;
        if (buzz_delay > 100)
        {
          buzz_delay = .75 * buzz_delay;
        }
      }
    }


    if (button1_depressed)
    {
      //pause!
      output_string(24, 1, 1);
      pause_time = current_time;
      state = 'p';
      break;
    }

    if (button2_depressed)
    {
      //advance word without incrementing score
            Serial.println(category);
      Serial.println(word);
      word = next_word_index(category, word);
      display_word(category, word);
    } 
    else if (button3_depressed)
    {
      //go back to the last word
     //       Serial.println(category);
      //Serial.println(word);
      word = previous_word_index(category, word);
      display_word(category, word);
    } 
    else if (button5_depressed)
    {
      //advance word and increment score
      Serial.println(category);
      Serial.println(word);
      word = next_word_index(category, word);
      display_word(category, word);
      round_score++;
      buzz(10,1);
    }

    //don't do anything if category button is pushed.

    break;

  case 'p':
    //we're paused.
    if (button1_depressed)
    {
      //compensate
      last_buzz = last_buzz + current_time - pause_time;
      round_end = round_end + current_time - pause_time;
      state = 'r';
      display_word(category, word);
      pause_time = -1;
    }
    break;


  case 'u':
    //we're in usb mode.
    //Serial.println("USB Connected.");
   
    monitor_serial();

  default:
    break;
  }

}

//I use the library Debounce from http://www.arduino.cc/playground/Code/Debounce
//I end up having to work around it a lot... I don't know if I'm going to still use it in further revisions of the code.
void button_setup()
{
  //don't forget the global Debounce objects!
  pinMode(BUTTON1, INPUT);
  pinMode(BUTTON2, INPUT);
  pinMode(BUTTON3, INPUT);
  pinMode(BUTTON4, INPUT);
  pinMode(BUTTON5, INPUT);
}

void update_buttons()
{
  static int button1_last = 1;
  static int button2_last = 1;
  static int button3_last = 1;
  static int button4_last = 1;
  static int button5_last = 1;


 button1.update();
  button2.update();
  button3.update();
   button4.update();
   button5.update();

  if (button1.read() == button1_last)
  {
    button1_depressed = 0;
    //button1_released = 0;
    //its the same as it was before
  } 
  
  
  else if (button1.read() == 0 and button1_last == 1)
  {
    button1_depressed = 1;
    //button1_released = 0;
  } 
  else if (button1.read() == 1 and button1_last == 0)
  {
    //button1_released = 1;
    button1_depressed = 0;
  }


  if (button2.read() == button2_last)
  {
    button2_depressed = 0;
    //button2_released = 0;
    //its the same as it was before
  } 
  else if (button2.read() == 0 and button2_last == 1)
  {
    button2_depressed = 1;
    //button2_released = 0;
  } 
  else if (button2.read() == 1 and button2_last == 0)
  {
//button2_released = 1;
    button2_depressed = 0;
  }


  if (button3.read() == button3_last)
  {
    button3_depressed = 0;
    //button3_released = 0;
    //its the same as it was before
  } 
  else if (button3.read() == 0 and button3_last == 1)
  {
    button3_depressed = 1;
    //button3_released = 0;
  } 
  else if (button3.read() == 1 and button3_last == 0)
  {
    //button3_released = 1;
    button3_depressed = 0;
  }


  if (button4.read() == button4_last)
  {
    button4_depressed = 0;
    //button4_released = 0;
    //its the same as it was before
  } 
  else if (button4.read() == 0 and button4_last == 1)
  {
    button4_depressed = 1;
    //button4_released = 0;
  } 
  else if (button4.read() == 1 and button4_last == 0)
  {
    //button4_released = 1;
    button4_depressed = 0;
  }

  if (button5.read() == button5_last)
  {
    button5_depressed = 0;
    //button5_released = 0;
    //its the same as it was before
  } 
  else if (button5.read() == 0 and button5_last == 1)
  {
    button5_depressed = 1;
    //button5_released = 0;
  } 
  else if (button5.read() == 1 and button5_last == 0)
  {
    //button5_released = 1;
    button5_depressed = 0;
  }

  button1_last = button1.read();
  button2_last = button2.read();
  button3_last = button3.read();
  button4_last = button4.read();
  button5_last = button5.read();
}
/*
void print_buttons()
{
  Serial.print(button1.read());
  Serial.print(button2.read());
  Serial.print(button3.read());
  Serial.print(button4.read());
  Serial.println(button5.read());
}

void button_test()
{
  long time = millis();
  //"10 second button test beginning."
  output_string(15, 1, 1);
  while (millis() <= time + 10000)
  {
    update_buttons();
    //if (button1_released)
    {
      //("Button 1 released.");
      //output_string(16, 1, 1);
    }

    if (button1_depressed)
    {
      //Serial.println("Button 1 depressed.");
      output_string(17, 1, 1);
    }
  }

}

*/

#include <Wire.h>
//Code from http://www.arduino.cc/playground/Code/I2CEEPROM

//TODO:
//I really should abstract out a little bit and get write_int, read_int, write_block, read_block, write_byte, read_byte
//so I don't have to mention things like EEPROM ADDRESS anywhere else.

void i2c_eeprom_read_16_block(int deviceaddress, unsigned int eeaddress, byte* buffer)
{
  i2c_eeprom_read_buffer(deviceaddress, eeaddress, buffer, 16);
}

void i2c_eeprom_write_16_block(int deviceaddress, unsigned int eeaddress, byte* buffer)
{
  int i;
  byte b;

  for (i = 0; i < 16; i++)
  {
    b = buffer[i];
    i2c_eeprom_write_byte(deviceaddress, eeaddress + i, b);

  }
}

void i2c_eeprom_write_int( int deviceaddress, unsigned int eeaddress, int data )
{
  i2c_eeprom_write_byte(deviceaddress, eeaddress, data>>8);
  i2c_eeprom_write_byte(deviceaddress, eeaddress+1, data & 0xFF);
}

int i2c_eeprom_read_int( int deviceaddress, unsigned int eeaddress )
{
  int i;
  i = i2c_eeprom_read_byte(deviceaddress, eeaddress);
  i = i<<8;
  i = i + i2c_eeprom_read_byte(deviceaddress, eeaddress+1);
  return i;
}


void i2c_eeprom_write_byte( int deviceaddress, unsigned int eeaddress, byte data ) {
  int rdata = data;
  Wire.beginTransmission(deviceaddress);
  Wire.send((int)(eeaddress >> 8)); // MSB
  Wire.send((int)(eeaddress & 0xFF)); // LSB
  Wire.send(rdata);
  Wire.endTransmission();
  delay(10); //voodooish...
}

// WARNING: address is a page address, 6-bit end will wrap around
// also, data can be maximum of about 30 bytes, because the Wire library has a buffer of 32 bytes
void i2c_eeprom_write_page( int deviceaddress, unsigned int eeaddresspage, byte* data, byte length ) {
  Wire.beginTransmission(deviceaddress);
  Wire.send((int)(eeaddresspage >> 8)); // MSB
  Wire.send((int)(eeaddresspage & 0xFF)); // LSB
  byte c;
  for ( c = 0; c < length; c++)
    Wire.send(data[c]);
  Wire.endTransmission();
}

byte i2c_eeprom_read_byte( int deviceaddress, unsigned int eeaddress ) {
  byte rdata = 0xFF;
  Wire.beginTransmission(deviceaddress);
  Wire.send((int)(eeaddress >> 8)); // MSB
  Wire.send((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();
  Wire.requestFrom(deviceaddress,1);
  if (Wire.available()) rdata = Wire.receive();
  return rdata;
}

// maybe let's not read more than 30 or 32 bytes at a time!
void i2c_eeprom_read_buffer( int deviceaddress, unsigned int eeaddress, byte *buffer, int length ) {
  Wire.beginTransmission(deviceaddress);
  Wire.send((int)(eeaddress >> 8)); // MSB
  Wire.send((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();
  Wire.requestFrom(deviceaddress,length);
  int c = 0;
  for ( c = 0; c < length; c++ )
    if (Wire.available()) buffer[c] = Wire.receive();
}

void eeprom_setup()
{
  Wire.begin();
}

/*
// this function will return the number of bytes currently free in RAM
// written by David A. Mellis
// based on code by Rob Faludi http://www.faludi.com
int availableMemory() {
  int size = 1024;
  byte *buf;

  while ((buf = (byte *) malloc(--size)) == NULL)
    ;

  free(buf);

  return size;
}
*/

void game_startup()
{
  output_string(18, 1, 1);
  delay(1000);
  output_string(23, 1, 1);
  delay(900);
}

void display_category(int category)
{
  char buf[16];
  get_category_title(category, buf);
  output_string(buf, 1, 1);
}

void display_word(int category, int word_index)
{
  char buf[16];
  get_word(category, word_index, buf);
  output_string(buf, 1, 1);
}

int next_category(int current_category)
{
  int next_category, category_count;
  category_count = get_category_count();
  if (current_category != category_count - 1)
  {
    next_category = current_category + 1;
  } 
  else
  {
    next_category = 0;
  }

  return next_category;
}

int previous_category(int current_category)
{
  int previous_category, category_count;
  category_count = get_category_count();
  if (current_category != 0)
  {
    previous_category = current_category - 1;
  } 
  else
  {
    previous_category = category_count-1;
  }

  return previous_category;
}

int next_word_index(int category, int current_word_index)
{
  int next_word_index, word_count;
  word_count = get_word_count(category);
  if (current_word_index != word_count - 1)
  {
    next_word_index = current_word_index + 1;
  } 
  else
  {
    next_word_index = 0;
  }

  return next_word_index;
}

int previous_word_index(int category, int current_word_index)
{
  int previous_word_index, word_count;
  word_count = get_word_count(category);
  if (current_word_index != 0)
  {
    previous_word_index = current_word_index - 1;
  } 
  else
  {
    previous_word_index = word_count-1;
  }

  return previous_word_index;
}

  

/*
void lcd_test()
{
  lcd.clear();
  lcd.print("Hello world!");
  delay(500);
  lcd.print("Hello");
  lcd.setCursor(1, 0);
  lcd.print("world!");
  delay(500);
  lcd.clear();
}
*/

#include <avr/pgmspace.h>

#define LARGEST_STRING_LENGTH 60

#define LCD_COLS 16
//help messages start at 0

prog_char string_0[] PROGMEM = "Help:\n";   
prog_char string_1[] PROGMEM = "Words and category titles can only be 15 characters long!";
prog_char string_2[] PROGMEM = "list categories";
prog_char string_3[] PROGMEM = "add category (title)";
prog_char string_4[] PROGMEM = "list words (category-number)";
prog_char string_5[] PROGMEM = "add word (category-number word)";
prog_char string_6[] PROGMEM = "rename category (category-number title)";
prog_char string_7[] PROGMEM = "clear all";
prog_char string_8[] PROGMEM = "clear words (category-number)";
prog_char string_9[] PROGMEM = "list all";
prog_char string_10[] PROGMEM = "Entering USB Mode";   
prog_char string_11[] PROGMEM = "No words in that category";
prog_char string_12[] PROGMEM = "Round over!     Score: ";
prog_char string_13[] PROGMEM = "Tests complete.";
prog_char string_14[] PROGMEM = "No categories defined";
prog_char string_15[] PROGMEM = "10 second button test beginning.";
prog_char string_16[] PROGMEM = "Button 1 released.";
prog_char string_17[] PROGMEM = "Button 1 depressed.";
prog_char string_18[] PROGMEM = "Klugephrase";
prog_char string_19[] PROGMEM = "Choose category:";
prog_char string_20[] PROGMEM = "MAIN SCREEN TURN ON";
prog_char string_21[] PROGMEM = "*buzz*";
prog_char string_22[] PROGMEM = "USB mode";
prog_char string_23[] PROGMEM = "feelslikeburning.com/klugephrase";
prog_char string_24[] PROGMEM = "Paused.";
prog_char string_25[] PROGMEM = "Exiting.";
prog_char string_26[] PROGMEM = "Category full.";
prog_char string_27[] PROGMEM = "No more available categories.";
prog_char string_28[] PROGMEM = "Adding category";
prog_char string_29[] PROGMEM = "Adding word";
prog_char string_30[] PROGMEM = "Clearing";


PGM_P string_table[] PROGMEM =
{   
  string_0,
  string_1,
  string_2,
  string_3,
  string_4,
  string_5,
  string_6,
  string_7,
  string_8,
  string_9,
  string_10,
  string_11,
  string_12,
  string_13,
  string_14,
  string_15,
  string_16,
  string_17,
  string_18,
  string_19,
  string_20,
  string_21,
  string_22,
  string_23,
  string_24,
  string_25,
  string_26,
  string_27,
  string_28,
  string_29,
  string_30,
};

void output_string(int index, int output_serial, int output_lcd)
{
  char buffer[LARGEST_STRING_LENGTH];

  strcpy_P(buffer, (char*)pgm_read_word(&(string_table[index])));
  output_string(buffer, output_serial, output_lcd);

}

void output_string(char* buffer, int output_serial, int output_lcd)
{
  if (output_serial)
  {
    Serial.println(buffer);
  }
  if (output_lcd)
  {
    lcd_print(buffer);
  }
}

void lcd_output_two_lines(char* line1, char* line2)
{
  lcd.clear();
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void lcd_output(char* line1)
{
  lcd.clear();
  lcd.print(line1);
}

void lcd_print(char* buffer)
{
  // TODO untested
  // For printing two lines, need to trim the string at LCD_COLS characters.
  // We save that character to a temp char, edit the buffer to replace it
  // with a '\0', print the string, and then restore that character.
  lcd.clear();
  char temp;
  int len = strlen(buffer);
  if (len > LCD_COLS)
  {
    // print line 1
    temp = buffer[LCD_COLS];
    buffer[LCD_COLS] = '\0';
    lcd.print(buffer);
    buffer[LCD_COLS] = temp;
    // print line 2
    lcd.setCursor(0, 1);
    if (len > (LCD_COLS<<1))
    {
      // too long for lcd, need to trim string
      buffer[LCD_COLS<<1] = '\0';
    }
    lcd.print(buffer+LCD_COLS);
  }
  else // only one line
  {
    // print line 1
    lcd.print(buffer);
  }
}




#define ROUND_OVER_INDEX 12
#define NUMBER_SIZE 6

void display_end_of_round(int round_score)
{
  char number[NUMBER_SIZE];
  itoa(round_score, number, 10);
  char buffer[35];
  strcpy_P(buffer, (char*)pgm_read_word(&(string_table[ROUND_OVER_INDEX])));
  int startpos = strlen(buffer);
  //int i;
  //for (i = 0; i < NUMBER_SIZE; i++);
  //{
  //  buffer[startposi] = number[i];
  //}
  //buffer[startpos+i] = 0;
  strcpy(buffer+22, number);

  output_string(buffer , 1, 1);
  delay(3000);


}



void buzz(int duration, int delay_time)
{
  //not the best way to do it by a long shot...
  long time = millis();
  int i;
  
  //*buzz*
  
  output_string(21, 1, 0);
  while (1)
  {
    if (millis() >= time + duration)
    {
      break;
    }

    for (i = 0; i < 10; i++)
    {

      digitalWrite(SPEAKER, HIGH);
      delay(delay_time);
      digitalWrite(SPEAKER, LOW);
      delay(delay_time);
    }
  }
}
/*
void speaker_test()
{
  int duration = 1000;
  int delay_time;
  for (delay_time = 1; delay_time < 10; delay_time++)
  {
    buzz(duration, delay_time);
    delay(300);
  }
  delay(1000);
  beep();
}
*/
void beep()
{
  buzz(10, 2);
}

#define INPUT_BUFFER_SIZE 29 //this has to be as long as the longest command.

void monitor_serial()
{
  //"Entering USB mode.  Reset or CTRL-C to resume normal operations."
  output_string(22, 1, 1);

  while (1)
  {
    Serial.print("\n> ");
    char input[INPUT_BUFFER_SIZE]; 
    int i;
    byte b;
    int quit = 0;

    for (i = 0; i < INPUT_BUFFER_SIZE-1; )//gotta leave room for that NULL!
    {
      if (Serial.available())
      {
        b = Serial.read();

        if (b == 13 or b == 10)
        {
          //they pressed enter
          input[i] = 0;
          break;
        } 
        else if (b == 3)
        {
          //control-c
          quit = 1;
          break;
        }
        else if (b == 127)
        {
          //backspace

        }
        else {
          input[i] = b;
          Serial.print(b);
          lcd.clear();
          lcd.write(b);
          i++;
        }

      }
    }
    Serial.println();
    //we have either a full string, or they're done with what they have.

    input[INPUT_BUFFER_SIZE] = 0;
    //if (quit)
    //{
    //  output_string(25, 1, 1);
    //  state = 'n';
    //  return;
    //}

    //Serial.println(availableMemory());

    parse_command(input);

  }
}

void parse_command(char* input)
{
  int category;
  char buffer[16];
  char cmd = 0;
  byte b;
  int input_length = strlen(input);
  int i;
  int number;

  if (input[0] == 'c')
  {

    //appending a new category
    if (input_length <= 17)
    {

      //its 16 or shorter (plus null!)
      append_category(input+1, 350);
      output_string(28, 1, 1);
      Serial.println(get_category_count()-1);
    }
  } 
  else if (input[0] >= 48 && input[0] <=57) //is numeric..
  {

    //find first space
    for (i = 0; i<input_length; i++)
    {
      if (input[i] == 32)
      {
        break;
      }
    }

    input[i] = 0;
    number = atoi(input);

    if (input_length <= 17)
    {
      //its 16 or shorter 
      //atoi will return 0 on error, and if its 0.
      int index;
      char buffer[17];
      output_string(29, 1, 1);
            Serial.println(number);
      Serial.println(input+i+1);
      index = append_word(number, input+i+1);
      Serial.println(index);
      get_word(number, index, buffer);
      buffer[16] = 0;
      Serial.println(buffer);

    }
  }
  else if (input[0] == 'x')
  {
    output_string(30, 1, 1);
    //they're clearing the rom
    set_category_count(0);
  }
  Serial.println();
}


void api_help()
{
  for (int i = 0; i < 10; i++)
  {
    output_string(i, 1, 0);
  }
}
/*
void parse_command(char* input)
{
  int category;
  char buffer[16];
  char cmd = 0;
  byte b;
  Serial.println("hullo");
  Serial.println((int)input[0]);
  if (input[0] >= 48 && input[0] <=57)
  {
    Serial.println("hello");
    //the character is a number
    if (input[1] == ' ')
    {
      //they're adding a category
      cmd = 'c';
    } 
    else if (input[1]==';')
    {
      //they're adding a word to this category

      cmd = 'w';
    }
    for (int i=2; i<18; i++)
    {
      b = input[i];
      if (!b)
      {
        //end of string
        buffer[i] = 0;
        break;
      }
      buffer[i] = b;
    }
    //now buffer contains the block
    if (cmd == 'c')
    {
      output_string(28, 1, 0);
      s_add_category(buffer, input[0]-48);
    } 
    else if (cmd == 'w')
    {
      output_string(29, 1, 0);
      append_word(input[0]-48, buffer);
      
    }
  } 
  else if (input[0] == 'x')
  {
    //they're clearing the rom
    set_category_count(0);
  }
  Serial.println("world");
}
*/



void api_list_all()
{
  char number[4];
  char buf[16];
  int index, i, j;
  int category_count = get_category_count();

  for (i = 0; i < category_count; i++)
  {
    Serial.print(itoa(i, number, 10));
    Serial.print(" ");
    get_category_title(i, buf);
    Serial.println(buf);
    Serial.println(get_word_count(i));
    for (j = 0; j < get_word_count(i); j++)
    {
      get_word(i, j, buf);
      Serial.println(buf);
    }
  }
  Serial.println("OK");
}


void api_list_categories(char* input)
{
  char number[4];
  char buf[16];
  int index, i;
  int category_count = get_category_count();
  for (i = 0; i < category_count; i++)
  {
    Serial.print(itoa(i, number, 10));
    Serial.print(" ");
    get_category_title(i, buf);
    Serial.println(buf);
  }
  Serial.println("OK");
}

/*

void api_add_category(char* input)
{
  //add category (title)
}

void api_list_words(char* input)
{

}

void api_add_word(char* input)
{
  //add word (category-number word)
}

void api_rename_category(char* input)
{
}

void api_clear_all(char* input)
{
  //clear all
}

void api_clear_words(char* input)
{
  //clear words (category-number)
}

*/

//every word has a max length of 15 characters plus one \0 at the end.  we store them at regular places in the program.
//we are assuming the buffer you are passing in is of size MAX_LENGTH 16.

//TODO: I should make category a struct...

#define MEMORY_BLOCK_SIZE 16
#define MAX_WORD_LENGTH 16
#define EEPROM_ADDRESS 0x50
#define CATEGORY_COUNT_ADDR 0
#define MAX_CATEGORY_COUNT 32
//there is a maximum of 255 categories because we use a single byte to store the count.
#define CATEGORY_SIZE 64
//everything is done in blocks of 16... there is 1 block for title, 1 block for word starting address, 1 block for word count so 3x16
//= 48


//block 0
// category count
//block 1
//category 0 title
//block 2
//category 0 word count
//block 3
//category 0 first word address
//block 4
//category 0 max_word_count

void set_category_count(int count)
{
  i2c_eeprom_write_byte(EEPROM_ADDRESS, CATEGORY_COUNT_ADDR, count);
}

int get_category_count()
{
  int category_count;

  category_count = i2c_eeprom_read_byte(EEPROM_ADDRESS, CATEGORY_COUNT_ADDR);
  return category_count;
}

void increment_category_count()
{
  int category_count = get_category_count();
  category_count = category_count + 1;

  set_category_count(category_count);
}

int get_category_address(int index)
{
  int category_address;
  category_address = MEMORY_BLOCK_SIZE + (index * CATEGORY_SIZE); // the first block is category count
  return category_address;
}

void get_category_title(int category, char* buffer)
{
  int address = get_category_address(category);
  int i;
  i2c_eeprom_read_16_block(EEPROM_ADDRESS, address, (byte*) buffer);
}

void set_category_title(int category, char* buffer)
{
  int address = get_category_address(category);
  i2c_eeprom_write_16_block(EEPROM_ADDRESS, address, (byte*) buffer);
}

int get_word_count(int category)
{
  int word_count_address = get_category_address(category)+MEMORY_BLOCK_SIZE;
  int word_count = i2c_eeprom_read_int(EEPROM_ADDRESS, word_count_address);
  return word_count;
}

void set_word_count(int category, int count)
{
  int word_count_address;
  word_count_address = get_category_address(category)+MEMORY_BLOCK_SIZE;
  i2c_eeprom_write_int(EEPROM_ADDRESS, word_count_address, count);
}

void increment_word_count(int category)
{
  int current_count, new_count;
  current_count = get_word_count(category);
  new_count = current_count + 1;
  set_word_count(category, new_count);
}

int get_first_word_address(int category)
{
  int location, address;
  location = get_category_address(category) + 2*MEMORY_BLOCK_SIZE;
  address = i2c_eeprom_read_int(EEPROM_ADDRESS, location);
  return address;
}

void set_first_word_address(int category, int address)
{
  int location;
  location = get_category_address(category) + 2*MEMORY_BLOCK_SIZE;
  i2c_eeprom_write_int(EEPROM_ADDRESS, location, address);
}

int get_word_address(int category, int index)
{
  int address; // the address is the return value

  //the address is the address of the first word in that category, plus MEMORY_BLOCK_SIZE*index
  //get the address of the specific word
  address = get_first_word_address(category)+index*MEMORY_BLOCK_SIZE;

  return address;
}

int get_max_word_count(int category)
{
  int word_count_address = get_category_address(category)+3*MEMORY_BLOCK_SIZE;
  int word_count = i2c_eeprom_read_int(EEPROM_ADDRESS, word_count_address);
  return word_count;
}

void set_max_word_count(int category, int count)
{
  int word_count_address;
  word_count_address = get_category_address(category)+3*MEMORY_BLOCK_SIZE;
  i2c_eeprom_write_int(EEPROM_ADDRESS, word_count_address, count);
}

void get_word(int category, int index, char* buffer)
{
  int i;
  int address;
  address = get_word_address(category, index);
  i2c_eeprom_read_16_block(EEPROM_ADDRESS, address, (byte*) buffer);

}

void set_word(int category, int index, char* buffer)
{
  int i;
  int address;
  address = get_word_address(category, index);
  i2c_eeprom_write_16_block(EEPROM_ADDRESS, address, (byte*) buffer);
}

int append_word(int category, char* buffer)
{
  int i, index, address, max_words, word_count;
  max_words = get_max_word_count(category);
  word_count = get_word_count(category);
  if (max_words == word_count)
  {
    //category full
    Serial.println(buffer);
    Serial.println(category);
    Serial.println(max_words);
    Serial.println(word_count);
    output_string(26, 1, 0);
    return -1;
  } 
  else
  {
    index = get_word_count(category);
    address = get_word_address(category, index);
    set_word(category, index, buffer);
    increment_word_count(category);
    return index;
  }
}

void set_category(int index, char* title, int word_count, int first_word_address, int max_word_count)
{
  set_category_title(index, title);
  set_word_count(index, word_count);
  set_first_word_address(index, first_word_address);
  set_max_word_count(index, max_word_count);
} 

int append_category(char* title, int max_word_count)
{
  int category_count, first_word_address;
  int word_count = 0;
  category_count = get_category_count();
  if (category_count == MAX_CATEGORY_COUNT)
  {
    //too many categories
    output_string(27, 1, 0);
    return -1;
  } 
  else if (category_count == 0)
  {
    first_word_address = MEMORY_BLOCK_SIZE+MAX_CATEGORY_COUNT*CATEGORY_SIZE; //remember the first block is current category count...
  } 
  else 
  {
    //we need to find the previous categorie's last possible word slot, and then go one past that.  
    //but don't forget the first block is current category count...
    first_word_address = MEMORY_BLOCK_SIZE+get_word_address(category_count-1, get_max_word_count(category_count-1));
  }
  //at this point, we know the first_word_address, and everything else has been passed in!
  //we also know that we're not out of categories, or you would have left here already.
  increment_category_count();
  set_category(category_count, title, word_count, first_word_address, max_word_count);

}

int s_add_category(char* title, int index)
{
  int first_word_address;
  int word_count = 0;
  if (index >= MAX_CATEGORY_COUNT)
  {
    //too many categories
    output_string(27, 1, 0);
    return -1;
  } 
  else if (index == 0)
  {
    first_word_address = MEMORY_BLOCK_SIZE+MAX_CATEGORY_COUNT*CATEGORY_SIZE; //remember the first block is current category count...
  } 
  else 
  {
    //we need to find the previous categorie's last possible word slot, and then go one past that.  
    //but don't forget the first block is current category count...
    first_word_address = MEMORY_BLOCK_SIZE+get_word_address(index-1, get_max_word_count(index-1));
  }
  //at this point, we know the first_word_address, and everything else has been passed in!
  //we also know that we're not out of categories, or you would have left here already.
  increment_category_count();
  set_category(index, title, word_count, first_word_address, 255);
}
/*
void word_dump()
{
  char buffer[17];
  for (long i=0; i < 2000; i++)
  {
    i2c_eeprom_read_16_block(EEPROM_ADDRESS, MEMORY_BLOCK_SIZE*i,  (byte*) buffer);
    buffer[16] = 0;
    Serial.println(i);
    Serial.println(buffer);
  }
}
*/
  


int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

