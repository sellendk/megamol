/*
 * ExtractProbeGeometry.cpp
 * Copyright (C) 2019 by MegaMol Team
 * Alle Rechte vorbehalten.
 */

#include "ExtractProbeGeometry.h"

#include "mesh/MeshCalls.h"
#include "ProbeCalls.h"

megamol::probe::ExtractProbeGeometry::ExtractProbeGeometry() 
    : Module()
    , m_mesh_slot("deployMesh", "")
    , m_probe_slot("getProbes", "") {

    this->m_mesh_slot.SetCallback(
        mesh::CallMesh::ClassName(), mesh::CallMesh::FunctionName(0), &ExtractProbeGeometry::getData);
    this->m_mesh_slot.SetCallback(
        mesh::CallMesh::ClassName(), mesh::CallMesh::FunctionName(1), &ExtractProbeGeometry::getMetaData);
    this->MakeSlotAvailable(&this->m_mesh_slot);

    this->m_probe_slot.SetCompatibleCall<CallProbesDescription>();
    this->MakeSlotAvailable(&this->m_probe_slot);
}

megamol::probe::ExtractProbeGeometry::~ExtractProbeGeometry() {
    this->Release();
}

bool megamol::probe::ExtractProbeGeometry::create() { return true; }

void megamol::probe::ExtractProbeGeometry::release() {}

bool megamol::probe::ExtractProbeGeometry::convertToLine(core::Call& call) {

    auto* cm = dynamic_cast<mesh::CallMesh*>(&call);
    mesh::MeshDataAccessCollection line;

    auto probe_count = this->m_probes->getNumProbes();

    _index_data = {0,1};
    _line_indices.type = mesh::MeshDataAccessCollection::ValueType::UNSIGNED_INT;
    _line_indices.byte_size = _index_data.size() * sizeof(uint32_t);
    _line_indices.data = reinterpret_cast<uint8_t*>(_index_data.data());

    _line_attribs.resize(probe_count);
    _vertex_data.resize(2*probe_count);

//#pragma omp parallel for
    for (auto i = 0; i < probe_count; i++) {
        auto probe = this->m_probes->getProbe<FloatProbe>(i);

        std::array<float,4> vert1, vert2;

        vert1[0] = probe.m_position[0] + probe.m_direction[0] * probe.m_begin;
        vert1[1] = probe.m_position[1] + probe.m_direction[1] * probe.m_begin;
        vert1[2] = probe.m_position[2] + probe.m_direction[2] * probe.m_begin;
        vert1[3] = 1.0f;

        vert2[0] = probe.m_position[0] + probe.m_direction[0] * probe.m_end;
        vert2[1] = probe.m_position[1] + probe.m_direction[1] * probe.m_end;
        vert2[2] = probe.m_position[2] + probe.m_direction[2] * probe.m_end;
        vert2[3] = 1.0f;

        _vertex_data[2 * i + 0] = std::move(vert1);
        _vertex_data[2 * i + 1] = std::move(vert2);

        _line_attribs[i].resize(1);
        _line_attribs[i][0].semantic = mesh::MeshDataAccessCollection::AttributeSemanticType::POSITION;
        _line_attribs[i][0].component_type = mesh::MeshDataAccessCollection::ValueType::FLOAT;
        _line_attribs[i][0].byte_size = 2 * sizeof(std::array<float, 4>);
        _line_attribs[i][0].component_cnt = 3;
        _line_attribs[i][0].stride = 4 * sizeof(float);
        _line_attribs[i][0].data = reinterpret_cast<uint8_t*>(&_vertex_data[2 * i + 0]);

        // put data in line
        line.addMesh(_line_attribs[i], _line_indices);
    }
    cm->setData(std::make_shared<mesh::MeshDataAccessCollection>(std::move(line)));

    return true;
}

bool megamol::probe::ExtractProbeGeometry::getData(core::Call& call) {
    
    auto* cm = dynamic_cast<mesh::CallMesh*>(&call);
    auto* cp = this->m_probe_slot.CallAs<CallProbes>();


    if (cp == nullptr) return false;

    auto mesh_meta_data = cm->getMetaData();
    auto probe_meta_data = cp->getMetaData();


    if (mesh_meta_data.m_data_hash == m_mesh_cached_hash && probe_meta_data.m_data_hash == m_probe_cached_hash )
        return true;

    if (!(*cp)(0)) return false;


    mesh_meta_data = cm->getMetaData();
    probe_meta_data = cp->getMetaData();

    mesh_meta_data.m_bboxs = probe_meta_data.m_bboxs;

    m_probes = cp->getData();

    // here something really happens
    this->convertToLine(call);

    m_probe_cached_hash = probe_meta_data.m_data_hash;
    m_mesh_cached_hash++;
    mesh_meta_data.m_data_hash = m_mesh_cached_hash;
    cm->setMetaData(mesh_meta_data);

    return true; 
}

bool megamol::probe::ExtractProbeGeometry::getMetaData(core::Call& call) {
    
    auto* cm = dynamic_cast<mesh::CallMesh*>(&call);
    auto* cp = this->m_probe_slot.CallAs<CallProbes>();

    if (cp == nullptr) return false;

    // set frame id before callback
    auto mesh_meta_data = cm->getMetaData();
    auto probe_meta_data = cp->getMetaData();

    probe_meta_data.m_frame_ID = mesh_meta_data.m_frame_ID;
    cp->setMetaData(probe_meta_data);

    if (!(*cp)(1)) return false;

    probe_meta_data = cp->getMetaData();
    mesh_meta_data.m_frame_cnt = probe_meta_data.m_frame_cnt;
    mesh_meta_data.m_bboxs = probe_meta_data.m_bboxs; // normally not available here

    cm->setMetaData(mesh_meta_data);

    return true;
}
