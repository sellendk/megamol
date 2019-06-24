/*
 * Constest2019DataLoader.h
 *
 * Copyright (C) 2019 by VISUS (Universitaet Stuttgart)
 * Alle Rechte vorbehalten.
 */

#ifndef MEGAMOLCORE_CONTEST2019DATALOADER_H_INCLUDED
#define MEGAMOLCORE_CONTEST2019DATALOADER_H_INCLUDED
#if (defined(_MSC_VER) && (_MSC_VER > 1000))
#    pragma once
#endif /* (defined(_MSC_VER) && (_MSC_VER > 1000)) */

#include "astro/AstroDataCall.h"
#include "mmcore/CalleeSlot.h"
#include "mmcore/param/ParamSlot.h"
#include "mmcore/view/AnimDataModule.h"
#include "vislib/math/Cuboid.h"

namespace megamol {
namespace astro {

class Contest2019DataLoader : public core::view::AnimDataModule {
public:
    /**
     * Answer the name of this module.
     *
     * @return The name of this module.
     */
    static const char* ClassName(void) { return "Contest2019DataLoader"; }

    /**
     * Answer a human readable description of this module.
     *
     * @return A human readable description of this module.
     */
    static const char* Description(void) { return "Data source module for the data of the SciVis Contest 2019."; }

    /**
     * Answers whether this module is available on the current system.
     *
     * @return 'true' if the module is available, 'false' otherwise.
     */
    static bool IsAvailable(void) { return true; }

    /** Ctor. */
    Contest2019DataLoader(void);

    /** Dtor. */
    virtual ~Contest2019DataLoader(void);

protected:
    /**
     * Constructs a new frame
     */
    virtual core::view::AnimDataModule::Frame* constructFrame(void) const;

    /**
     * Function that is called once upon initialization.
     *
     * @return True on success, false otherwise
     */
    virtual bool create(void);

    /**
     * Loads the data of a single frame from disk
     *
     * @param frame Pointer to the frame that will contain the data
     * @param idx The index of the frame
     */
    virtual void loadFrame(core::view::AnimDataModule::Frame* frame, unsigned int idx);

    /**
     * Function that is called once upon destruction
     */
    virtual void release(void);

    /**
     * Frame description
     */
    class Frame : public core::view::AnimDataModule::Frame {
    public:
        /** Frame Copy Ctor. */
        Frame(core::view::AnimDataModule& owner);

        /** Frame Dtor. */
        virtual ~Frame(void);

        /**
         * Clears the frame data by deleting all contained pointers
         */
        inline void Clear(void) {
            this->positions.reset();
            this->velocities.reset();
            this->temperatures.reset();
            this->masses.reset();
            this->internalEnergies.reset();
            this->smoothingLengths.reset();
            this->molecularWeights.reset();
            this->densities.reset();
            this->gravitationalPotentials.reset();
            this->isBaryonFlags.reset();
            this->isStarFlags.reset();
            this->isWindFlags.reset();
            this->isStarFormingGasFlags.reset();
            this->isAGNFlags.reset();
            this->particleIDs.reset();
        }

        /**
         * Loads a frame from a given file into this object
         *
         * @param filepath The path to the file to load from. As all frames are in seperate files, no open stream is
         * necessary.
         * @param frameIdx The zero-based index of the loaded frame.
         * @param redshift The redshift value for the frame
         *
         * @return True on success, false otherwise.
         */
        bool LoadFrame(std::string filepath, unsigned int frameIdx, float redshift = 0.0f);

        /**
         * Sets the data pointers of a given call to the internally stored values
         *
         * @param call The call that will be filled with the new data
         * @param boundingBox The bounding box of the data set
         * @param clipBox The clip box of the data set
         */
        void SetData(AstroDataCall& call, const vislib::math::Cuboid<float>& boundingBox,
            const vislib::math::Cuboid<float>& clipBox);

    private:
#pragma pack(push, 1)
        /**
         * Struct representing one particle in the file stored on disk
         */
        struct SavedData {
            float x;
            float vx;
            float y;
            float vy;
            float z;
            float vz;
            float mass;
            float internalEnergy;
            float smoothingLength;
            float molecularWeight;
            float density;
            float gravitationalPotential;
            int64_t particleID;
            uint16_t bitmask;
        };
#pragma pack(pop)

        /** Pointer to the position array */
        vec3ArrayPtr positions = nullptr;

        /** Pointer to the velocity array */
        vec3ArrayPtr velocities = nullptr;

        /** Pointer to the temperature array */
        floatArrayPtr temperatures = nullptr;

        /** Pointer to the mass array */
        floatArrayPtr masses = nullptr;

        /** Pointer to the interal energy array */
        floatArrayPtr internalEnergies = nullptr;

        /** Pointer to the smoothing length array */
        floatArrayPtr smoothingLengths = nullptr;

        /** Pointer to the molecular weight array */
        floatArrayPtr molecularWeights = nullptr;

        /** Pointer to the density array */
        floatArrayPtr densities = nullptr;

        /** Pointer to the gravitational potential array */
        floatArrayPtr gravitationalPotentials = nullptr;

        /** Pointer to the baryon flag array */
        boolArrayPtr isBaryonFlags = nullptr;

        /** Pointer to the star flag array */
        boolArrayPtr isStarFlags = nullptr;

        /** Pointer to the wind flag array */
        boolArrayPtr isWindFlags = nullptr;

        /** Pointer to the star forming gas flag array */
        boolArrayPtr isStarFormingGasFlags = nullptr;

        /** Pointer to the AGN flag array */
        boolArrayPtr isAGNFlags = nullptr;

        /** Pointer to the particle ID array */
        idArrayPtr particleIDs = nullptr;

        /** The redshift value of this frame */
        float redshift;
    };

    /**
     * Unlocker for the frame data
     */
    class Unlocker : public AstroDataCall::Unlocker {
    public:
        /** Copy Ctor. */
        Unlocker(Frame& frame) : AstroDataCall::Unlocker(), frame(&frame) {
            // intentionally empty
        }

        /** Dtor. */
        virtual ~Unlocker(void) {
            this->Unlock();
            ASSERT(this->frame == nullptr);
        }

        /** Overload of the unlock method */
        virtual void Unlock(void) {
            if (this->frame != nullptr) {
                this->frame->Unlock();
                this->frame = nullptr; // DO NOT DELETE!
            }
        }

    private:
        /** Pointer to the contained frame */
        Frame* frame;
    };
#
    /**
     * Function to retrieve the stored data
     *
     * @param caller The calling call
     * @return True on success, false otherwise
     */
    bool getDataCallback(core::Call& caller);

    /**
     * Function to retrieve the stored data set extents
     *
     * @param caller The calling call
     * @return True on success, false otherwise
     */
    bool getExtentCallback(core::Call& caller);

    /**
     * Callback function that is called when the filename or the count of filenames to read is changed
     *
     * @param slot The calling slot
     * @return True on success, false otherwise
     */
    bool filenameChangedCallback(core::param::ParamSlot& slot);

    /** Slot containing the name of the first loaded file */
    core::param::ParamSlot firstFilename;

    /** Slot containing the number of files that should be loaded< */
    core::param::ParamSlot filesToLoad;

    /** Slot to send the data over */
    core::CalleeSlot getDataSlot;

    /** The bounding box of the data */
    vislib::math::Cuboid<float> boundingBox;

    /** The clip box of the data */
    vislib::math::Cuboid<float> clipBox;

    /** Hash that changes with changing data */
    size_t data_hash;

    /** Vector containing the paths to all loadable files */
    std::vector<std::string> filenames;

    /** Vector containing the redshift value for all loadable files */
    std::vector<float> redshiftsForFilename;
};

} // namespace astro
} // namespace megamol

#endif /* MEGAMOLCORE_CONTEST2019DATALOADER_H_INCLUDED */
