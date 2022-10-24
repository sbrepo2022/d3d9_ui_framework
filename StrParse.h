#pragma once

#include <string>
#include <cstring>

//разбор строки на лексемы
class StrParse {
protected:
  std::string str;
  char separate;
  
public:
  StrParse(){}
  StrParse(std::string str, char separate);
  void set_text(std::string str);
  void set_separate(char separate);
  void get_lexem(char *lexem, int max_leight, int index);
  int size();
};
