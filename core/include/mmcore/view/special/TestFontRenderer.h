/*
* TestFontRenderer.h
*
* Copyright (C) 2010 by VISUS (Universitaet Stuttgart)
* Alle Rechte vorbehalten.
*/
/*
* This renderer serves only test purposes for new sdf font rendering and comparison to other font renderings
*/

#ifndef MEGAMOLCORE_TEST_FONT_RENDERER_H_INCLUDED
#define MEGAMOLCORE_TEST_FONT_RENDERER_H_INCLUDED
#if (defined(_MSC_VER) && (_MSC_VER > 1000))
#pragma once
#endif /* (defined(_MSC_VER) && (_MSC_VER > 1000)) */


#include "mmcore/view/Renderer3DModule.h"
#include "mmcore/param/ParamSlot.h"

#include "mmcore/view/special/SDFFont.h"

#include "vislib/graphics/gl/SimpleFont.h"
#include "vislib/graphics/gl/OutlineFont.h"


namespace megamol {
    namespace core {
        namespace view {
            namespace special {

        /**
        * Test renderer for fonts.
        */
        class TestFontRenderer : public megamol::core::view::Renderer3DModule {
        public:

            /**
            * Answer the name of this module.
            *
            * @return The name of this module.
            */
            static const char *ClassName(void) {
                return "TestFontRenderer";
            }

            /**
            * Answer a human readable description of this module.
            *
            * @return A human readable description of this module.
            */
            static const char *Description(void) {
                return "Test renderer for fonts.";
            }

            /**
            * Answers whether this module is available on the current system.
            *
            * @return 'true' if the module is available, 'false' otherwise.
            */
            static bool IsAvailable(void) {
                return true;
            }

            /** Ctor. */
            TestFontRenderer(void);

            /** Dtor. */
            virtual ~TestFontRenderer(void);

        protected:

            /**
            * Implementation of 'Create'.
            *
            * @return 'true' on success, 'false' otherwise.
            */
            virtual bool create(void);

            /**
            * Implementation of 'Release'.
            */
            virtual void release(void);

            /**
            * The get capabilities callback. The module should set the members
            * of 'call' to tell the caller its capabilities.
            *
            * @param call The calling call.
            *
            * @return The return value of the function.
            */
            virtual bool GetCapabilities(core::Call& call);

            /**
            * The get extents callback. The module should set the members of
            * 'call' to tell the caller the extents of its data (bounding boxes
            * and times).
            *
            * @param call The calling call.
            *
            * @return The return value of the function.
            */
            virtual bool GetExtents(core::Call& call);

            /**
            * The render callback.
            *
            * @param call The calling call.
            *
            * @return The return value of the function.
            */
            virtual bool Render(core::Call& call);

        private:

            /**********************************************************************
            * variables
            **********************************************************************/

            // font rendering
            vislib::graphics::gl::SimpleFont      simpleFont;
            vislib::graphics::gl::OutlineFont     outlineFont;
            megamol::core::view::special::SDFFont sdfFont;


            unsigned int renderMode;
            vislib::StringA testtext;

            /**********************************************************************
            * functions
            **********************************************************************/


            /**********************************************************************
            * parameters
            **********************************************************************/

            /**  */
            core::param::ParamSlot paramRenderMode1;
            /**  */
            core::param::ParamSlot paramRenderMode2;
            /**  */
            core::param::ParamSlot paramRenderMode3;
            /**  */
            core::param::ParamSlot paramRenderMode4;
            /**  */
            core::param::ParamSlot paramRenderMode5;
            /**  */
            core::param::ParamSlot paramRenderMode6;
            /**  */
            core::param::ParamSlot paramRenderMode7;
            /**  */
            core::param::ParamSlot paramRenderMode8;
            /**  */
            core::param::ParamSlot paramRenderMode9;
        };

            } /* end namespace special */
        } /* end namespace view */
    } /* end namespace core */
} /* end namespace megamol */

#endif /* MEGAMOLCORE_TEST_FONT_RENDERER_H_INCLUDED */
