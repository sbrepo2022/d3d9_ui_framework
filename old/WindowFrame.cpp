#include "WindowFrame.h"

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
      MainControl->draw(D3DXVECTOR2(0, 0));
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

void Button::init(UINT ID, D3DXVECTOR2 pos, D3DXVECTOR2 size, WindowAnimation *animation) {
  this->ID = ID;
  this->pos = pos;
  this->size = size;
  this->animation = animation;
}

void Button::draw(D3DXVECTOR2 offset) {
  if (this->state.visible) {
    if (this->animation != NULL)
      this->animation->draw(this, offset);
  }
}

bool Button::button_down(D3DXVECTOR2 CursorPos, D3DXVECTOR2 offset, ControlMessage *floatmsg) {
  bool ReturnValue = 0;
  ControlMessage msg;
  msg.ThisObject = this;
  msg.message = MSG_BUTTONDOWN;
  
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
  
  if (CursorPos.x > this->pos.x + offset.x && CursorPos.y > this->pos.y + offset.y &&
      CursorPos.x < this->pos.x + this->size.x + offset.x &&
      CursorPos.y < this->pos.y + this->size.y + offset.y && this->state.visible) {
    if (this->state.is_pushed == true) {
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
    this->state.on_rect = false;
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

void Group::draw(D3DXVECTOR2 offset) {
  if (this->state.visible) {
    if (this->animation != NULL)
      this->animation->draw(this, offset);
    for (int i = 0; i < this->controls.size(); i++) {
      this->controls[i]->draw(this->pos + offset);
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
  }
  
  for (int i = 0; i < this->controls.size(); i++) {
    if (this->controls[i]->button_down(CursorPos, this->pos + offset, floatmsg))
      ReturnValue = 1; 
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
  }
  else {
    this->state.on_rect = false;
  }
  
  for (int i = 0; i < this->controls.size(); i++) {
    if (this->controls[i]->mouse_move(CursorPos, this->pos + offset, floatmsg))
      ReturnValue = 1;
  }
  return ReturnValue;
}

void Group::add_control(Control *new_control) {
  this->controls.push_back(new_control);
}

void WindowManagerVertical1::set_elements(const std::vector<Control*> &controls, D3DXVECTOR2 pos, D3DXVECTOR2 size) {
  float NumElems = controls.size() + (controls.size() - 1) / 2 + 2;
  int ElemHeight = size.y / NumElems;
  int ElemWidth = ElemHeight * WHratio;
  int offset = ElemHeight;
  
  for (int i = 0; i < controls.size(); i++) {
    controls[i]->set_position(D3DXVECTOR2(size.x / 2 - ElemWidth / 2, offset));
    controls[i]->set_size(D3DXVECTOR2(ElemWidth, ElemHeight));
    offset += ElemHeight * 1.5;
  }
}

void WindowAnimationSimple::init(IDirect3DDevice9 *pDevice, IDirect3DTexture9 *Texture) {
  this->pDevice = pDevice;
  this->Texture = Texture;
}

void WindowAnimationSimple::draw(Control *control, D3DXVECTOR2 offset) {
  D3DSURFACE_DESC info;
  ID3DXSprite *Sprite;
  D3DXMATRIX Mat;
  
  Texture->GetLevelDesc(0, &info);
  
  D3DXVECTOR2 ScalingCenter(0, 0);
  D3DXVECTOR2 Scaling;
  if (control->get_size().x != 0)
    Scaling.x = control->get_size().x / info.Width;
  else
    Scaling.x = 1;
  
  if (control->get_size().y != 0)
    Scaling.y = control->get_size().y / info.Height;
  else
    Scaling.y = 1;
  D3DXVECTOR2 drawingpos = offset + control->get_position();
  D3DXMatrixTransformation2D(&Mat, &ScalingCenter, 0, &Scaling, NULL, 0, &drawingpos);
  
  D3DXCreateSprite(pDevice, &Sprite);
  Sprite->Begin(0);
  Sprite->SetTransform(&Mat);
  Sprite->Draw(Texture, NULL, NULL, NULL, 0xFFFFFFFF);
  Sprite->End();
  Sprite->Release();
}

void WindowAnimationType1::init(IDirect3DDevice9 *pDevice, IDirect3DTexture9 *Texture1, IDirect3DTexture9 *Texture2) {
  this->pDevice = pDevice;
  this->Texture1 = Texture1;
  this->Texture2 = Texture2;
}

void WindowAnimationType1::draw(Control *control, D3DXVECTOR2 offset) {
  D3DSURFACE_DESC info;
  ID3DXSprite *Sprite;
  D3DXMATRIX Mat;
  D3DXVECTOR2 ScalingCenter(0, 0);
  D3DXVECTOR2 Scaling;
  D3DXVECTOR2 drawingpos;
  int offset1 = 0, offset2 = 0;
  
  if (control->get_state().on_rect == 1) {
    offset1 = 2;
    if (control->get_state().is_pushed == 1)
      offset2 = 2;
  }
  
  drawingpos = offset + control->get_position();
  drawingpos.x = drawingpos.x - offset1 + offset2;
  drawingpos.y = drawingpos.y - offset1 + offset2;
  //--  
  Texture1->GetLevelDesc(0, &info);
  
  if (control->get_size().x != 0)
    Scaling.x = (control->get_size().x + offset1 * 2) / info.Width;
  else
    Scaling.x = 1;
  
  if (control->get_size().y != 0)
    Scaling.y = (control->get_size().y + offset1 * 2) / info.Height;
  else
    Scaling.y = 1;
  D3DXMatrixTransformation2D(&Mat, &ScalingCenter, 0, &Scaling, NULL, 0, &drawingpos);
  
  D3DXCreateSprite(pDevice, &Sprite);
  Sprite->Begin(0);
  Sprite->SetTransform(&Mat);
  Sprite->Draw(this->Texture1, NULL, NULL, NULL, 0xFFFFFFFF);
  Sprite->End();
  Sprite->Release();
  
  Texture2->GetLevelDesc(0, &info);
  
  if (control->get_size().x != 0)
    Scaling.x = (control->get_size().x + offset1 * 2) / info.Width;
  else
    Scaling.x = 1;
  
  if (control->get_size().y != 0)
    Scaling.y = (control->get_size().y + offset1 * 2) / info.Height;
  else
    Scaling.y = 1;
  D3DXMatrixTransformation2D(&Mat, &ScalingCenter, 0, &Scaling, NULL, 0, &drawingpos);
  
  D3DXCreateSprite(pDevice, &Sprite);
  Sprite->Begin(0);
  Sprite->SetTransform(&Mat);
  if (control->get_state().is_pushed == 1 && control->get_state().on_rect == 1)
    Sprite->Draw(this->Texture2, NULL, NULL, NULL, 0xFFFFFFFF);
  Sprite->End();
  Sprite->Release();
}

void WindowAnimationType2::init(IDirect3DDevice9 *pDevice, IDirect3DTexture9 *TexturePict, DWORD color1, DWORD color2, float AnimationTime) {
  this->pDevice = pDevice;
  this->TexturePict = TexturePict;
  this->color1 = color1;
  this->color2 = color2;
  if (AnimationTime != 0)
    this->AnimationTime = AnimationTime;
  else
    this->AnimationTime = 0.001;
  
  D3DSURFACE_DESC info;
  if (TexturePict != NULL)
    TexturePict->GetLevelDesc(0, &info);
  else
    info.Width = info.Height = 1;
  
  pDevice->CreateTexture(info.Width,info.Height,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&Texture,NULL);
}

void WindowAnimationType2::draw(Control *control, D3DXVECTOR2 offset) {
  D3DSURFACE_DESC info;
  ID3DXSprite *Sprite;
  IDirect3DSurface9 *Surface, *BackSurface;
  D3DXMATRIX Mat;
  
  Texture->GetSurfaceLevel(0,&Surface);
  pDevice->GetRenderTarget(0, &BackSurface);
  
//-----

  float CurrentTick, AnimProgress;
  CurrentTick = (float)clock() / CLOCKS_PER_SEC;
  if (control->get_state().on_rect) {
    CurrentAnimationTime += CurrentTick - LastTick;
    if (CurrentAnimationTime > AnimationTime) CurrentAnimationTime = AnimationTime;
  }
  else {
    CurrentAnimationTime -= CurrentTick - LastTick;
    if (CurrentAnimationTime < 0) CurrentAnimationTime = 0;
  }
  LastTick = CurrentTick;
  AnimProgress = CurrentAnimationTime / AnimationTime;
  
//-----
  Texture->GetLevelDesc(0, &info);
  
  int sizeoffset = 0;
  if (control->get_state().is_pushed && control->get_state().on_rect)
    sizeoffset = 2;
  
  D3DXVECTOR2 ScalingCenter(0, 0);
  D3DXVECTOR2 Scaling;
  if (control->get_size().x != 0)
    Scaling.x = (control->get_size().x - sizeoffset * 2) / info.Width;
  else
    Scaling.x = 1;
  
  if (control->get_size().y != 0)
    Scaling.y = (control->get_size().y - sizeoffset * 2) / info.Height;
  else
    Scaling.y = 1;
  D3DXVECTOR2 drawingpos = offset + control->get_position();
  drawingpos.x += sizeoffset;
  drawingpos.y += sizeoffset;
  D3DXMatrixTransformation2D(&Mat, &ScalingCenter, 0, &Scaling, NULL, 0, &drawingpos);
  
//-----  
  pDevice->SetRenderTarget(0, Surface);
  pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB((int)(COLOR_ALPHA(color2) * AnimProgress + COLOR_ALPHA(color1) * (1.0f - AnimProgress)), (int)(COLOR_RED(color2) * AnimProgress + COLOR_RED(color1) * (1.0f - AnimProgress)),
  (int)(COLOR_GREEN(color2) * AnimProgress + COLOR_GREEN(color1) * (1.0f - AnimProgress)), (int)(COLOR_BLUE(color2) * AnimProgress + COLOR_BLUE(color1) * (1.0f - AnimProgress))) , 1.0f, 0);
  pDevice->SetRenderTarget(0, BackSurface);
  
  D3DXCreateSprite(pDevice, &Sprite);
  Sprite->Begin(0);
  Sprite->SetTransform(&Mat);
  Sprite->Draw(Texture, NULL, NULL, NULL, 0xFFFFFFFF);
  if (TexturePict != NULL)
    Sprite->Draw(TexturePict, NULL, NULL, NULL, 0xFFFFFFFF);
  Sprite->End();
  Sprite->Release();
}