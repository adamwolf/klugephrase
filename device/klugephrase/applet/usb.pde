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
