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
