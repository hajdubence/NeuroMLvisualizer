#ifndef PARSER_H
#define PARSER_H

#include <map>
#include <cstring>
#include "common_structs.h"
#include "cell_renderer.h"
#include "pugixml/pugixml.hpp"


class Parser
{
public:

    static void readC302Directory(std::map<std::string, CellRenderer*>* componentRendererMap);
    static std::vector<std::vector<float>> readDatFile(std::string datFilename);
    static std::vector<NetworkCell> readNetworkFile(std::string networkFile);
    static std::map<std::string, std::map<std::string, int>> readLemsFile(std::string lemsFile);
    static Cell readCell(pugi::xml_node CellNode);

private:

};

#endif
