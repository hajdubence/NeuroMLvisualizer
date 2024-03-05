#include <filesystem>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "parser.h"


namespace fs = std::filesystem;

void Parser::readC302Directory(std::map<std::string, CellRenderer*>* componentRendererMap) {
    try {
        // Iterate through the directory
        fs::path folderPath = "c302\\examples\\cells";
        for (const auto& entry : fs::directory_iterator(folderPath)) {
            // Check if the entry is a regular file
            if (fs::is_regular_file(entry.path())) {
                // Print the file name
                char filepath[100] = "c302\\examples\\cells\\";
                int originalLength = strlen(filepath);
                strcat(filepath, entry.path().filename().string().c_str());

                // Check if it ends with "_D.cell.nml"
                size_t length = strlen(filepath);
                const char* suffix = "_D.cell.nml";
                if (length >= strlen(suffix) && strcmp(filepath + length - strlen(suffix), suffix) == 0) {

                }
                else {
                    //c302\examples\cells\????????.cell.nml
                    std::cout << filepath << std::endl;
                    std::string filepathString = filepath;
                    std::string id = filepathString.substr(originalLength, length - originalLength - 9);
                    (*componentRendererMap)[id] = new CellRenderer(filepath);
                }

            }
        }
    }
    catch (const fs::filesystem_error& e) {
        std::string err = "Error accessing the directory: ";
        throw std::runtime_error(err.append(e.what()));
    }
}

std::vector<std::vector<float>> Parser::readDatFile(std::string datFilename) {
    std::vector<std::vector<float>> matrix;
    std::ifstream file(datFilename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << datFilename << std::endl;
    }
    std::cout << "Parsing file: " << datFilename << std::endl;
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

    file.close();

    return matrix;
}

std::vector<NetworkCell> Parser::readNetworkFile(std::string networkFile) {
    std::vector<NetworkCell> displayNeurons;
    pugi::xml_document doc1;
    if (!doc1.load_file(networkFile.c_str())) {
        std::cerr << "Failed to load file" << std::endl;
    }
    for (pugi::xml_node networkChild = doc1.select_node("//network").node().first_child(); networkChild; networkChild = networkChild.next_sibling()) {
        if (!strcmp(networkChild.name(), "population")) {
            pugi::xml_node population = networkChild;
            std::string id = population.attribute("id").value();
            std::string component = population.attribute("component").value();
            pugi::xml_node locationNode = population.child("instance").child("location");
            glm::vec3 location = glm::vec3(
                locationNode.attribute("x").as_float(),
                locationNode.attribute("y").as_float(),
                locationNode.attribute("z").as_float());
            displayNeurons.push_back(NetworkCell(id, component, location));
        }
    }
    return displayNeurons;
}

std::map<std::string, std::map<std::string, int>> Parser::readLemsFile(std::string lemsFile) {
    pugi::xml_document doc;
    if (!doc.load_file(lemsFile.c_str())) {
        std::cerr << "Failed to load file" << std::endl;
    }
    std::map<std::string, std::map<std::string, int>> ColumnIdMap;
    for (pugi::xml_node child = doc.select_node("//Simulation").node().first_child(); child; child = child.next_sibling()) {
        if (!strcmp(child.name(), "OutputFile")) {
            std::string fileName = child.attribute("fileName").value();
            std::cout << "OutputFile: " << fileName;
            std::map<std::string, int> IdMap;
            ColumnIdMap[fileName] = IdMap;
            int i = 0;
            for (pugi::xml_node outputColumn = child.first_child(); outputColumn; outputColumn = outputColumn.next_sibling()) {
                std::string quantity = outputColumn.attribute("quantity").value();
                std::string id = quantity.substr(0, quantity.find("/"));
                ColumnIdMap[fileName][id] = i;
                i++;
            }
            std::cout << " should contain " << ColumnIdMap[fileName].size() << " columns" << std::endl;
        }
    }
    return ColumnIdMap;
}

Cell Parser::readCell(pugi::xml_node CellNode) {
    Cell cell = Cell();
    cell.id = CellNode.attribute("id").as_string();
    std::map<int, glm::vec4> distalMap;
    pugi::xml_node morphology = CellNode.select_node("morphology").node();

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

    return cell;
}
