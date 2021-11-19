/**
 * MegaMol
 * Copyright (c) 2009-2021, MegaMol Dev Team
 * All rights reserved.
 */

#include "mmcore/utility/plugins/AbstractPluginInstance.h"
#include "mmcore/utility/plugins/PluginRegister.h"

#include "table/TableFlagFilter.h"
#include "table/TableSelectionTx.h"
#include "misc/ParticleDensityOpacityModule.h"
#include "misc/ParticleListMergeModule.h"
#include "misc/ParticleInspector.h"
#include "misc/ParticleWorker.h"
#include "misc/AddClusterColours.h"


namespace megamol::datatools_gl {
class DatatoolsGLPluginInstance : public megamol::core::utility::plugins::AbstractPluginInstance {
    REGISTERPLUGIN(DatatoolsGLPluginInstance)

public:
    DatatoolsGLPluginInstance()
            : megamol::core::utility::plugins::AbstractPluginInstance(
                  "datatools_gl", "MegaMol Standard-Plugin containing data manipulation and conversion modules"){};

    ~DatatoolsGLPluginInstance() override = default;

    // Registers modules and calls
    void registerClasses() override {

        // register modules

        this->module_descriptions.RegisterAutoDescription<megamol::datatools_gl::table::TableFlagFilter>();
        this->module_descriptions.RegisterAutoDescription<megamol::datatools_gl::table::TableSelectionTx>();
        this->module_descriptions.RegisterAutoDescription<megamol::datatools_gl::misc::ParticleDensityOpacityModule>();
        this->module_descriptions.RegisterAutoDescription<megamol::datatools_gl::misc::ParticleInspector>();
        this->module_descriptions.RegisterAutoDescription<megamol::datatools_gl::misc::ParticleListMergeModule>();
        this->module_descriptions.RegisterAutoDescription<megamol::datatools_gl::misc::ParticleWorker>();
        this->module_descriptions.RegisterAutoDescription<megamol::datatools_gl::misc::AddClusterColours>();

    }
};
} // namespace megamol::datatools
