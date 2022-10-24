#include "WindowFrame.h"

void WindowManager::set_elements(const std::vector<Control*> &controls, D3DXVECTOR2 pos, D3DXVECTOR2 size) {
  for (int i = 0; i < controls.size(); i++) {
    controls[i]->set_position(controls[i]->get_position());
    controls[i]->set_size(controls[i]->get_size());
  }
}

void PaintState::init(IDirect3DDevice9 *pDevice) {
  this->pDevice = pDevice;
  
  IDirect3DSurface9 *BackSurface;
  pDevice->GetRenderTarget(0, &BackSurface);
  
  D3DSURFACE_DESC info;
  BackSurface->GetDesc(&info);
  
  pDevice->CreateTexture(info.Width,info.Height,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&(this->TextureBuffer),NULL);
}

void PaintState::BeginPaint() {
  TextureBuffer->GetSurfaceLevel(0, &(this->BuffSurface));
  pDevice->GetRenderTarget(0, &(this->BackSurface));
  pDevice->SetRenderTarget(0, this->BuffSurface);
  
  pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
}

void PaintState::EndPaint() {
  pDevice->SetRenderTarget(0, this->BackSurface);
}

void PaintState::print(D3DXVECTOR2 boundpos1, D3DXVECTOR2 boundpos2, D3DXVECTOR2 offset) {
  RECT bound;
  bound.left = boundpos1.x;
  bound.top = boundpos1.y;
  bound.right = boundpos2.x;
  bound.bottom = boundpos2.y;
  
  ID3DXSprite *Sprite;
  D3DXMATRIX Mat;
  D3DXMatrixTransformation2D(&Mat, NULL, 0, NULL, NULL, 0, &offset);
  D3DXCreateSprite(pDevice, &Sprite);
  Sprite->Begin(0);
  Sprite->SetTransform(&Mat);
  Sprite->Draw(this->TextureBuffer, &bound, NULL, NULL, 0xFFFFFFFF);
  Sprite->End();
  Sprite->Release();
}

void WindowFrame::init(Control *MainControl, D3DXVECTOR2 pos, D3DXVECTOR2 size) {
  this->MainControl = MainControl;
  if (MainControl != NULL) {
    this->MainControl->set_position(pos);
    this->MainControl->set_size(size);
  }
}

void WindowFrame::draw() {
  if (MainControl != NULL)
    if (this->MainControl->get_state().visible)
      MainControl->draw(D3DXVECTOR2(0, 0), D3DXVECTOR2(0, 0), this->MainControl->get_position() + this->MainControl->get_size());
}

bool WindowFrame::button_down(D3DXVECTOR2 CursorPos) {
  ControlMessage FloatMessage;
  bool ReturnValue = 0;
  
  if (MainControl != NULL) 
    if (MainControl->button_down(CursorPos, D3DXVECTOR2(0, 0), &FloatMessage)) {
      MainControl->set_focus(false);
      FloatMessage.ThisObject->set_focus(true);
      ReturnValue = true;
    }
  return ReturnValue;
}

bool WindowFrame::button_up(D3DXVECTOR2 CursorPos) {
  ControlMessage FloatMessage;
  bool ReturnValue = 0;
  
  if (MainControl != NULL) 
    if (MainControl->button_up(CursorPos, D3DXVECTOR2(0, 0), &FloatMessage)) {
      ReturnValue = true;
    }
  return ReturnValue;
}

bool WindowFrame::mouse_move(D3DXVECTOR2 CursorPos) {
  ControlMessage FloatMessage;
  bool ReturnValue = 0;
  
  if (MainControl != NULL) 
    if (MainControl->mouse_move(CursorPos, D3DXVECTOR2(0, 0), &FloatMessage)) {
      ReturnValue = true;
    }
  return ReturnValue;
}

void WindowFrame::rect_out() {
  if (MainControl != NULL) 
    MainControl->rect_out();
}

void Button::init(UINT ID, D3DXVECTOR2 pos, D3DXVECTOR2 size, WindowAnimation *animation) {
  this->ID = ID;
  this->pos = pos;
  this->size = size;
  this->animation = animation;
}

void Button::draw(D3DXVECTOR2 offset, D3DXVECTOR2 boundpos1, D3DXVECTOR2 boundpos2) {
  if (this->state.visible) {
    if (this->animation != NULL)
      this->animation->draw(this, offset, boundpos1, boundpos2);
  }
}

bool Button::button_down(D3DXVECTOR2 CursorPos, D3DXVECTOR2 offset, ControlMessage *floatmsg) {
  bool ReturnValue = 0;
  ControlMessage msg;
  msg.ThisObject = this;
  msg.message = MSG_BUTTONDOWN;
  msg.MsgStr = this->texts.MsgStr;
  
  if (CursorPos.x > this->pos.x + offset.x && CursorPos.y > this->pos.y + offset.y &&
      CursorPos.x < this->pos.x + this->size.x + offset.x &&
      CursorPos.y < this->pos.y + this->size.y + offset.y && this->state.visible) {
    this->state.is_pushed = true;
    *floatmsg = msg;
    if (callbackf != NULL)
      (*callbackf)(msg);
    ReturnValue = 1;
  }
  return ReturnValue;
}

bool Button::button_up(D3DXVECTOR2 CursorPos, D3DXVECTOR2 offset, ControlMessage *floatmsg) {
  bool ReturnValue = 0;
  ControlMessage msg;
  msg.ThisObject = this;
  msg.message = MSG_BUTTONUP;
  msg.MsgStr = this->texts.MsgStr;
  
  if (CursorPos.x > this->pos.x + offset.x && CursorPos.y > this->pos.y + offset.y &&
      CursorPos.x < this->pos.x + this->size.x + offset.x &&
      CursorPos.y < this->pos.y + this->size.y + offset.y && this->state.visible) {
    if (this->state.is_pushed && this->state.on_rect) {
      *floatmsg = msg;
      if (callbackf != NULL)
        (*callbackf)(msg);
      ReturnValue = 1;
    }
  }
  this->state.is_pushed = false;
  
  return ReturnValue;
}

bool Button::mouse_move(D3DXVECTOR2 CursorPos, D3DXVECTOR2 offset, ControlMessage *floatmsg) {
  bool ReturnValue = 0;
  ControlMessage msg;
  msg.ThisObject = this;
  msg.message = MSG_MOUSEOVER;
  msg.MsgStr = this->texts.MsgStr;
  
  if (CursorPos.x > this->pos.x + offset.x && CursorPos.y > this->pos.y + offset.y &&
      CursorPos.x < this->pos.x + this->size.x + offset.x &&
      CursorPos.y < this->pos.y + this->size.y + offset.y && this->state.visible) {
    this->state.on_rect = true;
    *floatmsg = msg;
    if (callbackf != NULL)
      (*callbackf)(msg);
    ReturnValue = 1;
  }
  else {
    this->rect_out();
  }
  return ReturnValue;
}

void Button::set_callbackf(void (*callbackf)(ControlMessage)) {
  this->callbackf = callbackf;
}

void Group::init(UINT ID, D3DXVECTOR2 pos, D3DXVECTOR2 size, WindowAnimation *animation, WindowManager *manager) {
  this->ID = ID;
  this->pos = pos;
  this->size = size;
  this->animation = animation;
  this->manager = manager;
}

void Group::draw(D3DXVECTOR2 offset, D3DXVECTOR2 boundpos1, D3DXVECTOR2 boundpos2) {
  D3DXVECTOR2 specpos1 = this->pos + offset, specpos2 = this->pos + this->size + offset;
  if (specpos1.x < boundpos1.x) specpos1.x = boundpos1.x;
  if (specpos1.y < boundpos1.y) specpos1.y = boundpos1.y;
  if (specpos2.x > boundpos2.x) specpos2.x = boundpos2.x;
  if (specpos2.y > boundpos2.y) specpos2.y = boundpos2.y;
  
  if (this->state.visible) {
    if (this->animation != NULL)
      this->animation->draw(this, offset, boundpos1, boundpos2);
    for (int i = 0; i < this->controls.size(); i++) {
      this->controls[i]->draw(this->pos + offset, specpos1, specpos2);
    }
  }
}

void Group::set_focus(bool focus) {
  for (int i = 0; i < this->controls.size(); i++) {
    this->controls[i]->set_focus(focus);
  }
}

void Group::set_position(D3DXVECTOR2 pos) {
  this->pos = pos;
  if (this->manager != NULL)
    this->manager->set_elements(this->controls, this->pos, this->size);
}

void Group::set_size(D3DXVECTOR2 size) {
  this->size = size;
  if (this->manager != NULL)
    this->manager->set_elements(this->controls, this->pos, this->size);
}

bool Group::button_down(D3DXVECTOR2 CursorPos, D3DXVECTOR2 offset, ControlMessage *floatmsg) {
  bool ReturnValue = 0;
  
  if (CursorPos.x > this->pos.x + offset.x && CursorPos.y > this->pos.y + offset.y &&
      CursorPos.x < this->pos.x + this->size.x + offset.x &&
      CursorPos.y < this->pos.y + this->size.y + offset.y && this->state.visible) {
    this->state.is_pushed = true;
    for (int i = 0; i < this->controls.size(); i++) {
      if (this->controls[i]->button_down(CursorPos, this->pos + offset, floatmsg))
        ReturnValue = 1; 
    }
  }
  
  return ReturnValue;
}

bool Group::button_up(D3DXVECTOR2 CursorPos, D3DXVECTOR2 offset, ControlMessage *floatmsg) {
  bool ReturnValue = 0;
  
  this->state.is_pushed = false;
  if (CursorPos.x > this->pos.x + offset.x && CursorPos.y > this->pos.y + offset.y &&
      CursorPos.x < this->pos.x + this->size.x + offset.x &&
      CursorPos.y < this->pos.y + this->size.y + offset.y && this->state.visible) {
  }
  
  for (int i = 0; i < this->controls.size(); i++) {
    if (this->controls[i]->button_up(CursorPos, this->pos + offset, floatmsg))
      ReturnValue = 1;
  }
  return ReturnValue;
}

bool Group::mouse_move(D3DXVECTOR2 CursorPos, D3DXVECTOR2 offset, ControlMessage *floatmsg) {
  bool ReturnValue = 0;
  
  if (CursorPos.x > this->pos.x + offset.x && CursorPos.y > this->pos.y + offset.y &&
      CursorPos.x < this->pos.x + this->size.x + offset.x &&
      CursorPos.y < this->pos.y + this->size.y + offset.y && this->state.visible) {
    this->state.on_rect = true;
    for (int i = 0; i < this->controls.size(); i++) {
      if (this->controls[i]->mouse_move(CursorPos, this->pos + offset, floatmsg))
        ReturnValue = 1;
    }
  }
  else {
    this->rect_out();
  }
  
  return ReturnValue;
}

void Group::rect_out() {
  this->state.on_rect = false;
  for (int i = 0; i < this->controls.size(); i++) {
    this->controls[i]->rect_out();
  }
}

void Group::add_control(Control *new_control) {
  this->controls.push_back(new_control);
}

int Tab::get_tab_index(std::string str) {
  StrParse Parser1(str, ';');
  char str2[11] = "";
  Parser1.get_lexem(str2, 10, 0);
  if (strcmp(str2, "showtab") == 0) {
    Parser1.get_lexem(str2, 10, 1);
    return atoi(str2);
  }
  else
    return -1;
}

void Tab::init(Group *switches, UINT ID, D3DXVECTOR2 pos, D3DXVECTOR2 size, WindowAnimation *animation, WindowManager *manager) {
  this->switches = switches;
  this->ID = ID;
  this->pos = pos;
  this->size = size;
  this->animation = animation;
  this->manager = manager;
}

void Tab::draw(D3DXVECTOR2 offset, D3DXVECTOR2 boundpos1, D3DXVECTOR2 boundpos2) {
  D3DXVECTOR2 specpos1 = this->pos + offset, specpos2 = this->pos + this->size + offset;
  if (specpos1.x < boundpos1.x) specpos1.x = boundpos1.x;
  if (specpos1.y < boundpos1.y) specpos1.y = boundpos1.y;
  if (specpos2.x > boundpos2.x) specpos2.x = boundpos2.x;
  if (specpos2.y > boundpos2.y) specpos2.y = boundpos2.y;
  
  if (this->state.visible) {
    if (this->animation != NULL)
      this->animation->draw(this, offset, boundpos1, boundpos2);
    for (int i = 0; i < this->tabs.size(); i++) {
      this->tabs[i]->draw(this->pos + offset, specpos1, specpos2);
    }
    if (this->switches != NULL)
      switches->draw(this->pos + offset, specpos1, specpos2);
  }
}

void Tab::set_focus(bool focus) {
  if (this->switches != NULL)
    this->switches->set_focus(focus);
  for (int i = 0; i < this->tabs.size(); i++) {
    this->tabs[i]->set_focus(focus);
  }
}

void Tab::set_position(D3DXVECTOR2 pos) {
  this->pos = pos;
  std::vector<Control*> controls;
  controls = this->tabs;
  controls.insert(controls.begin(), this->switches);
  if (this->manager != NULL)
    this->manager->set_elements(controls, this->pos, this->size);
}

void Tab::set_size(D3DXVECTOR2 size) {
  this->size = size;
  std::vector<Control*> controls;
  controls = this->tabs;
  controls.insert(controls.begin(), this->switches);
  if (this->manager != NULL)
    this->manager->set_elements(controls, this->pos, this->size);
}

bool Tab::button_down(D3DXVECTOR2 CursorPos, D3DXVECTOR2 offset, ControlMessage *floatmsg) {
  bool ReturnValue = 0;
  int Index;
  
  if (CursorPos.x > this->pos.x + offset.x && CursorPos.y > this->pos.y + offset.y &&
      CursorPos.x < this->pos.x + this->size.x + offset.x &&
      CursorPos.y < this->pos.y + this->size.y + offset.y && this->state.visible) {
    this->state.is_pushed = true;
    
    if (this->switches != NULL)
      if (this->switches->button_down(CursorPos, this->pos + offset, floatmsg))
        ReturnValue = 1;
    for (int i = 0; i < this->tabs.size(); i++) {
      if (this->tabs[i]->button_down(CursorPos, this->pos + offset, floatmsg))
        ReturnValue = 1; 
    }
  }

  return ReturnValue;
}

bool Tab::button_up(D3DXVECTOR2 CursorPos, D3DXVECTOR2 offset, ControlMessage *floatmsg) {
  bool ReturnValue = 0;
  int Index;
  
  this->state.is_pushed = false;
  if (CursorPos.x > this->pos.x + offset.x && CursorPos.y > this->pos.y + offset.y &&
      CursorPos.x < this->pos.x + this->size.x + offset.x &&
      CursorPos.y < this->pos.y + this->size.y + offset.y && this->state.visible) {
  }
  
  if (this->switches != NULL)
    if (this->switches->button_up(CursorPos, this->pos + offset, floatmsg)) {
      ReturnValue = 1;
      Index = get_tab_index(floatmsg->ThisObject->get_msgstr());
      if (Index != -1) {
        for (int i = 0; i < this->tabs.size(); i++)
          tabs[i]->show_control(false);
        tabs[Index]->show_control(true);
      }
    }
  for (int i = 0; i < this->tabs.size(); i++) {
    if (this->tabs[i]->button_up(CursorPos, this->pos + offset, floatmsg))
      ReturnValue = 1; 
  }

  return ReturnValue;
}

bool Tab::mouse_move(D3DXVECTOR2 CursorPos, D3DXVECTOR2 offset, ControlMessage *floatmsg) {
  bool ReturnValue = 0;
  int Index;
  
  if (CursorPos.x > this->pos.x + offset.x && CursorPos.y > this->pos.y + offset.y &&
      CursorPos.x < this->pos.x + this->size.x + offset.x &&
      CursorPos.y < this->pos.y + this->size.y + offset.y && this->state.visible) {
    this->state.on_rect = true;
    if (this->switches != NULL)
      if (this->switches->mouse_move(CursorPos, this->pos + offset, floatmsg))
        ReturnValue = 1;
    for (int i = 0; i < this->tabs.size(); i++) {
      if (this->tabs[i]->mouse_move(CursorPos, this->pos + offset, floatmsg))
        ReturnValue = 1; 
    }
  }
  else {
    this->rect_out();
  }

  return ReturnValue;
}

void Tab::rect_out() {
  this->state.on_rect = false;
  if (this->switches != NULL)
    this->switches->rect_out();
  for (int i = 0; i < this->tabs.size(); i++) {
    this->tabs[i]->rect_out(); 
  }
}

void Tab::set_controls(Control *SwitchButton, Control *ElemGroup) {
  std::string str("showtab");
  char str2[11];
  this->tabs.push_back(ElemGroup);
  if (this->switches != NULL) {
    str += ";";
    itoa(tabs.size() - 1, str2, 10);
    str += str2;
    SwitchButton->set_msgstr(str);
    this->switches->add_control(SwitchButton);
  }
}

