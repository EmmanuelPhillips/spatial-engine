#include <cmath>
#include <raylib.h>
#include <raymath.h>

#include <iostream>
#include <random>
#include <vector>

class Particle {
private:
  Vector2 position_;
  Vector2 velocity_;
  float radius_;
  Color colour_;

  void check_boundaries(float screen_width, float screen_height) {
    if (position_.x - radius_ <= 0) {
      position_.x = radius_;
      velocity_.x *= -1;
    }
    if (position_.x + radius_ >= screen_width) {
      position_.x = screen_width - radius_;
      velocity_.x *= -1;
    }
    if (position_.y - radius_ <= 0) {
      position_.y = radius_;
      velocity_.y *= -1;
    }
    if (position_.y + radius_ >= screen_height) {
      position_.y = screen_height - radius_;
      velocity_.y *= -1;
    }
  }

public:
  Particle(Vector2 p, Vector2 v, float r, Color c)
      : position_{p}, velocity_{v}, radius_{r}, colour_{c} {}

  Vector2 get_position() const { return position_; }
  float get_radius() const { return radius_; }
  void set_position(Vector2 p) { position_ = p; }

  void draw() const { DrawCircleV(position_, radius_, colour_); }

  void move(float screen_width, float screen_height) {
    position_.x += velocity_.x;
    position_.y += velocity_.y;
    check_boundaries(screen_width, screen_height);
  }
};

namespace Physics {
bool check_collision(const Particle &p1, const Particle &p2) {
  float dx{p2.get_position().x - p1.get_position().x};
  float dy{p2.get_position().y - p1.get_position().y};
  float distance_squared{(dx * dx) + (dy * dy)};

  float radii_sum{p1.get_radius() + p2.get_radius()};
  float radii_sum_squared{radii_sum * radii_sum};

  return distance_squared <= radii_sum_squared;
}

void resolve_collision(Particle &p1, Particle &p2) {}
} // namespace Physics

int main() {
  const float screen_width(800.0f);
  const float screen_height(400.0f);
  const float min_velocity(-2.0f);
  const float max_velocity(2.0f);
  const float min_radius(2.0f);
  const float max_radius(5.0f);
  const int num_particles{50};
  const Color particle_colour{RED};

  InitWindow(screen_width, screen_height, "particles are here");
  SetTargetFPS(60);

  std::random_device rd;
  std::mt19937 gen(rd());

  std::uniform_real_distribution<float> pos_x(max_radius,
                                              screen_width - max_radius);
  std::uniform_real_distribution<float> pos_y(max_radius,
                                              screen_height - max_radius);
  std::uniform_real_distribution<float> vel_x(min_velocity, max_velocity);
  std::uniform_real_distribution<float> vel_y(min_velocity, max_velocity);
  std::uniform_real_distribution<float> radius(min_radius, max_radius);

  std::vector<Particle> particles;
  particles.reserve(num_particles);

  for (int i{0}; i < num_particles; ++i) {
    Vector2 pos{pos_x(gen), pos_y(gen)};
    Vector2 vel{vel_x(gen), vel_y(gen)};
    float rad{radius(gen)};
    Color c{particle_colour};
    particles.emplace_back(pos, vel, rad, c);
  }

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    for (auto &particle : particles) {
      particle.move(screen_width, screen_height);
    }
    for (int i{0}; i < num_particles; ++i) {
      for (int j{i + 1}; j < num_particles; ++j) {
        if (Physics::check_collision(particles[i], particles[j])) {
          Physics::resolve_collision(particles[i], particles[j]);
        }
      }
    }

    for (const auto &particle : particles) {
      particle.draw();
    }

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
