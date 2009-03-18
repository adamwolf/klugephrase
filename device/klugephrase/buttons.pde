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
