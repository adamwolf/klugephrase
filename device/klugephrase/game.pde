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

  
