/*
 * Configurator.h
 *
 * Copyright (C) 2019 by Universitaet Stuttgart (VIS).
 * Alle Rechte vorbehalten.
 */
// Creating a node graph editor for ImGui
// Quick demo, not production code! This is more of a demo of how to use ImGui to create custom stuff.
// Better version by @daniel_collin here https://gist.github.com/emoon/b8ff4b4ce4f1b43e79f2
// See https://github.com/ocornut/imgui/issues/306
// v0.03: fixed grid offset issue, inverted sign of 'scrolling'
// Animated gif: https://cloud.githubusercontent.com/assets/8225057/9472357/c0263c04-4b4c-11e5-9fdf-2cd4f33f6582.gif

#ifndef MEGAMOL_GUI_CONFIGURATOR_H_INCLUDED
#define MEGAMOL_GUI_CONFIGURATOR_H_INCLUDED

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"

#include <map>
#include <math.h> // fmodf
#include <tuple>

#include "mmcore/CoreInstance.h"
#include "mmcore/view//Input.h"

#include "FileUtils.h"
#include "GUIUtils.h"
#include "Graph.h"
#include "WindowManager.h"

#include "vislib/sys/Log.h"


namespace megamol {
namespace gui {

class Configurator {
public:
    /**
     * Initialises a new instance.
     */
    Configurator();

    /**
     * Finalises an instance.
     */
    virtual ~Configurator();

    /**
     * Draw configurator ImGui window.
     * (Call in GUIView::drawConfiguratorCallback())
     */
    bool Draw(WindowManager::WindowConfiguration& wc, megamol::core::CoreInstance* core_instance);

    /**
     * Checks if any hotkeys are pressed.
     * (Call in GUIView::OnKey())
     *
     * @return true when any hotkey is pressed.
     */
    bool CheckHotkeys(void);

private:
    // VARIABLES --------------------------------------------------------------

    typedef std::tuple<megamol::core::view::KeyCode, bool> HotkeyData;
    enum HotkeyIndex : size_t { MODULE_SEARCH = 0, INDEX_COUNT = 1 };

    std::array<HotkeyData, HotkeyIndex::INDEX_COUNT> hotkeys;

    Graph graph;
    GUIUtils utils;

    int window_rendering_state;
    std::string project_filename;

    struct State {
        int selected_module_list;
        int selected_module_graph;
        ImVec2 scrolling;
        float zooming;
        bool show_grid;
    } state;

    // FUNCTIONS --------------------------------------------------------------

    bool draw_window_menu(megamol::core::CoreInstance* core_instance);
    bool draw_window_module_list(void);
    bool draw_window_graph_canvas(void);
    bool draw_canvas_grid(ImVec2 scrolling, float zooming) const;
    bool draw_canvas_calls(Graph::CallGraphType calls, ImVec2 position_offset) const;
    bool draw_canvas_module_call_slots(
        Graph::ModulePtr mod, ImVec2 position_offset, float slot_radius, float slot_label_offset);
    bool draw_canvas_selected_call(ImVec2 position_offset);

    // ------------------------------------------------------------------------
};

} // namespace gui
} // namespace megamol

#endif // MEGAMOL_GUI_CONFIGURATOR_H_INCLUDED