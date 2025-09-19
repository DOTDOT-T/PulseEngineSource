/**
 * @file pch.h
 * @author Dorian LEXTERIAQUE (dlexteriaque@gmail.com)
 * @brief pre compiled headers
 * @version 0.1
 * @date 2025-09-18
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef PULSE_PCH_H
#define PULSE_PCH_H

// === Standard libraries ===
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <memory>

// === engine editor libraries ===
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui-node/imgui_node_editor.h"
#include "imgui-node/imgui_node_editor_internal.h"
namespace ed = ax::NodeEditor;

// === web communication libraries ===
#define ASIO_STANDALONE
#include <asio.hpp>
#include "json.hpp"

#endif