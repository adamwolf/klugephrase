#include <LiquidCrystal.h>

#include <Debounce.h>

#define BUTTON1 9
#define BUTTON2 10
#define BUTTON3 11
#define BUTTON4 12
#define BUTTON5 13

#define INITIAL_BUZZ_DELAY 750
#define ROUND_LENGTH  60000


#define RS 2
#define RW 3
#define E 4
#define DB4 5
#define DB5 8
#define DB6 7
#define DB7 6

#define BUTTON1 9
#define BUTTON2 10
#define BUTTON3 11
#define BUTTON4 12
#define BUTTON5 13

#define SPEAKER 14

#define EEPROM_ADDRESS 0x50

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




void test()
{
  //lcd_test();
  //button_test();
  //speaker_test();
  //word_test();
  //"Tests complete."
  output_string(13, 1, 1);
}

void init_memory()
{
}


void setup()
{
  Serial.begin(9600);
  eeprom_setup();
  button_setup();
  speaker_setup();
  Serial.println(availableMemory());
  test();
  state = 'n';
  
  init_memory();
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
      //Serial.println("no words");

      state = 'x';
    } 
    else {
      //start picking the categories
      category = 0;
      state = 'c';
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
        word = 0;
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
      display_end_of_round(round_score);
      buzz(250, 4);

      state = 'c';
      display_category(category);
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
      word = next_word_index(category, word);
      display_word(category, word);
    } 
    else if (button3_depressed)
    {
      //go back to the last word
      word = previous_word_index(category, word);
      display_word(category, word);
    } 
    else if (button5_depressed)
    {
      //advance word and increment score
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
