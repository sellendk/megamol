/*
 * Color.h
 *
 * Copyright (C) 2008 by Universitaet Stuttgart (VIS).
 * Alle Rechte vorbehalten.
 */

#ifndef MMPROTEINPLUGIN_COLOR_H_INCLUDED
#define MMPROTEINPLUGIN_COLOR_H_INCLUDED
#if (defined(_MSC_VER) && (_MSC_VER > 1000))
#pragma once
#endif /* (defined(_MSC_VER) && (_MSC_VER > 1000)) */

#include "MolecularDataCall.h"
#include "CallProteinData.h"

namespace megamol {
namespace protein {

    class Color {

    public:

        /** The names of the coloring modes */
        enum ColoringMode {
            ELEMENT     = 0,
            RESIDUE     = 1,
            STRUCTURE   = 2,
            BFACTOR     = 3,
            CHARGE      = 4,
            OCCUPANCY   = 5,
            CHAIN       = 6,
            MOLECULE    = 7,
            RAINBOW     = 8,
            CHAINBOW    = 9,
            AMINOACID   = 10,
            VALUE       = 11,
            CHAIN_ID    = 12
        };

        /**
         * fill amino acid color table
         *
         * @param aminoAcidColorTable The amino acid color table.
         */
        static void FillAminoAcidColorTable(
            vislib::Array<vislib::math::Vector<unsigned char, 3> >
              &aminoAcidColorTable);

        /**
         * fill amino acid color table
         *
         * @param aminoAcidColorTable The amino acid color table.
         */
        static void FillAminoAcidColorTable(
            vislib::Array<vislib::math::Vector<float, 3> >
              &aminoAcidColorTable);

        /**
         * Make color table for all atoms acoording to the current coloring
         * mode.
         * The color table is only computed if it is empty or if the
         * recomputation is forced by parameter.
         *
         * @param mol                 The data interface.
         * @param minGradColor        The minimum value for gradient coloring.
         * @param midGradColor        The middle value for gradient coloring.
         * @param maxGradColor        The maximum value for gradient coloring.
         * @param currentColoringMode The current coloring mode.
         * @param atomColorTable      The atom color table.
         * @param colorLookupTable    The color lookup table.
         * @param rainbowColors       The rainbow color lookup table.
         * @param forceRecompute      Force recomputation of the color table.
         */
        static void MakeColorTable(const MolecularDataCall *mol,
            vislib::TString minGradColor,
            vislib::TString midGradColor,
            vislib::TString maxGradColor,
            ColoringMode currentColoringMode,
            vislib::Array<vislib::math::Vector<float, 3> > &atomColorTable,
            vislib::Array<vislib::math::Vector<float, 3> > &colorLookupTable,
            vislib::Array<vislib::math::Vector<float, 3> > &rainbowColors,
            bool forceRecompute = false);

        /**
         * Make color table for all atoms acoording to the current coloring
         * mode.
         * The color table is only computed if it is empty or if the
         * recomputation is forced by parameter.
         *
         * @param prot                The data interface.
         * @param currentColoringMode The current coloring mode.
         * @param protAtomColorTable  The atom color table.
         * @param aminoAcidColorTable The amino acid color table.
         * @param rainbowColors       The rainbow color lookup table.
         * @param forceRecompute      Force recomputation of the color table.
         */
        static void MakeColorTable( const CallProteinData *prot,
            ColoringMode currentColoringMode,
            vislib::Array<unsigned char> &protAtomColorTable,
            vislib::Array<vislib::math::Vector<unsigned char, 3> >
              &aminoAcidColorTable,
            std::vector<vislib::math::Vector<float,3> > &rainbowColors,
            bool forceRecompute = false);

        /**
         * Make color table for all atoms acoording to the current coloring
         * mode.
         * The color table is only computed if it is empty or if the
         * recomputation is forced by parameter
         */
        static void MakeColorTable( const CallProteinData *prot,
            ColoringMode currentColoringMode,
            vislib::math::Vector<float, 3> minValueColor,
            vislib::math::Vector<float, 3> meanValueColor,
            vislib::math::Vector<float, 3> maxValueColor,
            std::vector<vislib::math::Vector<float, 3> > &atomColor,
            vislib::Array<vislib::math::Vector<float, 3> > &aminoAcidColorTable,
            std::vector<vislib::math::Vector<float,3> > &rainbowColors,
            bool forceRecompute = true);

         /**
         * Creates a rainbow color table with 'num' entries.
         *
         * @param num            The number of color entries.
         * @param rainbowColors  The rainbow color lookup table.
         */
        static void MakeRainbowColorTable( unsigned int num,
            vislib::Array<vislib::math::Vector<float, 3> > &rainbowColors);

        /**
         * Creates a rainbow color table with 'num' entries.
         *
         * @param num            The number of color entries.
         * @param rainbowColors  The rainbow color lookup table.
         */
        static void MakeRainbowColorTable( unsigned int num,
            std::vector<vislib::math::Vector<float,3> > &rainbowColors);

        /**
         * Read color table from file.
         *
         * @param filename          The filename of the color table file.
         * @param colorLookupTable  The color lookup table.
         */
        static void ReadColorTableFromFile( vislib::StringA filename,
            vislib::Array<vislib::math::Vector<float, 3> > &colorLookupTable);

    };

} /* end namespace protein */
} /* end namespace megaMol */

#endif /* MMPROTEINPLUGIN_COLOR_H_INCLUDED */
