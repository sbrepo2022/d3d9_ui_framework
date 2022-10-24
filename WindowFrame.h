/*
Добавить:
1.Текст
2.Новые элементы управления
*/

#include <ctime>
#include <cstring>
#include <d3d9.h>
#include <d3dx9.h>
#include <vector>
#include <string>
#include "StrParse.h"

#define COLOR_ALPHA(color) (color & 0xff000000) / 0xffffff
#define COLOR_RED(color) (color & 0xff0000) / 0xffff
#define COLOR_GREEN(color) (color & 0xff00) / 0xff
#define COLOR_BLUE(color) (color & 0xff)

#define MSG_BUTTONDOWN 0
#define MSG_BUTTONUP 1
#define MSG_MOUSEOVER 2

class Control;

struct ControlState {
  bool is_pushed;
  bool on_rect;
  bool visible, is_locked;
  bool focus;
  ControlState() {is_pushed = 0; on_rect = 0; visible = 1; is_locked = 0; focus = 0;}
};

struct ControlTexts {
  std::string ControlText;
  std::string MsgStr;
};

struct ControlMessage {
  Control *ThisObject;
  UINT message;
  std::string MsgStr;
};

class Control {
protected:
  ControlState state;
  ControlTexts texts;
  UINT ID;
  D3DXVECTOR2 pos;
  D3DXVECTOR2 size;
  
public:
  virtual void set_id(UINT ID) {this->ID = ID;}
  virtual void set_focus(bool focus) {this->state.focus = focus;}//перегружать для групп
  virtual void set_position(D3DXVECTOR2 pos) {this->pos = pos;}//перегружать для групп
  virtual void set_size(D3DXVECTOR2 size) {this->size = size;}//перегружать для групп
  virtual void set_state(ControlState state) {this->state = state;}
  virtual void set_msgstr(std::string MsgStr) {this->texts.MsgStr = MsgStr;}
  virtual UINT get_id() {return this->ID;}
  virtual bool get_focus() {return this->state.focus;}
  virtual D3DXVECTOR2 get_position() {return this->pos;}
  virtual D3DXVECTOR2 get_size() {return this->size;}
  virtual ControlState get_state() {return this->state;}
  virtual std::string get_msgstr() {return this->texts.MsgStr;}
  virtual void show_control(bool visible) {if (! this->state.is_locked) this->state.visible = visible;}
  virtual void lock_control(bool is_locked) {this->state.is_locked = is_locked; this->state.visible = 0;}
  
  virtual void draw(D3DXVECTOR2 offset, D3DXVECTOR2 boundpos1, D3DXVECTOR2 boundpos2) = 0;
  virtual bool button_down(D3DXVECTOR2 CursorPos, D3DXVECTOR2 offset, ControlMessage *floatmsg) = 0;
  virtual bool button_up(D3DXVECTOR2 CursorPos, D3DXVECTOR2 offset, ControlMessage *floatmsg) = 0;
  virtual bool mouse_move(D3DXVECTOR2 CursorPos, D3DXVECTOR2 offset, ControlMessage *floatmsg) = 0;
  virtual void rect_out() = 0;
};

class WindowManager {
public:
  virtual void set_elements(const std::vector<Control*> &controls, D3DXVECTOR2 pos, D3DXVECTOR2 size);
};

class PaintState {
protected:
  IDirect3DDevice9 *pDevice;
  IDirect3DSurface9 *BuffSurface, *BackSurface;
  IDirect3DTexture9 *TextureBuffer;
  
public:
  void init(IDirect3DDevice9 *pDevice);
  void BeginPaint();
  void EndPaint();
  void print(D3DXVECTOR2 boundpos1, D3DXVECTOR2 boundpos2, D3DXVECTOR2 offset);
};

class WindowAnimation {
protected:
  PaintState PaintObject;
  
public:
  virtual void draw(Control *control, D3DXVECTOR2 offset, D3DXVECTOR2 boundpos1, D3DXVECTOR2 boundpos2){}
};

class WindowFrame {
protected:
  Control *MainControl;
  
public:
  WindowFrame() {MainControl = NULL;}
  virtual void init(Control *MainControl, D3DXVECTOR2 pos, D3DXVECTOR2 size);

  virtual void set_position(D3DXVECTOR2 pos) {if (MainControl != NULL) this->MainControl->set_position(pos);}
  virtual void set_size(D3DXVECTOR2 size) {if (MainControl != NULL) this->MainControl->set_size(size);}
  virtual D3DXVECTOR2 get_position() {if (MainControl != NULL) return this->MainControl->get_position();}
  virtual D3DXVECTOR2 get_size() {if (MainControl != NULL) return this->MainControl->get_size();}
  virtual void show_control(bool visible) {if (MainControl != NULL) MainControl->show_control(visible);}

  virtual void draw();
  virtual bool button_down(D3DXVECTOR2 CursorPos);
  virtual bool button_up(D3DXVECTOR2 CursorPos);
  virtual bool mouse_move(D3DXVECTOR2 CursorPos);
  virtual void rect_out();
};

//------------

class Button : public Control {
protected:
  WindowAnimation *animation;
  void (*callbackf)(ControlMessage);

public:
  Button() {this->animation = NULL; this->callbackf = NULL; this->state.visible = true;}
  void init(UINT ID, D3DXVECTOR2 pos, D3DXVECTOR2 size, WindowAnimation *animation);
  void draw(D3DXVECTOR2 offset, D3DXVECTOR2 boundpos1, D3DXVECTOR2 boundpos2);
  
  bool button_down(D3DXVECTOR2 CursorPos, D3DXVECTOR2 offset, ControlMessage *floatmsg);
  bool button_up(D3DXVECTOR2 CursorPos, D3DXVECTOR2 offset, ControlMessage *floatmsg);
  bool mouse_move(D3DXVECTOR2 CursorPos, D3DXVECTOR2 offset, ControlMessage *floatmsg);
  void rect_out() {this->state.on_rect = 0;}
  
  void set_callbackf(void (*callbackf)(ControlMessage));
};

class Group : public Control {
protected:
  std::vector<Control*> controls;
  WindowAnimation *animation;
  WindowManager *manager;

public:
  Group() {this->animation = NULL; this->manager = NULL; this->state.visible = true;}
  void init(UINT ID, D3DXVECTOR2 pos, D3DXVECTOR2 size, WindowAnimation *animation, WindowManager *manager);
  void draw(D3DXVECTOR2 offset, D3DXVECTOR2 boundpos1, D3DXVECTOR2 boundpos2);
  
  void set_focus(bool focus); //перегрузка
  void set_position(D3DXVECTOR2 pos); //перегрузка
  void set_size(D3DXVECTOR2 size); //перегрузка
  
  bool button_down(D3DXVECTOR2 CursorPos, D3DXVECTOR2 offset, ControlMessage *floatmsg);
  bool button_up(D3DXVECTOR2 CursorPos, D3DXVECTOR2 offset, ControlMessage *floatmsg);
  bool mouse_move(D3DXVECTOR2 CursorPos, D3DXVECTOR2 offset, ControlMessage *floatmsg);
  void rect_out();
  
  void add_control(Control *new_control);
};


class Tab : public Control {
protected:
  Group *switches;
  std::vector<Control*> tabs;
  WindowAnimation *animation;
  WindowManager *manager;
  
  int get_tab_index(std::string str);
  
public:
  Tab() {switches = NULL; this->animation = NULL; this->manager = NULL; this->state.visible = true;}
  void init(Group *switches, UINT ID, D3DXVECTOR2 pos, D3DXVECTOR2 size, WindowAnimation *animation, WindowManager *manager);
  void draw(D3DXVECTOR2 offset, D3DXVECTOR2 boundpos1, D3DXVECTOR2 boundpos2);
  
  void set_focus(bool focus); //перегрузка
  void set_position(D3DXVECTOR2 pos); //перегрузка
  void set_size(D3DXVECTOR2 size); //перегрузка
  
  bool button_down(D3DXVECTOR2 CursorPos, D3DXVECTOR2 offset, ControlMessage *floatmsg);
  bool button_up(D3DXVECTOR2 CursorPos, D3DXVECTOR2 offset, ControlMessage *floatmsg);
  bool mouse_move(D3DXVECTOR2 CursorPos, D3DXVECTOR2 offset, ControlMessage *floatmsg);
  void rect_out();
  
  void set_controls(Control *SwitchButton, Control *ElemGroup);
};
//------

