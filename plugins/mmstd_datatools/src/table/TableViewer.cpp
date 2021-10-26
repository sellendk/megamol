#include "stdafx.h"
#include "TableViewer.h"

#include "mmcore/utility/log/Log.h"
#include "imgui.h"
#include "imgui_internal.h"

using namespace megamol::stdplugin::datatools;
using namespace megamol;

/*
 * Table::ParticlesToTable
 */
TableViewer::TableViewer(void)
    : Module()
    , slotTableOut("floattableout", "Provides the table.")
    , slotTableIn("floattablein", "Ingests the table.") {

    /* Register parameters. */

    /* Register calls. */
    this->slotTableOut.SetCallback(
        table::TableDataCall::ClassName(), "GetData", &TableViewer::getTableData);
    this->slotTableOut.SetCallback(
        table::TableDataCall::ClassName(), "GetHash", &TableViewer::getTableHash);

    this->MakeSlotAvailable(&this->slotTableOut);

    this->slotTableIn.SetCompatibleCall<table::TableDataCallDescription>();
    this->MakeSlotAvailable(&this->slotTableIn);
}


/*
 * TableToParticles::~TableToParticles
 */
TableViewer::~TableViewer(void) { this->Release(); }


/*
 * megamol::pcl::PclDataSource::create
 */
bool TableViewer::create(void) {
    return true;
}

/*
 * megamol::pcl::PclDataSource::getMultiParticleData
 */
bool TableViewer::getTableData(core::Call& call) {
    auto* t_in = this->slotTableIn.CallAs<table::TableDataCall>();

    auto* t_out = dynamic_cast<table::TableDataCall*>(&call);
    if (t_out == nullptr) return false;

    if (t_in != nullptr) {
        t_in->SetFrameID(t_out->GetFrameID());
        if (!(*t_in)(0)) return false;

        if (t_in->GetColumnsCount() > 0) {
            drawTable(t_in);
        }

        t_out->SetFrameCount(t_in->GetFrameCount());
        t_out->SetFrameID(t_in->GetFrameID());
        t_out->Set(t_in->GetColumnsCount(), t_in->GetRowsCount(), t_in->GetColumnsInfos(), t_in->GetData());
    } else {
        return false;
    }
    return true;
}


bool TableViewer::getTableHash(core::Call& call) {
    auto* t_in = this->slotTableIn.CallAs<table::TableDataCall>();

    auto* t_out = dynamic_cast<table::TableDataCall*>(&call);
    if (t_out == nullptr) return false;

    if (t_in != nullptr) {
        t_in->SetFrameID(t_out->GetFrameID());
        if (!(*t_in)(1)) return false;
        t_out->SetDataHash(t_in->DataHash());
        t_out->SetFrameCount(t_in->GetFrameCount());
        return true;
    }
    return false;
}


/*
 * megamol::pcl::PclDataSource::release
 */
void TableViewer::release(void) {
    
}


void TableViewer::drawTable(table::TableDataCall* t_in) {
    bool valid_imgui_scope = ((ImGui::GetCurrentContext() != nullptr) ? (ImGui::GetCurrentContext()->WithinFrameScope) : (false));
    if (!valid_imgui_scope) return;

    std::string table_name = "##table";
    table_name += this->Name();

    ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable
            | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
            | ImGuiTableFlags_ScrollY;

    if (ImGui::BeginTable(table_name.c_str(), t_in->GetColumnsCount() + 1, flags)) {
        ImGui::TableSetupColumn("row", ImGuiTableColumnFlags_WidthFixed);
        for(auto c_idx = 0; c_idx < t_in->GetColumnsCount(); ++c_idx) {
            auto c = t_in->GetColumnsInfos()[c_idx];
            ImGui::TableSetupColumn(c.Name().c_str(), ImGuiTableColumnFlags_WidthFixed);
        }
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();
        ImGuiListClipper clipper;
        clipper.Begin(t_in->GetRowsCount());
        while(clipper.Step()) {
            for (auto row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row) {
                ImGui::PushID(row);
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("%010d", row);
                const auto row_data = t_in->GetData(row);
                for(auto c_idx = 0; c_idx < t_in->GetColumnsCount(); ++c_idx) {
                    ImGui::TableNextColumn();
                    ImGui::Text("%04.4f", row_data[c_idx]);
                }
                ImGui::PopID();
            }
        }
        ImGui::EndTable();
    }
}
