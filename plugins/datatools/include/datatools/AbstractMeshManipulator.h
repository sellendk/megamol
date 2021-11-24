/*
 * AbstractMeshManipulator.h
 *
 * Copyright (C) 2019 MegaMol Dev Team
 * Alle Rechte vorbehalten.
 */
#pragma once

#include "geometry_calls_GL/CallTriMeshDataGL.h"
#include "AbstractManipulator.h"

namespace megamol {
namespace datatools {

using AbstractMeshManipulator = AbstractManipulator<geocalls_gl::CallTriMeshDataGL>;

} /* end namespace datatools */
} /* end namespace megamol */
