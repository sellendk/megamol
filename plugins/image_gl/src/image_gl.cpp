/**
 * MegaMol
 * Copyright (c) 2009-2021, MegaMol Dev Team
 * All rights reserved.
 */

#include "mmcore/factories/AbstractPluginInstance.h"
#include "mmcore/factories/PluginRegister.h"

#include "ImageLoader.h"
#include "ImageRenderer.h"

namespace megamol::image_gl {
class ImageGlPluginInstance : public megamol::core::factories::AbstractPluginInstance {
    REGISTERPLUGIN(ImageGlPluginInstance)

public:
    ImageGlPluginInstance() : megamol::core::factories::AbstractPluginInstance("image_gl", "The image_gl plugin."){};

    ~ImageGlPluginInstance() override = default;

    // Registers modules and calls
    void registerClasses() override {

        // register modules
        this->module_descriptions.RegisterAutoDescription<megamol::image_gl::ImageRenderer>();
        this->module_descriptions.RegisterAutoDescription<megamol::image_gl::ImageLoader>();

        // register calls
    }
};
} // namespace megamol::image_gl
