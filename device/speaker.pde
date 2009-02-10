
void speaker_setup()
{
  pinMode(SPEAKER, OUTPUT);
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

void beep()
{
  buzz(10, 2);
}
