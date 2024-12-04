#ifndef BRAIN_GAME_OBJECT_H
#define BRAIN_GAME_OBJECT_H

#include "brain_model.h"
#include <glm/gtc/matrix_transform.hpp>
// std
#include <memory>
#include <unordered_map>

namespace brn {

struct TransformComponent {
  glm::vec3 translation{}; // (position offset)
  glm::vec3 scale{1.f, 1.f, 1.f};
  glm::vec3 rotation{};

  // Matrix corresponds to tranlation * Ry * Rx * Rz * scale transformation
  // Rotation convention uses tait-bryan angles with axis order Y(1), X(2), Z(3)
  glm::mat4 mat4();
  glm::mat3 normalMatrix();
};

struct PointLightComponent {
  float lightIntensity = 1.f;
};

class BrnGameObject {
public:
  using id_t = unsigned int;
  using Map = std::unordered_map<id_t, BrnGameObject>;

  static BrnGameObject createGameObject() {
    static id_t currentId = 0;
    return BrnGameObject{currentId++};
  }

  static BrnGameObject makePointLight(float intensity = 10.f,
                                      float radius = 0.2f,
                                      glm::vec3 color = glm::vec3(1.f));

  BrnGameObject(const BrnGameObject &) = delete;
  BrnGameObject &operator=(const BrnGameObject &) = delete;
  BrnGameObject(BrnGameObject &&) = default;
  BrnGameObject &operator=(BrnGameObject &&) = default;

  const id_t getId() { return id; }

  glm::vec3 color{};
  TransformComponent transform{};

  // Optional pointer components
  std::shared_ptr<BrnModel> model{};
  std::unique_ptr<PointLightComponent> pointLight = nullptr;

private:
  BrnGameObject(id_t objId) : id{objId} {}

  id_t id;
};
} // namespace brn

#endif // ! BRAIN_GAME_OBJECT_H
