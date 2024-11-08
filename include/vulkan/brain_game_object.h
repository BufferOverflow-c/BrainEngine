#ifndef BRAIN_GAME_OBJECT_H
#define BRAIN_GAME_OBJECT_H

#include "brain_model.h"

// std
#include <memory>

struct Transform2dComponent {
  glm::vec2 translation{}; // (position offset)
  glm::vec2 scale{1.f, 1.f};
  float rotation{};
  glm::mat2 mat2() {
    const float s = glm::sin(rotation);
    const float c = glm::cos(rotation);
    glm::mat2 rotMatrix{{c, s}, {-s, c}};

    glm::mat2 scaleMat{{scale.x, 0.f}, {0.f, scale.y}};
    return rotMatrix * scaleMat;
  };
};

namespace brn {
class BrnGameObject {
public:
  using id_t = unsigned int;

  static BrnGameObject createGameObject() {
    static id_t currentId = 0;
    return BrnGameObject{currentId++};
  }

  BrnGameObject(const BrnGameObject &) = delete;
  BrnGameObject &operator=(const BrnGameObject &) = delete;
  BrnGameObject(BrnGameObject &&) = default;
  BrnGameObject &operator=(BrnGameObject &&) = default;

  const id_t getId() { return id; }

  std::shared_ptr<BrnModel> model{};
  glm::vec3 color{};
  Transform2dComponent transform2d{};

private:
  BrnGameObject(id_t objId) : id{objId} {}

  id_t id;
};
} // namespace brn

#endif // ! BRAIN_GAME_OBJECT_H
