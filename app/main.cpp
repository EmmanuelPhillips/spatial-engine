#include <cmath>
#include <raylib.h>
#include <raymath.h>

#include <array>
#include <iostream>
#include <memory>
#include <optional>
#include <random>
#include <stdexcept>
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
  Vector2 get_velocity() const { return velocity_; }
  float get_radius() const { return radius_; }
  void set_position(Vector2 p) { position_ = p; }
  void set_velocity(Vector2 v) { velocity_ = v; }

  void draw() const { DrawCircleV(position_, radius_, colour_); }

  void update(float screen_width, float screen_height) {
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

std::optional<Vector2> resolve_overlap(Particle &p1, Particle &p2) {
  float dx{p2.get_position().x - p1.get_position().x};
  float dy{p2.get_position().y - p1.get_position().y};
  float distance{std::sqrt(dx * dx + dy * dy)};

  if (distance == 0) {
    return std::nullopt;
  }

  float unit_dx{dx / distance};
  float unit_dy{dy / distance};
  Vector2 unit_normal{unit_dx, unit_dy};

  float touching_distance{p1.get_radius() + p2.get_radius()};
  float overlap{touching_distance - distance};
  if (overlap <= 0) {
    return std::nullopt;
  }
  float push{overlap / 2};

  Vector2 push_vector{
      Vector2{unit_normal.x * push, unit_normal.y * push}}; // vector2scale
  p1.set_position(Vector2Subtract(p1.get_position(), push_vector));
  p2.set_position(Vector2Add(p2.get_position(), push_vector));

  return unit_normal;
}

void resolve_velocities(Particle &p1, Particle &p2,
                        const Vector2 &unit_normal) {
  Vector2 p1_velocity{p1.get_velocity()};
  Vector2 p2_velocity{p2.get_velocity()};
  Vector2 relative_velocity{Vector2Subtract(p1_velocity, p2_velocity)};

  float approach_speed{Vector2DotProduct(relative_velocity, unit_normal)};
  if (approach_speed <= 0) {
    return;
  }

  Vector2 impulse{Vector2Scale(unit_normal, approach_speed)};
  p1.set_velocity(Vector2Subtract(p1.get_velocity(), impulse));
  p2.set_velocity(Vector2Add(p2.get_velocity(), impulse));
}

void resolve_collision(Particle &p1, Particle &p2) {
  if (check_collision(p1, p2)) {
    auto result{resolve_overlap(p1, p2)};
    if (!result) {
      return;
    }
    Vector2 unit_normal{result.value()};
    resolve_velocities(p1, p2, unit_normal);
  }
}

} // namespace Physics

class Quadtree {
public:
  struct Entity {
    int index;
    Vector2 position;
  };

private:
  Rectangle bounds_;
  int capacity_;
  int current_depth_;
  int max_depth_;
  bool divided_;

  std::vector<Entity> entities_;
  std::array<std::unique_ptr<Quadtree>, 4> child_nodes_;

public:
  Quadtree(Rectangle b, int c = 4, int curr_d = 0, int max_d = 8)
      : bounds_{b}, capacity_{c}, current_depth_{curr_d}, max_depth_{max_d},
        divided_{false} {
    entities_.reserve(capacity_);
  }

  bool insert(const Entity &e) {
    if (!CheckCollisionPointRec(e.position, bounds_)) {
      return false;
    }
    if (!divided_ && (static_cast<int>(entities_.size()) < capacity_ ||
                      current_depth_ >= max_depth_)) {
      entities_.push_back(e);
      return true;
    }
    if (!divided_ && static_cast<int>(entities_.size()) >= capacity_ &&
        current_depth_ < max_depth_) {
      subdivide();
    }
    if (divided_) {
      for (auto &child : child_nodes_) {
        if (child->insert(e)) {
          return true;
        }
      }
    }

    return false;
  }

  void subdivide() {
    float half_width{bounds_.width / 2.0f};
    float half_height{bounds_.height / 2.0f};
    child_nodes_[0] = std::make_unique<Quadtree>(
        Rectangle{bounds_.x, bounds_.y, half_width, half_height}, capacity_,
        current_depth_ + 1, max_depth_);
    child_nodes_[1] = std::make_unique<Quadtree>(
        Rectangle{bounds_.x + half_width, bounds_.y, half_width, half_height},
        capacity_, current_depth_ + 1, max_depth_);
    child_nodes_[2] = std::make_unique<Quadtree>(
        Rectangle{bounds_.x, bounds_.y + half_height, half_width, half_height},
        capacity_, current_depth_ + 1, max_depth_);
    child_nodes_[3] = std::make_unique<Quadtree>(
        Rectangle{bounds_.x + half_width, bounds_.y + half_height, half_width,
                  half_height},
        capacity_, current_depth_ + 1, max_depth_);

    divided_ = true;
    for (auto &entity : entities_) {
      insert(entity);
    }
    entities_.clear();
  }

  void query(Rectangle range, std::vector<Entity> &found) const {
    if (!CheckCollisionRecs(range, bounds_)) {
      return;
    }
    if (divided_) {
      for (const auto &child : child_nodes_) {
        child->query(range, found);
      }
      return;
    }
    for (const auto &entity : entities_) {
      if (CheckCollisionPointRec(entity.position, range)) {
        found.push_back(entity);
      }
    }
  }
};

int main() {
  const float screen_width(1280.0f);
  const float screen_height(720.0f);
  const float min_velocity(-2.0f);
  const float max_velocity(2.0f);
  const float min_radius(7.5f);
  const float max_radius(7.5f);
  const int num_particles{2000};
  const Color particle_colour{RED};

  Rectangle screen_bounds{0, 0, screen_width, screen_height};

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

    particles.emplace_back(pos, vel, rad, particle_colour);
  }

  std::vector<Quadtree::Entity> candidates;

  while (!WindowShouldClose()) {

    for (auto &particle : particles) {
      particle.update(screen_width, screen_height);
    }

    Quadtree tree{screen_bounds};

    for (int i{0}; i < static_cast<int>(particles.size()); ++i) {
      tree.insert({i, particles[i].get_position()});
    }

    for (int i{0}; i < static_cast<int>(particles.size()); ++i) {

      Vector2 position{particles[i].get_position()};

      float search_distance{particles[i].get_radius() + max_radius};

      Rectangle search_range{position.x - search_distance,
                             position.y - search_distance,
                             search_distance * 2.0f, search_distance * 2.0f};

      candidates.clear();
      tree.query(search_range, candidates);

      for (const auto &candidate : candidates) {

        if (candidate.index <= i) {
          continue;
        }

        Physics::resolve_collision(particles[i], particles[candidate.index]);
      }
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    for (const auto &particle : particles) {
      particle.draw();
    }

    DrawFPS(0, 0);
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
