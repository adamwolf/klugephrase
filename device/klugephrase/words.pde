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
  

