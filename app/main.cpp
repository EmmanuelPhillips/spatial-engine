#include <raylib.h>

#include <iostream>

class Particle {
private:
  Vector2 position_;
  Vector2 velocity_;
  float radius_;
  Color colour_;

public:
  Particle(Vector2 p, Vector2 v, float r, Color c)
      : position_{p}, velocity_{v}, radius_{r}, colour_{c} {}

  void draw() const { DrawCircleV(position_, radius_, colour_); }

  void move() {
    position_.x += velocity_.x;
    position_.y += velocity_.y;
  }
};

int main() {
  Particle p1{{100.0, 100.0}, {1.0, -1.0}, 3.0, RED};

  InitWindow(800, 400, "particles are here");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    p1.draw();
    p1.move();

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
