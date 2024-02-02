/**
 * MegaMol
 * Copyright (c) 2021, MegaMol Dev Team
 * All rights reserved.
 */

#pragma once

#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glowl/glowl.h>

#include "CompositingOutHandler.h"
#include "mmcore/CalleeSlot.h"
#include "mmcore/CallerSlot.h"
#include "mmcore/param/ParamSlot.h"
#include "mmcore_gl/utility/ShaderFactory.h"

#include "mmstd_gl/ModuleGL.h"
#include "mmstd_gl/special/TextureInspector.h"

namespace megamol::compositing_gl {

class DepthOfField : public mmstd_gl::ModuleGL {
public:
    /**
     * Answer the name of this module.
     *
     * @return The name of this module.
     */
    static const char* ClassName() {
        return "DepthOfField";
    }

    /**
     * Answer a human readable description of this module.
     *
     * @return A human readable description of this module.
     */
    static const char* Description() {
        return "Compositing module that computes antialiasing";
    }

    /**
     * Answers whether this module is available on the current system.
     *
     * @return 'true' if the module is available, 'false' otherwise.
     */
    static bool IsAvailable() {
        return true;
    }

#ifdef MEGAMOL_USE_PROFILING
    static void requested_lifetime_resources(frontend_resources::ResourceRequest& req) {
        ModuleGL::requested_lifetime_resources(req);
        req.require<frontend_resources::PerformanceManager>();
    }
#endif

    DepthOfField();
    ~DepthOfField() override;

protected:
    /**
     * Implementation of 'Create'.
     *
     * @return 'true' on success, 'false' otherwise.
     */
    bool create() override;

    /**
     * Implementation of 'Release'.
     */
    void release() override;

    /**
     * TODO
     */
    bool getDataCallback(core::Call& caller);

    /**
     * TODO
     */
    bool getMetaDataCallback(core::Call& caller);

private:
    /**
     * Resets previously set GL states
     */
    inline void resetGLStates() {
        glUseProgram(0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        //glBindBuffer(ssbo_constants_->getTarget(), 0);
    }

    /**
     * \brief Sets GUI parameter slot visibility depending on antialiasing technique.
     */
    bool visibilityCallback(core::param::ParamSlot& slot);

    /**
     * \brief Sets texture format variables and recompiles shaders.
     *
     *  @return 'true' if updates sucessfull, 'false' otherwise
     */
    bool textureFormatUpdate();

    /**
     * \brief Sets the setting parameter values of the SMAAConstants struct depending
     * on the chosen quality level.
     */
    bool setSettingsCallback(core::param::ParamSlot& slot);

    /**
     * \brief First pass. Generates the circle of consfusion (coc) based on the focal plane.
     *                    The result of this step will be a full resolution coc texture.
     *
     * \param depth The depth texture from the input (texture to blur).
                    Used for calculating the circle of confusion.
     */
    void cocGeneration(const std::shared_ptr<glowl::Texture2D>& depth);

    /**
     * \brief Second pass. Downsampling of the color (input) and the coc texture.
     *                     The result is 3 quarter-sized textures:
     *                         - downsampled color texture
     *                         - downsampled color texture multiplied by far coc values
     *                         - downsampled coc texture
     *
     * \param color The input texture (texture to blur).
     * \param coc   The coc texture generated in the first pass.
     */
    void downsample(
        const std::shared_ptr<glowl::Texture2D>& color,
        const std::shared_ptr<glowl::Texture2D>& coc
    );

    /**
     * \brief Third pass. Generates a blurred version of the near coc values.
     *                    The result is a single quarter-sized blurred near coc texture.
     *
     * \param coc_4 The downsampled (quarter-sized) coc texture generated in the second pass.
     */
    void nearCoCBlur(const std::shared_ptr<glowl::Texture2D>& coc_4);

    /**
     * \brief Fourth pass. The actual depth of field calculation.
     *                     The result is 2 quarter-sized textures: one for the near and one for the far field.
     * 
     * \param color_4             The downsampled color texture generated in the second pass.
     * \param color_mul_coc_far_4 The downsampled color texture multiplied by far coc values generated in the second pass.
     * \param coc_4               The downsampled coc texture generated in the second pass.
     * \param coc_near_blurred_4  The downsampled blurred near coc texture generated in the third pass.
     */
    void computation(
        const std::shared_ptr<glowl::Texture2D>& color_4,
        const std::shared_ptr<glowl::Texture2D>& color_mul_coc_far_4,
        const std::shared_ptr<glowl::Texture2D>& coc_4,
        const std::shared_ptr<glowl::Texture2D>& coc_near_blurred_4
    );

    /**
     * \brief Fifth pass. Since the computation pass undersamples the color buffers, the near and far fields need to be filled.
     *                    The result is 2 quarter-sized textures: one for the filled near field and one for the filled far field.
     * 
     * \param coc_4               The downsampled coc texture generated in the second pass.
     * \param coc_near_blurred_4  The downsampled blurred near coc texture generated in the third pass.
     * \param near_4              The downsampled near field texture generated in the fourth pass.
     * \param far_4               The downsampled far field texture generated in the fourth pass.
     */
    void fill(
        const std::shared_ptr<glowl::Texture2D>& coc_4,
        const std::shared_ptr<glowl::Texture2D>& coc_near_blurred_4,
        const std::shared_ptr<glowl::Texture2D>& near_4,
        const std::shared_ptr<glowl::Texture2D>& far_4
    );

    
    /**
     * \brief Sixth and final pass. Merges the results from the previous passes to a final image.
     *                              The result is a single full resolution output texture
     *
     * \param color       The original input/color texture.
     * \param coc         CoC texture from the first pass.
     * \param near_4      Downsampled near field texture from the fourth pass.
     * \param far_4       Downsampled far field texture from the fourth pass.
     * \param near_fill_4 Downsampled filled near field texture generated in the fifth pass.
     * \param far_fill_4  Downsampled filled far field texture generated in the fifth pass.
     */
    void composite(
        const std::shared_ptr<glowl::Texture2D>& color,
        const std::shared_ptr<glowl::Texture2D>& coc,
        const std::shared_ptr<glowl::Texture2D>& near_4,
        const std::shared_ptr<glowl::Texture2D>& far_4,
        const std::shared_ptr<glowl::Texture2D>& near_fill_4,
        const std::shared_ptr<glowl::Texture2D>& far_fill_4
    );

    /**
    * \brief Clears all used textures to black to ensure correctness of the textures.
    */
    void clearAllTextures();

    /**
    * \brief Reloads all textures if input size changes. TODO: also if format of texture changes?
    */
    void reloadAllTextures();

   /**
   * \brief Retrieves the texture layout, resizes it and then reloads the texture.
   *
   * \param tex The texture to resize
   * \param width The new width
   * \param height The new height
   */
    void resizeTexture(const std::shared_ptr<glowl::Texture2D>& tex, int width, int height);

    /**
    * Returns a resolution dimension if c is in [0, 3]:
    * 0 - full width
    * 1 - full height
    * 2 - half width
    * 3 - half height
    * 
    * \param c Determines which width or height to return
    *
    * \return Returns the requested dimension, or -1 if c is outside of [0, 3].
    */
    inline int getRes(int c) {
        if (c < 0 || c > 3)
            return -1;
        return res_[c];
    }
    inline int getWidth() { return res_[0]; }
    inline int getHeight() { return res_[1]; }
    inline int getHalfWidth() { return res_[2]; }
    inline int getHalfHeight() { return res_[3]; }

    // profiling
#ifdef MEGAMOL_USE_PROFILING
    frontend_resources::PerformanceManager::handle_vector timers_;
    frontend_resources::PerformanceManager* perf_manager_ = nullptr;
#endif

    uint32_t version_;

    mmstd_gl::special::TextureInspector tex_inspector_;

    /** Shader programs for DepthOfField */
    std::unique_ptr<glowl::GLSLProgram> coc_generation_prgm_;
    std::unique_ptr<glowl::GLSLProgram> downsample_prgm_;
    std::unique_ptr<glowl::GLSLProgram> near_coc_blur_prgm_;
    std::unique_ptr<glowl::GLSLProgram> computation_prgm_;
    std::unique_ptr<glowl::GLSLProgram> fill_prgm_;
    std::unique_ptr<glowl::GLSLProgram> composite_prgm_;

    /** dof textures for gather-based bokeh depth of field */
    //std::shared_ptr<glowl::Texture2D> color_tx2D_;
    //std::shared_ptr<glowl::Texture2D> depth_tx2D_;
    std::shared_ptr<glowl::Texture2D> coc_tx2D_;
    std::shared_ptr<glowl::Texture2D> color_4_tx2D_;
    std::shared_ptr<glowl::Texture2D> color_mul_coc_far_4_tx2D_;
    std::shared_ptr<glowl::Texture2D> coc_4_tx2D_;
    std::shared_ptr<glowl::Texture2D> coc_near_blurred_4_tx2D_;
    std::shared_ptr<glowl::Texture2D> near_4_tx2D_;
    std::shared_ptr<glowl::Texture2D> far_4_tx2D_;
    std::shared_ptr<glowl::Texture2D> near_fill_4_tx2D_;
    std::shared_ptr<glowl::Texture2D> far_fill_4_tx2D_;
    std::shared_ptr<glowl::Texture2D> output_tx2D_;

    // ivec4(full_width, full_height, half_width, half_height)
    glm::ivec4 res_;

    /** Slot for requesting the output textures from this module, i.e. lhs connection */
    megamol::core::CalleeSlot output_tex_slot_;

    /** Slot for optionally querying an input texture, i.e. a rhs connection */
    megamol::core::CallerSlot input_tex_slot_;

    /** Slot for optionally querying a depth texture, i.e. a rhs connection */
    megamol::core::CallerSlot depth_tex_slot_;

    CompositingOutHandler out_format_handler_;

    bool settings_have_changed_;
};

} // namespace megamol::compositing_gl
