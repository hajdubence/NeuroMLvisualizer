#!/bin/bash

CC=g++
SOUREFILES="NeuroMLvisualizer.cpp camera.cpp model_renderer.cpp cell_renderer.cpp shader.cpp shader_cylinder.cpp shader_triangle.cpp parser.cpp enviroment.cpp"
IMGUIFILES="imgui/imgui.cpp imgui/imgui_demo.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp imgui/imgui_impl_glfw.cpp imgui/imgui_impl_opengl3.cpp"
PUGIXMLFILES="pugixml/pugixml.cpp"
CPPFLAGS=`pkg-config --cflags glew glfw3 glm assimp`
CPPFLAGS="$CPPFLAGS"
LDFLAGS=`pkg-config --libs glew glfw3 glm assimp`
LDFLAGS="$LDFLAGS -ldl"

$CC $SOUREFILES $IMGUIFILES $PUGIXMLFILES $CPPFLAGS $LDFLAGS -o NeuroMLvisualizer
