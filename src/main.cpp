#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <objidl.h>
#include <shellapi.h>
#include <gdiplus.h>
#include <windowsx.h>

#include "GdiplusRenderer.h"

class GdiplusWindow {
public:
  GdiplusWindow(int width, int height, const char *title, HINSTANCE instance, const char *argument, INT cmd_show) :
    renderer{width, height} {
    // Initialize GDI+.
    Gdiplus::GdiplusStartupInput input;
    Gdiplus::GdiplusStartup(&gdiplus_token, &input, NULL);

    LPCSTR class_name = TEXT("SVGWindow");
    WNDCLASS wnd_class {
      CS_HREDRAW | CS_VREDRAW,
      GdiplusWindow::callback,
      0, 0,
      instance,
      LoadIcon(NULL, IDI_APPLICATION),
      LoadCursor(NULL, IDC_ARROW),
      (HBRUSH)GetStockObject(WHITE_BRUSH),
      NULL,
      class_name,
    };

    RegisterClass(&wnd_class);

    this->window = CreateWindow(
      class_name,
      TEXT(title),
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      width, height,
      NULL, NULL, instance, NULL
    );

    SetWindowLongPtr(this->window, GWLP_USERDATA, (LONG_PTR)&this->renderer);

    ShowWindow(this->window, cmd_show);
    UpdateWindow(this->window);

    if (argument && argument[0] && !this->renderer.load_file(argument)) {
      char msg[64];
      snprintf(
        msg, sizeof(msg), "Failed to open file `%s`", argument
      );
      MessageBox(this->window, msg, "Error", MB_ICONWARNING | MB_OK);
    }

    InvalidateRect(this->window, NULL, TRUE);
  }

  GdiplusWindow(const GdiplusWindow&) = delete;
  GdiplusWindow& operator=(const GdiplusWindow&) = delete;
  GdiplusWindow(GdiplusWindow&&) = delete;
  GdiplusWindow& operator=(GdiplusWindow&&) = delete;

  void run() {
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  ~GdiplusWindow() {
    this->renderer.clear();
    Gdiplus::GdiplusShutdown(this->gdiplus_token);
  }
private:
  HWND window;
  ULONG_PTR gdiplus_token;
  GdiplusRenderer renderer;

  static LRESULT CALLBACK callback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    GdiplusRenderer *renderer = (GdiplusRenderer*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    switch(message) {
      case WM_CREATE: {
        DragAcceptFiles(hWnd, TRUE);
      } break;
      case WM_LBUTTONDOWN: {
        renderer->drag_start(Point {
          (double)GET_X_LPARAM(lParam),
          (double)GET_Y_LPARAM(lParam),
        });
      } break;
      case WM_LBUTTONUP: {
        renderer->drag_end();
      } break;
      case WM_MOUSEMOVE: {
        if (renderer->drag_move(Point {
          (double)GET_X_LPARAM(lParam),
          (double)GET_Y_LPARAM(lParam),
        })) {
          InvalidateRect(hWnd, NULL, TRUE);
        }
      } break;
      case WM_MOUSEWHEEL: {
        double delta = (double)GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
        renderer->zoom(delta);

        InvalidateRect(hWnd, NULL, TRUE);
      } break;
      case WM_SIZE: {
        renderer->resize(LOWORD(lParam), HIWORD(lParam));
        InvalidateRect(hWnd, NULL, TRUE);
      } break;
      case WM_DROPFILES: {
        HDROP hDrop = (HDROP)wParam;
        char filePath[MAX_PATH];
        DragQueryFile(hDrop, 0, filePath, MAX_PATH);
        renderer->load_file(filePath);
        DragFinish(hDrop);
        InvalidateRect(hWnd, NULL, TRUE);
      } break;
      case WM_ERASEBKGND:
        return (LRESULT)1;
      case WM_PAINT: {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);

        RECT rc;
        GetClientRect(hWnd, &rc);

        HDC hdc = CreateCompatibleDC(ps.hdc);
        HBITMAP bitmap = CreateCompatibleBitmap(
          ps.hdc,
          rc.right - rc.left,
          rc.bottom - rc.top
        );
        HBITMAP old_bitmap = (HBITMAP)SelectObject(hdc, bitmap);

        HBRUSH background = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
        FillRect(hdc, &rc, background);
        DeleteObject(background);

        Gdiplus::Graphics graphics {hdc};
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
        renderer->render(&graphics);

        BitBlt(
          ps.hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
          hdc, 0, 0, SRCCOPY
        );

        SelectObject(hdc, old_bitmap);
        DeleteObject(bitmap);
        DeleteDC(hdc);

        EndPaint(hWnd, &ps);
      } break;
      case WM_DESTROY: {
        SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
        PostQuitMessage(0);
      } break;
      default: {
        return DefWindowProc(hWnd, message, wParam, lParam);
      }
    }
    return 0;
  }
};

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR argument, INT iCmdShow) {
  GdiplusWindow window(960, 720, "SVG viewer app", hInstance, argument, iCmdShow);
  window.run();
  return 0;
}
