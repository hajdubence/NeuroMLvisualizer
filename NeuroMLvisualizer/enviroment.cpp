#include "enviroment.h"
#include <cstring>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

void Enviroment::readFile(const char* path) {
    pugi::xml_document doc;
    if (!doc.load_file(path)) {
        std::cerr << "Failed to load file" << std::endl;
    }
    for (pugi::xpath_node lems : doc.select_nodes("/Lems")) {
        parseLemsNode(lems.node(), path);
    }
    for (pugi::xpath_node neuroml : doc.select_nodes("/neuroml")) {
        parseNeuromlNode(neuroml.node(), path);
    }
}

void Enviroment::parseLemsNode(pugi::xml_node lemsNode, const char* path) {

}

void Enviroment::parseNeuromlNode(pugi::xml_node neuromlNode, const char* path) {
    for (pugi::xml_node child = neuromlNode.first_child(); child; child = child.next_sibling()) {
        if (!strcmp(child.name(), "include")) {
            parseIncludeNode(child, path);
        }
        if (!strcmp(child.name(), "cell")) {
            parseCellNode(child);
        }
        if (!strcmp(child.name(), "network")) {
            parseNetworkNode(child);
        }
    }
}

void Enviroment::readDatFile() {

}

void Enviroment::parseIncludeNode(pugi::xml_node includeNode, const char* path) {
    std::string href = includeNode.attribute("href").as_string();
    fs::path dirPath = fs::path(path).parent_path();
    readFile((dirPath / href).string().c_str());
}

void Enviroment::parseCellNode(pugi::xml_node cellNode) {
    Cell cell = Cell();
    cell.id = cellNode.attribute("id").as_string();
    std::map<int, glm::vec4> distalMap;
    pugi::xml_node morphology = cellNode.select_node("morphology").node();

    for (pugi::xml_node child = morphology.first_child(); child; child = child.next_sibling()) {
        if (!strcmp(child.name(), "segment")) {

            int segment_id = child.attribute("id").as_int();
            glm::vec4 prox;
            glm::vec4 dist;
            if (child.child("proximal"))
            {
                pugi::xml_node proximal = child.child("proximal");
                prox = glm::vec4(
                    proximal.attribute("x").as_float(),
                    proximal.attribute("y").as_float(),
                    proximal.attribute("z").as_float(),
                    proximal.attribute("diameter").as_float());
            }
            else
            {
                // From the NeuroML documentation:
                // The proximal point can be omitted, usually because it is the same as a point on the parent segment
                int parent_id = child.child("parent").attribute("segment").as_int();
                prox = distalMap[parent_id];
            }
            if (child.child("distal"))
            {
                pugi::xml_node distal = child.child("distal");
                dist = glm::vec4(
                    distal.attribute("x").as_float(),
                    distal.attribute("y").as_float(),
                    distal.attribute("z").as_float(),
                    distal.attribute("diameter").as_float());
                distalMap[segment_id] = dist;
            }
            cell.AddSegment(prox, dist);

        }
    }
    
    cellRenderers.push_back(CellRenderer(cell));
}

void Enviroment::parseNetworkNode(pugi::xml_node networkNode) {

}
