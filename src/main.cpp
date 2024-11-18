#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <objidl.h>
#include <shellapi.h>

#include <fstream>
#include <sstream>

#include "parser.h"

class SVGRenderer {
public:
  SVGRenderer() : shapes{nullptr} {}
  
  void load_file(const char *filename) {
    std::ifstream fin(filename);
    std::ostringstream ss;
    ss << fin.rdbuf();
    this->svg_file = ss.str();
    this->shapes = parse_xml(this->svg_file);
  }

  void render(Gdiplus::Graphics *graphics) {
    for (BaseShape *shape = this->shapes.get(); shape; shape = shape->next.get()) {
      shape->render(graphics);
    }
  }
private:
  std::unique_ptr<BaseShape> shapes;
  std::string svg_file;
};

class SVGWindow {
public:
  SVGWindow(int width, int height, const char *title, HINSTANCE instance, INT cmd_show) {
    // Initialize GDI+.
    Gdiplus::GdiplusStartupInput input;
    Gdiplus::GdiplusStartup(&gdiplus_token, &input, NULL);

    LPCSTR class_name = TEXT("SVGWindow");
    WNDCLASS wnd_class = {
      CS_HREDRAW | CS_VREDRAW,
      SVGWindow::callback,
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
  }

  void run() {
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  ~SVGWindow() {
    CloseWindow(this->window);
    Gdiplus::GdiplusShutdown(this->gdiplus_token);
  }
private:
  HWND window;
  ULONG_PTR gdiplus_token;
  SVGRenderer renderer;

  static LRESULT CALLBACK callback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch(message) {
      case WM_CREATE: {
        DragAcceptFiles(hWnd, TRUE);
      } break;
     case WM_DROPFILES: {
        HDROP hDrop = (HDROP)wParam;
        char filePath[MAX_PATH];
        DragQueryFile(hDrop, 0, filePath, MAX_PATH); // Get number of files dropped
        SVGRenderer *renderer = (SVGRenderer*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
        renderer->load_file(filePath);
        DragFinish(hDrop);

        InvalidateRect(hWnd, NULL, TRUE);
      } break;
      case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        Gdiplus::Graphics graphics = {hdc};
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        SVGRenderer *renderer = (SVGRenderer*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
        renderer->render(&graphics);
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

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow) {
  SVGWindow window(960, 720, "SVG viewer app", hInstance, iCmdShow);
  window.run();
  return 0;
}
