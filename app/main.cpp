#include <raylib.h>

#include <string>

int main() {
  InitWindow(1280, 720, "my test window");
  SetTargetFPS(60);

  const std::string pressed_str{"You press the left mouse button"};
  const std::string unpressed_str{"You haven't pressed the left mouse button"};

  bool is_active{false};

  while (!WindowShouldClose()) {

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      is_active = !is_active;
    }

    if (is_active) {
      ClearBackground(RED);
      DrawText(pressed_str.c_str(), 20, 20, 20, WHITE);
    } else {
      ClearBackground(WHITE);
      DrawText(unpressed_str.c_str(), 20, 20, 20, RED);
    }

    EndDrawing();
  }
  return 0;
}
