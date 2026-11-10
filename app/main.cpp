#include <raylib.h>

#include <iostream>
#include <random>
#include <vector>

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
  float screenWidth(800.0);
  float screenHeight(400.0);

  InitWindow(screenWidth, screenHeight, "particles are here");
  SetTargetFPS(60);

  std::random_device rd;
  std::mt19937 gen(rd());

  std::uniform_real_distribution<float> posX(0.0, screenWidth);
  std::uniform_real_distribution<float> posY(0.0, screenHeight);
  std::uniform_real_distribution<float> velX(-2.0, 2.0);
  std::uniform_real_distribution<float> velY(-2.0, 2.0);
  std::uniform_real_distribution<float> radius(2.0, 5.0);

  std::vector<Particle> particles;
  const int numParticles{1000};
  particles.reserve(numParticles);

  for (int i{0}; i < numParticles; ++i) {
    Vector2 pos{posX(gen), posY(gen)};
    Vector2 vel{velX(gen), velY(gen)};
    float rad{radius(gen)};
    Color c{RED};
    particles.emplace_back(pos, vel, rad, c);
  }

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    for (auto &particle : particles) {
      particle.move();
      particle.draw();
    }

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
