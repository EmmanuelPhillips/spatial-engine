#include <string>

#include "raylib.h"

int main() {
  InitWindow(1024, 576, "This is a Raylib Test");
  SetTargetFPS(60);

  std::string msg{"centred text"};
  int fontSize{24};
  int textW{MeasureText(msg.c_str(), fontSize)};

  while (!WindowShouldClose()) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      break;
    }

    BeginDrawing();
    ClearBackground(BLACK);

    if (IsCursorOnScreen()) {
      DrawText(msg.c_str(), GetScreenWidth() / 2 - textW / 2,
               GetScreenHeight() / 2 - fontSize / 2, fontSize, WHITE);
    }

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
