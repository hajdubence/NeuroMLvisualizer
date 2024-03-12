#ifndef ENVIROMENT
#define ENVIROMENT

#include <vector>
#include <map>
#include "cell_renderer.h"
#include "common_structs.h"
#include "pugixml/pugixml.hpp"

class Enviroment {

public:

	std::map<std::string, CellRenderer*> cellRenderers;
	std::vector<NetworkCell> networkCells;
	std::map<std::string, std::map<std::string, int>> outputFileCulumns;
	std::map<std::string, std::vector<std::vector<float>>> outputFiles;

	void readFile(const char* path);

private:
	void parseLemsNode(pugi::xml_node lemsNode, const char* path);
	void parseLemsIncludeNode(pugi::xml_node includeNode, const char* path);
	void parseSimulationNode(pugi::xml_node simulationNode, const char* path);
	void readOutputFiles(const char* path);

	void parseNeuromlNode(pugi::xml_node neuromlNode, const char* path);
	void parseIncludeNode(pugi::xml_node includeNode, const char* path);
	void parseCellNode(pugi::xml_node cellNode);
	void parseNetworkNode(pugi::xml_node networkNode);

};


#endif