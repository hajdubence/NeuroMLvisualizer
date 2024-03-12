#include "enviroment.h"
#include <cstring>
#include <iostream>
#include <filesystem>
#include <stdio.h>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

void Enviroment::readFile(const char* path) {
    pugi::xml_document doc;
    if (!doc.load_file(path)) {
        std::cerr << "Failed to load file: " << path << std::endl;
    }
    for (pugi::xpath_node lems : doc.select_nodes("/Lems")) {
        parseLemsNode(lems.node(), path);
    }
    for (pugi::xpath_node neuroml : doc.select_nodes("/neuroml")) {
        parseNeuromlNode(neuroml.node(), path);
    }
}



void Enviroment::parseLemsNode(pugi::xml_node lemsNode, const char* path) {
    for (pugi::xml_node child = lemsNode.first_child(); child; child = child.next_sibling()) {
        if (!strcmp(child.name(), "Include")) {
            parseLemsIncludeNode(child, path);
        }
        if (!strcmp(child.name(), "Simulation")) {
            parseSimulationNode(child, path);
        }
    }
}

void Enviroment::parseLemsIncludeNode(pugi::xml_node includeNode, const char* path) {
    std::string file = includeNode.attribute("file").as_string();
    // core NeuroML2 ComponentType definitions
    if (file == "Cells.xml" || file == "Networks.xml" || file == "Simulation.xml") {
        return;
    }
    fs::path dirPath = fs::path(path).parent_path();
    readFile((dirPath / file).string().c_str());
}

void Enviroment::parseSimulationNode(pugi::xml_node simulationNode, const char* path) {
    for (pugi::xml_node child = simulationNode.first_child(); child; child = child.next_sibling()) {
        if (!strcmp(child.name(), "OutputFile")) {
            std::string fileName = child.attribute("fileName").value();
            std::map<std::string, int> IdMap;
            outputFileCulumns[fileName] = IdMap;
            int i = 0;
            for (pugi::xml_node outputColumn = child.first_child(); outputColumn; outputColumn = outputColumn.next_sibling()) {
                std::string quantity = outputColumn.attribute("quantity").value();
                std::string id = quantity.substr(0, quantity.find("/"));
                outputFileCulumns[fileName][id] = i;
                i++;
            }
            std::cout << "OutputFile: " << fileName << " should contain " << outputFileCulumns[fileName].size() << " columns" << std::endl;
        }
    }
    readOutputFiles(path);
}

void Enviroment::readOutputFiles(const char* path) {
    fs::path dirPath = fs::path(path).parent_path();
    for (auto it = outputFileCulumns.begin(); it != outputFileCulumns.end(); ++it) {
        std::string fileName = it->first;
        std::vector<std::vector<float>> matrix;
        std::ifstream file((dirPath / fileName).string().c_str());
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << fileName << std::endl;
        }
        std::cout << "Parsing file: " << fileName << std::endl;
        std::string line;
        while (std::getline(file, line)) {
            std::vector<float> row;
            std::istringstream iss(line);
            float value;
            while (iss >> value) {
                row.push_back(value);
            }
            matrix.push_back(row);
        }
        outputFiles[fileName] = matrix;
        file.close();
    }
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

            pugi::xml_node distal = child.child("distal");
            dist = glm::vec4(
                distal.attribute("x").as_float(),
                distal.attribute("y").as_float(),
                distal.attribute("z").as_float(),
                distal.attribute("diameter").as_float());
            distalMap[segment_id] = dist;

            cell.AddSegment(prox, dist);
        }
    }
    
    cellRenderers[cell.id] = new CellRenderer(cell);
}

void Enviroment::parseNetworkNode(pugi::xml_node networkNode) {
    for (pugi::xml_node child = networkNode.first_child(); child; child = child.next_sibling()) {
        if (!strcmp(child.name(), "population")) {
            pugi::xml_node population = child;
            std::string id = population.attribute("id").value();
            std::string component = population.attribute("component").value();
            pugi::xml_node locationNode = population.child("instance").child("location");
            glm::vec3 location = glm::vec3(
                locationNode.attribute("x").as_float(),
                locationNode.attribute("y").as_float(),
                locationNode.attribute("z").as_float());
            networkCells.push_back(NetworkCell(id, component, location));
        }
    }
}
