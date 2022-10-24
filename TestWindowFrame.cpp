#include <d3d9.h>
#include <d3dx9.h>
#include <windows.h>
#include "SpecialFramework.h"

Group group0, GroupExit;

WindowFrame Window;
Group group1, group2;
Button button0;
Button button1, button2, button3;
Button button21, button22, button23, button24;
Tab tab1;
Group Switches;

WindowFrame Window_2;
Group group1_2, group2_2;
Button button0_2;
Button button1_2, button2_2, button3_2;
Button button21_2, button22_2, button23_2, button24_2;

WindowManager manager1;
WindowManagerVertical1 manager2;
WindowManagerHorizontal2 manager3, manager31;
WindowManagerTab manager4;
WindowManagerMatrix manager5;
WindowManagerWindowed manager6;
WindowAnimation animation0;
WindowAnimationSimple animation1, animation2;
WindowAnimationType1 animation3;
WindowAnimationAnimPict1 animation10;
WindowAnimationType2 animation4, animation41, animation42, animation43,
animation44, animation45, animation46, animation410;
WindowAnimationWindowed animation5;

//-----------------------------
bool WndClass(HINSTANCE, WNDPROC, LPCTSTR);
HWND CreateMainWindow(HINSTANCE hInst, LPCSTR title, int width, int height);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
bool D3DInit(HWND hWnd, IDirect3D9 **pD3D, IDirect3DDevice9 **pDevice);
void D3DRelease(IDirect3D9 **pD3D, IDirect3DDevice9 **pDevice);
//------------
void Render();
bool InitResources();
bool InitFrame();
void CallbackF(ControlMessage msg);
//-----------------------------

char PushName[256] = "Hello world!";

IDirect3D9 *pD3D;
IDirect3DDevice9 *pDevice;
IDirect3DTexture9 *Texture1, *Texture2, *Texture3, *Texture4, *Texture5, *Texture6;

HINSTANCE GlobalhInst;
char chClassName[] = "WindClass";

HWND hWndMain;
RECT StartRect;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                   LPSTR lpszCmdLine, int nCmdShow) {
  GlobalhInst = hInstance;
  if (! WndClass(hInstance, WndProc, chClassName))
    return 0;
  
  hWndMain = CreateMainWindow(hInstance, "Framework Test", CW_USEDEFAULT, CW_USEDEFAULT);
  ShowWindow(hWndMain, SW_SHOWMAXIMIZED);
  GetClientRect(hWndMain, &StartRect);
  
  if (! D3DInit(hWndMain, &pD3D, &pDevice))
    return 0;
  
  if (! InitResources())
    return 0;
  
  if (! InitFrame())
    return 0;
  
  MSG msg;
  PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
  while(msg.message != WM_QUIT) {
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      DispatchMessage(&msg);
      TranslateMessage(&msg);
    }
    else {
      Render();
    }
  }
  return msg.wParam;
}

bool WndClass(HINSTANCE hInst, WNDPROC WindowProc, LPCTSTR szClassName) {
  WNDCLASS wc;
  wc.lpfnWndProc = (WNDPROC)WindowProc;
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.cbClsExtra = wc.cbWndExtra = 0;
  wc.hInstance = hInst;
  wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE;
  wc.lpszMenuName = (LPCTSTR)NULL;
  wc.lpszClassName = szClassName;
  return RegisterClass(&wc);
}

HWND CreateMainWindow(HINSTANCE hInst, LPCSTR title, int width, int height) {
  return CreateWindow(chClassName, title, WS_OVERLAPPEDWINDOW,
			   CW_USEDEFAULT, CW_USEDEFAULT,
			   width, height,
			   0, 0, hInst, NULL);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
D3DXVECTOR2 CurPos;
ControlMessage message;
  
switch(msg) {
  case WM_LBUTTONDOWN:
    CurPos.x = LOWORD(lParam);
    CurPos.y = HIWORD(lParam);
    Window.button_down(CurPos);
    Window_2.button_down(CurPos);
    break;

  case WM_LBUTTONUP:
    CurPos.x = LOWORD(lParam);
    CurPos.y = HIWORD(lParam);
    Window.button_up(CurPos);
    Window_2.button_up(CurPos);
    break;
    
  case WM_MOUSEMOVE:
    CurPos.x = LOWORD(lParam);
    CurPos.y = HIWORD(lParam);
    if (! Window.mouse_move(CurPos) && ! Window_2.mouse_move(CurPos)) {
      strcpy(PushName, "Framework Test");
      SetWindowText(hWndMain, PushName);
    }
    break;
  
  case WM_DESTROY:
    D3DRelease(&pD3D, &pDevice);
    PostQuitMessage(0);
    break;
    
  default:
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
return 0;
}

bool D3DInit(HWND hWnd, IDirect3D9 **pD3D, IDirect3DDevice9 **pDevice) {
  if (NULL == (*pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
    return false;
  
  D3DPRESENT_PARAMETERS d3dpp;
  ZeroMemory(&d3dpp, sizeof(d3dpp));
  d3dpp.Windowed = true;
  d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
  d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
  d3dpp.EnableAutoDepthStencil = true;
  d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
  HRESULT hr;
  hr = (*pD3D)->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                          D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                          &d3dpp, &(*pDevice));
  if( FAILED(hr) || !(*pDevice))
    return false;
  
  return true;
}

void D3DRelease(IDirect3D9 **pD3D, IDirect3DDevice9 **pDevice) {
  if (*pDevice)
    (*pDevice)->Release();
  if (*pD3D)
    (*pD3D)->Release();
}

//------------------------------------------------------------------------------
void Render() {
  pDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(128, 128, 128), 1.0f, 0);
  
  if (SUCCEEDED(pDevice->BeginScene())) {
    Window.draw();
    Window_2.draw();
    pDevice->EndScene();
  }
  
  pDevice->Present(NULL, NULL, NULL, NULL);
}

bool InitResources() {
  HRESULT hr;
  
  hr = D3DXCreateTextureFromFile(pDevice, "textures\\button.jpg", &Texture1);
  if (FAILED(hr)) {
    return false;
  }
  
  hr = D3DXCreateTextureFromFile(pDevice, "textures\\effect.png", &Texture2);
  if (FAILED(hr)) {
    return false;
  }
  
  hr = D3DXCreateTextureFromFile(pDevice, "textures\\brick.jpg", &Texture3);
  if (FAILED(hr)) {
    return false;
  }
  
  hr = D3DXCreateTextureFromFile(pDevice, "textures\\exit.png", &Texture4);
  if (FAILED(hr)) {
    return false;
  }
  
  hr = D3DXCreateTextureFromFile(pDevice, "textures\\wall2.png", &Texture5);
  if (FAILED(hr)) {
    return false;
  }
  
  hr = D3DXCreateTextureFromFile(pDevice, "textures\\flash.png", &Texture6);
  if (FAILED(hr)) {
    return false;
  }
  
  pDevice->SetRenderState(D3DRS_LIGHTING, false);
  pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
  pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
  pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
  
  return true;
}

bool InitFrame() {
  RECT board;
  board.left = 10;
  board.right = 10;
  board.bottom = 10;
  board.top = 40;
  manager2.init(3, 0.5f);
  manager3.init(2, 0.1f, ALIGN_LEFT);
  manager31.init(2, 0.1f, ALIGN_RIGHT);
  manager4.init(50, 0.2f);
  manager5.init(4, 0.5f);
  manager6.init(board);
  animation1.init(pDevice, Texture3);
  animation2.init(pDevice, Texture5);
  animation3.init(pDevice, Texture1, Texture2);
  animation4.init(pDevice, Texture4, D3DCOLOR_ARGB(255, 220, 220, 220), D3DCOLOR_ARGB(255, 230, 0, 0), 0.25f);
  animation41.init(pDevice, Texture4, D3DCOLOR_ARGB(64, 0, 254, 0), D3DCOLOR_ARGB(255, 0, 254, 0), 0.5f);
  animation42.init(pDevice, Texture4, D3DCOLOR_ARGB(64, 0, 0, 255), D3DCOLOR_ARGB(255, 0, 0, 255), 0.5f);
  animation43.init(pDevice, Texture4, D3DCOLOR_ARGB(64, 255, 254, 0), D3DCOLOR_ARGB(255, 255, 254, 0), 0.5f);
  animation44.init(pDevice, Texture4, D3DCOLOR_ARGB(64, 0, 254, 255), D3DCOLOR_ARGB(255, 0, 254, 255), 0.5f);
  animation45.init(pDevice, Texture4, D3DCOLOR_ARGB(64, 255, 0, 255), D3DCOLOR_ARGB(255, 255, 0, 255), 0.5f);
  animation46.init(pDevice, Texture4, D3DCOLOR_ARGB(64, 0, 0, 0), D3DCOLOR_ARGB(255, 0, 0, 0), 0.5f);
  animation410.init(pDevice, Texture4, D3DCOLOR_ARGB(64, 0, 0, 0), D3DCOLOR_ARGB(255, 0, 0, 0), 0.25f);
  animation5.init(pDevice, NULL, D3DCOLOR_ARGB(255, 255, 255, 255), D3DCOLOR_ARGB(128, 255, 0, 0), board, 0);
  animation10.init(pDevice, Texture6, 4, 16, 1.0f);
  group0.init(0, D3DXVECTOR2(0, 0), D3DXVECTOR2(0, 0), &animation5, &manager6);
  GroupExit.init(0, D3DXVECTOR2(0, 0), D3DXVECTOR2(0, 0), NULL, &manager31);
  group1.init(0, D3DXVECTOR2(0, 60), D3DXVECTOR2(430, 230), &animation1, &manager1);
  button1.init(1, D3DXVECTOR2(0, 0), D3DXVECTOR2(210, 110), &animation4);
  button1.set_callbackf(&CallbackF);
  //group1.add_control(&button1);
  button2.init(2, D3DXVECTOR2(220, 0), D3DXVECTOR2(210, 110), &animation10);
  button2.set_callbackf(&CallbackF);
  group1.add_control(&button2);
  button3.init(3, D3DXVECTOR2(0, 120), D3DXVECTOR2(210, 110), &animation10);
  button3.set_callbackf(&CallbackF);
  group1.add_control(&button3);
  //--
  group2.init(0, D3DXVECTOR2(220, 120), D3DXVECTOR2(210, 110), &animation0, &manager1);
  button21.init(21, D3DXVECTOR2(0, 0), D3DXVECTOR2(100, 50), &animation10);
  button21.set_callbackf(&CallbackF);
  group2.add_control(&button21);
  button22.init(22, D3DXVECTOR2(110, 0), D3DXVECTOR2(100, 50), &animation10);
  button22.set_callbackf(&CallbackF);
  group2.add_control(&button22);
  button23.init(23, D3DXVECTOR2(0, 60), D3DXVECTOR2(100, 50), &animation10);
  button23.set_callbackf(&CallbackF);
  group2.add_control(&button23);
  button24.init(24, D3DXVECTOR2(110, 60), D3DXVECTOR2(100, 50), &animation10);
  button24.set_callbackf(&CallbackF);
  group2.add_control(&button24);
  button0.init(0, D3DXVECTOR2(0, 0), D3DXVECTOR2(100, 50), &animation410);
  //--
  group1.add_control(&group2);
  Switches.init(0, D3DXVECTOR2(0, 0), D3DXVECTOR2(430, 50), &animation0, &manager3);
  tab1.init(&Switches, 24, D3DXVECTOR2(0, 0), D3DXVECTOR2(430, 290), NULL, &manager4);
  tab1.set_controls(&button0, &group1);
  group1.show_control(false);
  
  GroupExit.add_control(&button1);
  group0.add_control(&GroupExit);
  group0.add_control(&tab1);
  //Window.init(&group1, D3DXVECTOR2(50, 50), D3DXVECTOR2(430, 230));
  Window.init(&group0, D3DXVECTOR2(50, 50), D3DXVECTOR2(450, 340));//
  
//================================================================================

  group1_2.init(0, D3DXVECTOR2(0, 60), D3DXVECTOR2(430, 230), &animation2, &manager5);
  button1_2.init(1, D3DXVECTOR2(0, 0), D3DXVECTOR2(210, 110), &animation3);
  button1_2.set_callbackf(&CallbackF);
  group1_2.add_control(&button1_2);
  button2_2.init(2, D3DXVECTOR2(220, 0), D3DXVECTOR2(210, 110), &animation3);
  button2_2.set_callbackf(&CallbackF);
  group1_2.add_control(&button2_2);
  button3_2.init(3, D3DXVECTOR2(0, 120), D3DXVECTOR2(210, 110), &animation3);
  button3_2.set_callbackf(&CallbackF);
  group1_2.add_control(&button3_2);
  //--
  group2_2.init(0, D3DXVECTOR2(220, 120), D3DXVECTOR2(210, 110), &animation0, &manager1);
  button21_2.init(21, D3DXVECTOR2(0, 0), D3DXVECTOR2(100, 50), &animation3);
  button21_2.set_callbackf(&CallbackF);
  group1_2.add_control(&button21_2);
  button22_2.init(22, D3DXVECTOR2(110, 0), D3DXVECTOR2(100, 50), &animation3);
  button22_2.set_callbackf(&CallbackF);
  group1_2.add_control(&button22_2);
  button23_2.init(23, D3DXVECTOR2(0, 60), D3DXVECTOR2(100, 50), &animation3);
  button23_2.set_callbackf(&CallbackF);
  group1_2.add_control(&button23_2);
  button24_2.init(24, D3DXVECTOR2(110, 60), D3DXVECTOR2(100, 50), &animation3);
  button24_2.set_callbackf(&CallbackF);
  group1_2.add_control(&button24_2);
  button0_2.init(0, D3DXVECTOR2(110, 0), D3DXVECTOR2(100, 50), &animation3);
  //--
  group1_2.add_control(&group2_2);
  tab1.set_controls(&button0_2, &group1_2);
  
  //Window_2.init(&group1_2, D3DXVECTOR2(500, 50), D3DXVECTOR2(430, 230));
  Window.set_size(Window.get_size());
  return true;
}

void CallbackF(ControlMessage msg) {
  strcpy(PushName, "Framework Test -");
  switch (msg.ThisObject->get_id()) {
    case 1:
      strcat(PushName, " button1");
      if (msg.message == MSG_BUTTONUP)
        Window.show_control(false);
      break;
          
    case 2:
      strcat(PushName, " button2");
      break;
          
    case 3:
      strcat(PushName, " button3");
      break;
          
    case 21:
      strcat(PushName, " button21");
      break;
          
    case 22:
      strcat(PushName, " button22");
      break;
          
    case 23:
      strcat(PushName, " button23");
      break;
          
    case 24:
      strcat(PushName, " button24");
      break;
  }
  
  switch (msg.message) {
    case MSG_BUTTONDOWN:
      strcat(PushName, " down");
      break;
    
    case MSG_BUTTONUP:
      strcat(PushName, " up");
      break;
  }
  
  SetWindowText(hWndMain, PushName);
}