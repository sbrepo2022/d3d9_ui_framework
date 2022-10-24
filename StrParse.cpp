#include "StrParse.h"

StrParse::StrParse(std::string str, char separate) {
  this->str = str;
  this->separate = separate;
}

void StrParse::set_text(std::string str) {
  this->str = str;
}

void StrParse::set_separate(char separate) {
  this->separate = separate;
}

void StrParse::get_lexem(char *lexem, int max_leight, int index) {
  int i = 0, pos = 0, index_pos = 0;
  
  strcpy(lexem, "");
  
  while (this->str[i] != '\0' && index_pos <= index && pos < max_leight) {
    if (str[i] == this->separate) {
      index_pos++;
      i++;
      continue;
    }
    
    if (index_pos == index) {
      lexem[pos] = this->str[i];
      if (str[i + 1] == this->separate || str[i + 1] == '\0')
        lexem[pos + 1] = '\0';
      pos++;
    }
    
    i++;
  }
}

int StrParse::size() {
  int i = 0, max_index = 0;
  while (this->str[i] != '\0') {
    if (str[i] == this->separate) {
      max_index++;
    }
    i++;
  }
  return max_index + 1;
}
