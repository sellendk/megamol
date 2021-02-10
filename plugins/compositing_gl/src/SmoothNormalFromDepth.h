/*
 * SmoothNormalFromDepth.h
 *
 * Copyright (C) 2021 by Universitaet Stuttgart (VISUS).
 * All rights reserved.
 */

#ifndef SMOOTH_NORMAL_FROM_DEPTH_H_INCLUDED
#define SMOOTH_NORMAL_FROM_DEPTH_H_INCLUDED

#include <memory>

#include "mmcore/utility/plugins/Plugin200Instance.h"
#include "mmcore/CalleeSlot.h"
#include "mmcore/CallerSlot.h"
#include "mmcore/param/ParamSlot.h"

#define GLOWL_OPENGL_INCLUDE_GLAD
#include "glowl/GLSLProgram.hpp"
#include "glowl/Texture2D.hpp"

namespace megamol {
namespace compositing {

    class SmoothNormalFromDepth : public core::Module {
    public:
        /**
         * Answer the name of this module.
         *
         * @return The name of this module.
         */
        static const char* ClassName() {
            return "SmoothNormalFromDepth";
        }

        /**
         * Answer a human readable description of this module.
         *
         * @return A human readable description of this module.
         */
        static const char* Description() {
            return "Compositing module that reconstructs smooth normals from depth.";
        }

        /**
         * Answers whether this module is available on the current system.
         *
         * @return 'true' if the module is available, 'false' otherwise.
         */
        static bool IsAvailable() {
            return true;
        }

        SmoothNormalFromDepth();
        ~SmoothNormalFromDepth();

    protected:
        /**
         * Implementation of 'Create'.
         *
         * @return 'true' on success, 'false' otherwise.
         */
        bool create();

        /**
         * Implementation of 'Release'.
         */
        void release();

        /**
         * TODO
         */
        bool getDataCallback(core::Call& caller);

        /**
         * TODO
         */
        bool getMetaDataCallback(core::Call& caller);

    private:

        uint32_t m_version;

        /** Shader program for texture add */
        std::unique_ptr<glowl::GLSLProgram> m_smooth_normal_from_depth_prgm;

        /** Texture that the combination result will be written to */
        std::shared_ptr<glowl::Texture2D> m_output_texture;

        /** Slot for requesting the output textures from this module, i.e. lhs connection */
        megamol::core::CalleeSlot m_output_tex_slot;

        /** Slot for querying primary input texture, i.e. a rhs connection */
        megamol::core::CallerSlot m_input_tex_slot;

        /** Slot for querying camera, i.e. a rhs connection */
        megamol::core::CallerSlot m_camera_slot;
    };

} // namespace compositing
} // namespace megamol

#endif // !SMOOTH_NORMAL_FROM_DEPTH_H_INCLUDED
