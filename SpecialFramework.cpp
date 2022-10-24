#include "SpecialFramework.h"

void WindowManagerSimple::set_elements(const std::vector<Control*> &controls, D3DXVECTOR2 pos, D3DXVECTOR2 size) {
  for (int i = 0; i < controls.size(); i++) {
    controls[i]->set_position(D3DXVECTOR2(0, 0));
    controls[i]->set_size(size);
  }
}

void WindowManagerVertical1::set_elements(const std::vector<Control*> &controls, D3DXVECTOR2 pos, D3DXVECTOR2 size) {
  float NumElems = controls.size() + (controls.size() - 1) * this->space + 2;
  int ElemHeight = size.y / NumElems;
  int ElemWidth = ElemHeight * WHratio;
  int offset = ElemHeight;
  
  for (int i = 0; i < controls.size(); i++) {
    controls[i]->set_position(D3DXVECTOR2(size.x / 2 - ElemWidth / 2, offset));
    controls[i]->set_size(D3DXVECTOR2(ElemWidth, ElemHeight));
    offset += ElemHeight * (1.0 + this->space);
  }
}

void WindowManagerHorizontal1::set_elements(const std::vector<Control*> &controls, D3DXVECTOR2 pos, D3DXVECTOR2 size) {
  float NumElems = controls.size() + (controls.size() - 1) * this->space;
  int ElemWidth = size.x / NumElems;
  int ElemHeight = ElemWidth * (1 / WHratio);
  int offset = 0;
  
  for (int i = 0; i < controls.size(); i++) {
    controls[i]->set_position(D3DXVECTOR2(offset, size.y / 2 - ElemHeight / 2));
    controls[i]->set_size(D3DXVECTOR2(ElemWidth, ElemHeight));
    offset += ElemWidth * (1.0 + this->space);
  }
}

void WindowManagerHorizontal2::set_elements(const std::vector<Control*> &controls, D3DXVECTOR2 pos, D3DXVECTOR2 size) {
  int ElemHeight = size.y;
  int ElemWidth = ElemHeight * WHratio;
  int offset = 0, NumElem = controls.size();
  
  switch (align) {
    case ALIGN_LEFT:
      for (int i = 0; i < controls.size(); i++) {
        controls[i]->set_position(D3DXVECTOR2(offset, 0));
        controls[i]->set_size(D3DXVECTOR2(ElemWidth, ElemHeight));
        offset += ElemWidth * (1.0 + this->space);
      }
      break;
      
    case ALIGN_CENTER:
      offset = (size.x - ElemWidth * (NumElem + this->space * (NumElem - 1))) / 2;
      for (int i = 0; i < controls.size(); i++) {
        controls[i]->set_position(D3DXVECTOR2(offset, 0));
        controls[i]->set_size(D3DXVECTOR2(ElemWidth, ElemHeight));
        offset += ElemWidth * (1.0 + this->space);
      }
      break;
      
    case ALIGN_RIGHT:
      offset = ElemWidth;
      for (int i = controls.size() - 1; i >= 0; i--) {
        controls[i]->set_position(D3DXVECTOR2(size.x - offset, 0));
        controls[i]->set_size(D3DXVECTOR2(ElemWidth, ElemHeight));
        offset += ElemWidth * (1.0 + this->space);
      }
      break;
  }
}

void WindowManagerMatrix::set_elements(const std::vector<Control*> &controls, D3DXVECTOR2 pos, D3DXVECTOR2 size) {
  int ElemSize = size.x / ((float)(this->NumHeight) * (1.0f + this->space));
  for (int i = 0; i < controls.size(); i++) {
    controls[i]->set_position(D3DXVECTOR2(ElemSize * (1 + this->space) * (i % NumHeight) + ElemSize * this->space * 0.5f, ElemSize * (1 + this->space) * (i / NumHeight) + ElemSize * this->space * 0.5f));
    controls[i]->set_size(D3DXVECTOR2(ElemSize, ElemSize));
  }
}

void WindowManagerTab::set_elements(const std::vector<Control*> &controls, D3DXVECTOR2 pos, D3DXVECTOR2 size) {
  controls[0]->set_position(D3DXVECTOR2(0, 0));
  controls[0]->set_size(D3DXVECTOR2(size.x, this->SwitchesHeight));
  
  for (int i = 1; i < controls.size(); i++) {
    controls[i]->set_position(D3DXVECTOR2(0, this->SwitchesHeight * (1 + space)));
    controls[i]->set_size(D3DXVECTOR2(size.x, size.y - this->SwitchesHeight * (1 + space)));
  }
}

void WindowManagerWindowed::set_elements(const std::vector<Control*> &controls, D3DXVECTOR2 pos, D3DXVECTOR2 size) {
  if (controls[0] != NULL) {
    controls[0]->set_position(D3DXVECTOR2(board.left, 0));
    controls[0]->set_size(D3DXVECTOR2(size.x - board.left - board.right, board.top));
  }
  
  if (controls[1] != NULL) {
    controls[1]->set_position(D3DXVECTOR2(board.left, board.top));
    controls[1]->set_size(D3DXVECTOR2(size.x - board.left - board.right, size.y - board.top - board.bottom));
  }
}

void WindowAnimationSimple::init(IDirect3DDevice9 *pDevice, IDirect3DTexture9 *Texture) {
  this->pDevice = pDevice;
  this->Texture = Texture;
  PaintObject.init(pDevice);
}

void WindowAnimationSimple::draw(Control *control, D3DXVECTOR2 offset, D3DXVECTOR2 boundpos1, D3DXVECTOR2 boundpos2) {
  D3DSURFACE_DESC info;
  ID3DXSprite *Sprite;
  D3DXCreateSprite(pDevice, &Sprite);
  D3DXMATRIX Mat;
//----
  PaintObject.BeginPaint();
//----
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
  
  Sprite->Begin(0);
  Sprite->SetTransform(&Mat);
  Sprite->Draw(Texture, NULL, NULL, NULL, 0xFFFFFFFF);
  Sprite->End();
  Sprite->Release();
//----
  PaintObject.EndPaint();
  PaintObject.print(boundpos1, boundpos2, offset);
}

void WindowAnimationFill::init(IDirect3DDevice9 *pDevice, IDirect3DTexture9 *Texture) {
  this->pDevice = pDevice;
  this->Texture = Texture;
  PaintObject.init(pDevice);
}

void WindowAnimationFill::draw(Control *control, D3DXVECTOR2 offset, D3DXVECTOR2 boundpos1, D3DXVECTOR2 boundpos2) {
  D3DSURFACE_DESC info;
  ID3DXSprite *Sprite;
  D3DXCreateSprite(pDevice, &Sprite);
  D3DXMATRIX Mat;
//----
  PaintObject.BeginPaint();
//----
  Texture->GetLevelDesc(0, &info);
  
  D3DXVECTOR2 ScalingCenter(0, 0);
  D3DXVECTOR2 Scaling;
  Scaling.x = 1;
  Scaling.y = 1;
  int NumElemWidth = (boundpos2.x - boundpos1.x) / info.Width + 1;
  int NumElemHeight = (boundpos2.y - boundpos1.y) / info.Height + 1;
  D3DXVECTOR2 TabOffset = offset;
  D3DXVECTOR2 drawingpos;
  
  Sprite->Begin(0);
  for (int i = 0; i < NumElemHeight; i++) {
    for (int j = 0; j < NumElemWidth; j++) {
      drawingpos = TabOffset + control->get_position();
      D3DXMatrixTransformation2D(&Mat, &ScalingCenter, 0, &Scaling, NULL, 0, &drawingpos);
      Sprite->SetTransform(&Mat);
      Sprite->Draw(Texture, NULL, NULL, NULL, 0xFFFFFFFF);
      TabOffset.x += info.Width;
    }
    TabOffset.x = 0;
    TabOffset.y += info.Height;
  }
  Sprite->End();
  Sprite->Release();
//----
  PaintObject.EndPaint();
  PaintObject.print(boundpos1, boundpos2, offset);
}

void WindowAnimationWindowed::init(IDirect3DDevice9 *pDevice, IDirect3DTexture9 *Texture, DWORD color1, DWORD color2, RECT board, bool using_texture) {
  this->pDevice = pDevice;
  this->Texture = Texture;
  this->color1 = color1;
  this->color2 = color2;
  this->board = board;
  this->using_texture = using_texture;
  PaintObject.init(pDevice);
}

void WindowAnimationWindowed::draw(Control *control, D3DXVECTOR2 offset, D3DXVECTOR2 boundpos1, D3DXVECTOR2 boundpos2) {
  D3DSURFACE_DESC info;
  ID3DXSprite *Sprite;
  D3DXMATRIX Mat;
//----
  PaintObject.BeginPaint();
//----
/*
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
  
//-----  */
  D3DRECT rect;
  rect.x1 = control->get_position().x + offset.x; rect.y1 = control->get_position().y + offset.y;
  rect.x2 = control->get_position().x + control->get_size().x + offset.x;
  rect.y2 = control->get_position().y + control->get_size().y + offset.y;
  pDevice->Clear(1, &rect, D3DCLEAR_TARGET, this->color1, 1.0f, 0);
  rect.x1 = rect.x1 + board.left; rect.y1 = rect.y1 + board.top;
  rect.x2 = rect.x2 - board.right; rect.y2 = rect.y2 - board.bottom;
  pDevice->Clear(1, &rect, D3DCLEAR_TARGET, this->color2, 1.0f, 0);
  
/*  D3DXCreateSprite(pDevice, &Sprite);
  Sprite->Begin(0);
  Sprite->SetTransform(&Mat);
  if (TexturePict != NULL)
    Sprite->Draw(TexturePict, NULL, NULL, NULL, 0xFFFFFFFF);
  Sprite->End();
  Sprite->Release();*/
//----
  PaintObject.EndPaint();
  PaintObject.print(boundpos1, boundpos2, offset);
}

void WindowAnimationType1::init(IDirect3DDevice9 *pDevice, IDirect3DTexture9 *Texture1, IDirect3DTexture9 *Texture2) {
  this->pDevice = pDevice;
  this->Texture1 = Texture1;
  this->Texture2 = Texture2;
  PaintObject.init(pDevice);
}

void WindowAnimationType1::draw(Control *control, D3DXVECTOR2 offset, D3DXVECTOR2 boundpos1, D3DXVECTOR2 boundpos2) {
  D3DSURFACE_DESC info;
  ID3DXSprite *Sprite;
  D3DXMATRIX Mat;
  D3DXVECTOR2 ScalingCenter(0, 0);
  D3DXVECTOR2 Scaling;
  D3DXVECTOR2 drawingpos;
//----
  PaintObject.BeginPaint();
//----
  int offset1 = 0, offset2 = 0;
  
  if (control->get_state().on_rect == 1 && control->get_state().is_pushed == 0) {
    offset1 = 2;
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
//----
  PaintObject.EndPaint();
  PaintObject.print(boundpos1, boundpos2, offset);
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
  PaintObject.init(pDevice);
}

void WindowAnimationType2::draw(Control *control, D3DXVECTOR2 offset, D3DXVECTOR2 boundpos1, D3DXVECTOR2 boundpos2) {
  D3DSURFACE_DESC info;
  ID3DXSprite *Sprite;
  D3DXMATRIX Mat;
//----
  PaintObject.BeginPaint();
//----

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
  if (TexturePict != NULL)
    TexturePict->GetLevelDesc(0, &info);
  
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
  D3DRECT rect;
  rect.x1 = control->get_position().x + offset.x + sizeoffset; rect.y1 = control->get_position().y + offset.y + sizeoffset;
  rect.x2 = control->get_position().x + control->get_size().x + offset.x - sizeoffset;
  rect.y2 = control->get_position().y + control->get_size().y + offset.y - sizeoffset;
  pDevice->Clear(1, &rect, D3DCLEAR_TARGET, D3DCOLOR_ARGB((int)(COLOR_ALPHA(color2) * AnimProgress + COLOR_ALPHA(color1) * (1.0f - AnimProgress)), (int)(COLOR_RED(color2) * AnimProgress + COLOR_RED(color1) * (1.0f - AnimProgress)),
  (int)(COLOR_GREEN(color2) * AnimProgress + COLOR_GREEN(color1) * (1.0f - AnimProgress)), (int)(COLOR_BLUE(color2) * AnimProgress + COLOR_BLUE(color1) * (1.0f - AnimProgress))) , 1.0f, 0);
  
  D3DXCreateSprite(pDevice, &Sprite);
  Sprite->Begin(0);
  Sprite->SetTransform(&Mat);
  if (TexturePict != NULL)
    Sprite->Draw(TexturePict, NULL, NULL, NULL, 0xFFFFFFFF);
  Sprite->End();
  Sprite->Release();
//----
  PaintObject.EndPaint();
  PaintObject.print(boundpos1, boundpos2, offset);
}

void WindowAnimationAnimPict1::init(IDirect3DDevice9 *pDevice, IDirect3DTexture9 *TexturePict,
  int NumPictInLine, int NumPict, float AnimationTime) {
  this->pDevice = pDevice;
  this->TexturePict = TexturePict;
  this->NumPictInLine = NumPictInLine;
  this->NumPict = NumPict;
  if (AnimationTime != 0)
    this->AnimationTime = AnimationTime;
  else
    this->AnimationTime = 0.001;
  PaintObject.init(pDevice);
}

void WindowAnimationAnimPict1::draw(Control *control, D3DXVECTOR2 offset, D3DXVECTOR2 boundpos1, D3DXVECTOR2 boundpos2) {
  D3DSURFACE_DESC info;
  ID3DXSprite *Sprite;
  D3DXMATRIX Mat;
//----
  PaintObject.BeginPaint();
//----

  float CurrentTick, AnimProgress;
  int PictureNumber;
  CurrentTick = (float)clock() / CLOCKS_PER_SEC;

  CurrentAnimationTime += CurrentTick - LastTick;
  if (CurrentAnimationTime >= AnimationTime) CurrentAnimationTime = CurrentAnimationTime - AnimationTime;

  LastTick = CurrentTick;
  AnimProgress = CurrentAnimationTime / AnimationTime;
  PictureNumber = (int)(NumPict * AnimProgress);
//-----
  if (TexturePict != NULL)
    TexturePict->GetLevelDesc(0, &info);
  
  D3DXVECTOR2 PictSize;
  int NumPictInColumn = NumPict / NumPictInLine;
  if (NumPict % NumPictInLine != 0)
    NumPictInColumn += 1;
  PictSize.x = info.Width / NumPictInLine;
  PictSize.y = info.Height / NumPictInColumn;
  
  D3DXVECTOR2 ScalingCenter(0, 0);
  D3DXVECTOR2 Scaling;
  if (control->get_size().x != 0)
    Scaling.x = (control->get_size().x) / PictSize.x;
  else
    Scaling.x = 1;
  
  if (control->get_size().y != 0)
    Scaling.y = (control->get_size().y) / PictSize.y;
  else
    Scaling.y = 1;
  
  D3DXVECTOR2 drawingpos = offset + control->get_position();
  D3DXMatrixTransformation2D(&Mat, &ScalingCenter, 0, &Scaling, NULL, 0, &drawingpos);
  
//-----  
  RECT RectSource;
  RectSource.left = (PictureNumber % NumPictInLine) * PictSize.x;
  RectSource.top = (PictureNumber / NumPictInLine) * PictSize.y;
  RectSource.right = RectSource.left + PictSize.x;
  RectSource.bottom = RectSource.top + PictSize.y;
  
  D3DXCreateSprite(pDevice, &Sprite);
  Sprite->Begin(0);
  Sprite->SetTransform(&Mat);
  if (TexturePict != NULL)
    Sprite->Draw(TexturePict, &RectSource, NULL, NULL, 0xFFFFFFFF);
  Sprite->End();
  Sprite->Release();
//----
  PaintObject.EndPaint();
  PaintObject.print(boundpos1, boundpos2, offset);
}

