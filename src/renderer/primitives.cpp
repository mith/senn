#include "primitives.hpp"

#include <glm/gtx/transform.hpp>

glm::mat4 model_matrix(const ObjectAttribes& object_attributes)
{
    return glm::translate(object_attributes.position)
         * glm::mat4_cast(object_attributes.orientation)
         * glm::scale(object_attributes.scale);
}
