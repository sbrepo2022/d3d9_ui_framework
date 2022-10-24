/*
Добавить:
1.Текст
2.Новые элементы управления
*/

#include <ctime>
#include <d3d9.h>
#include <d3dx9.h>
#include <vector>

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
  bool visible;
  bool focus;
  ControlState() {is_pushed = 0; on_rect = 0; visible = 1; focus = 0;}
};

struct ControlMessage {
  Control *ThisObject;
  UINT message;
};

class Control {
protected:
  ControlState state;
  UINT ID;
  D3DXVECTOR2 pos;
  D3DXVECTOR2 size;
  
public:
  virtual void set_id(UINT ID) {this->ID = ID;}
  virtual void set_focus(bool focus) {this->state.focus = focus;}
  virtual void set_position(D3DXVECTOR2 pos) {this->pos = pos;}
  virtual void set_size(D3DXVECTOR2 size) {this->size = size;}
  virtual void set_state(ControlState state) {this->state = state;}
  virtual UINT get_id() {return this->ID;}
  virtual bool get_focus() {return this->state.focus;}
  virtual D3DXVECTOR2 get_position() {return this->pos;}
  virtual D3DXVECTOR2 get_size() {return this->size;}
  virtual ControlState get_state() {return this->state;}
  virtual void show_control(bool visible) {this->state.visible = visible;}
  
  virtual void draw(D3DXVECTOR2 offset) = 0;
  virtual bool button_down(D3DXVECTOR2 CursorPos, D3DXVECTOR2 offset, ControlMessage *floatmsg) = 0;
  virtual bool button_up(D3DXVECTOR2 CursorPos, D3DXVECTOR2 offset, ControlMessage *floatmsg) = 0;
  virtual bool mouse_move(D3DXVECTOR2 CursorPos, D3DXVECTOR2 offset, ControlMessage *floatmsg) = 0;
};

class WindowManager {
public:
  virtual void set_elements(const std::vector<Control*> &controls, D3DXVECTOR2 pos, D3DXVECTOR2 size){}
};

class WindowAnimation {
public:
  virtual void draw(Control *control, D3DXVECTOR2 offset){}
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
};

//------------

class Button : public Control {
protected:
  WindowAnimation *animation;
  void (*callbackf)(ControlMessage);

public:
  Button() {this->animation = NULL; this->callbackf = NULL; this->state.visible = true;}
  void init(UINT ID, D3DXVECTOR2 pos, D3DXVECTOR2 size, WindowAnimation *animation);
  void draw(D3DXVECTOR2 offset);
  
  bool button_down(D3DXVECTOR2 CursorPos, D3DXVECTOR2 offset, ControlMessage *floatmsg);
  bool button_up(D3DXVECTOR2 CursorPos, D3DXVECTOR2 offset, ControlMessage *floatmsg);
  bool mouse_move(D3DXVECTOR2 CursorPos, D3DXVECTOR2 offset, ControlMessage *floatmsg);
  
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
  void draw(D3DXVECTOR2 offset);
  
  void set_focus(bool focus); //перегрузка
  void set_position(D3DXVECTOR2 pos); //перегрузка
  void set_size(D3DXVECTOR2 size); //перегрузка
  
  bool button_down(D3DXVECTOR2 CursorPos, D3DXVECTOR2 offset, ControlMessage *floatmsg);
  bool button_up(D3DXVECTOR2 CursorPos, D3DXVECTOR2 offset, ControlMessage *floatmsg);
  bool mouse_move(D3DXVECTOR2 CursorPos, D3DXVECTOR2 offset, ControlMessage *floatmsg);
  
  void add_control(Control *new_control);
};

//------

class WindowManagerVertical1 : public WindowManager {  
protected:
  int WHratio;

public:
  void init(int WHratio) {this->WHratio = WHratio;}
  void set_elements(const std::vector<Control*> &controls, D3DXVECTOR2 pos, D3DXVECTOR2 size);
};

class WindowAnimationSimple : public WindowAnimation {
protected:
  IDirect3DDevice9 *pDevice;
  IDirect3DTexture9 *Texture;
  
public:
  void init(IDirect3DDevice9 *pDevice, IDirect3DTexture9 *Texture);
  void draw(Control *control, D3DXVECTOR2 offset);
};

class WindowAnimationType1 : public WindowAnimation {
protected:
  IDirect3DDevice9 *pDevice;
  IDirect3DTexture9 *Texture1, *Texture2;
  
public:
  void init(IDirect3DDevice9 *pDevice, IDirect3DTexture9 *Texture1, IDirect3DTexture9 *Texture2);
  void draw(Control *control, D3DXVECTOR2 offset);
};

class WindowAnimationType2 : public WindowAnimation {
protected:
  IDirect3DDevice9 *pDevice;
  IDirect3DTexture9 *Texture, *TexturePict;
  float AnimationTime, CurrentAnimationTime, LastTick;
  DWORD color1, color2;
  
public:
  WindowAnimationType2() {AnimationTime = 0.001; LastTick = 0; CurrentAnimationTime = 0;}
  void init(IDirect3DDevice9 *pDevice, IDirect3DTexture9 *TexturePict, DWORD color1, DWORD color2, float AnimationTime);
  void draw(Control *control, D3DXVECTOR2 offset);
};

