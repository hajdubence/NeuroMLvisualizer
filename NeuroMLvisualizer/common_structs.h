#ifndef COMMON_STRUCTS
#define COMMON_STRUCTS

#include <string>
#include <vector>
#include <glm/glm.hpp>

struct Cell
{
    std::string id;
    // morpology
    std::vector<glm::vec4> cylinderVector;
    std::vector<glm::vec4> sphereVector;

    void AddSegment(glm::vec4 proximal, glm::vec4 distal) {
        if (proximal == distal) // sphere
        {
            sphereVector.push_back(proximal);
        }
        else // cylinder
        {
            cylinderVector.push_back(proximal);
            cylinderVector.push_back(distal);
        }
    }

};

struct NetworkCell
{
    std::string id;
    std::string component;
    glm::vec3 position;

    NetworkCell(std::string id, std::string component, glm::vec3 position) {
        this->id = id;
        this->component = component;
        this->position = position;
    }
};


#endif