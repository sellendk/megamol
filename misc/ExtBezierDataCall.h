/*
 * ExtBezierDataCall.h
 *
 * Copyright (C) 2010 by VISUS (Universitaet Stuttgart)
 * Alle Rechte vorbehalten.
 */

#ifndef MEGAMOLCORE_EXTBEZIERDATACALL_H_INCLUDED
#define MEGAMOLCORE_EXTBEZIERDATACALL_H_INCLUDED
#if (defined(_MSC_VER) && (_MSC_VER > 1000))
#pragma once
#endif /* (defined(_MSC_VER) && (_MSC_VER > 1000)) */

#include "api/MegaMolCore.std.h"
#include "AbstractGetData3DCall.h"
#include "CallAutoDescription.h"
#include "vislib/BezierCurve.h"
#include "vislib/ColourRGBAu8.h"
#include "vislib/forceinline.h"
#include "vislib/mathfunctions.h"
#include "vislib/Point.h"
#include "vislib/Quaternion.h"
#include "vislib/Vector.h"


namespace megamol {
namespace core {
namespace misc {


    /**
     * Call for extended b�zier curve data.
     * This call transports an flat array of cubic b�zier curves.
     */
    class MEGAMOLCORE_API ExtBezierDataCall : public AbstractGetData3DCall {
    public:

        /**
         * Class for control points of the bezier curve
         */
        class MEGAMOLCORE_API Point {
        public:

            /**
             * Ctor.
             */
            Point(void) : pos(0.0f, 0.0f, 0.0f), ori(0.0f, 0.0f, 0.0f, 1.0f),
                    radY(0.1f), radZ(0.1f), col(192, 192, 192, 255) {
                // intentionally empty
            }

            /**
             * Ctor.
             *
             * @param x The x coordinate of position
             * @param y The y coordinate of position
             * @param z The z coordinate of position
             * @param qx The x coordinate of quaternion
             * @param qy The y coordinate of quaternion
             * @param qz The z coordinate of quaternion
             * @param qw The w coordinate of quaternion
             * @param ry The radius in y direction
             * @param rz The radius in z direction
             * @param c The colour code
             */
            Point(float x, float y, float z, float qx, float qy, float qz,
                    float qw, float ry, float rz, unsigned char cr,
                    unsigned char cg, unsigned char cb) : pos(x, y, z),
                    ori(qx, qy, qz, qw), radY(ry), radZ(rz),
                    col(cr, cg, cb, 255) {
                // intentionally empty
            }

            /**
             * Ctor.
             *
             * @param pos The position
             * @param ori The orientation quaternion
             * @param ry The radius in y direction
             * @param rz The radius in z direction
             * @param c The colour
             */
            Point(const vislib::math::Point<float, 3> &pos,
                    const vislib::math::Quaternion<float> &ori,
                    float ry, float rz,
                    const vislib::graphics::ColourRGBAu8& c) : pos(pos),
                    ori(ori), radY(ry), radZ(rz), col(c) {
                // intentionally empty
            }

            /**
             * Copy ctor.
             *
             * @param src The object to clone from
             */
            Point(const Point& src) : pos(src.pos), ori(src.ori),
                    radY(src.radY), radZ(src.radZ), col(src.col) {
                // intentionally empty
            }

            /**
             * Dtor.
             */
            ~Point(void) {
                // intentionally empty
            }

            inline const vislib::math::Point<float, 3>& GetPosition(void) const {
                return this->pos;
            }

            inline const vislib::math::Quaternion<float>& GetOrientation(void) const {
                return this->ori;
            }

            inline float GetRadiusY(void) const {
                return this->radY;
            }

            inline float GetRadiusZ(void) const {
                return this->radZ;
            }

            inline const vislib::graphics::ColourRGBAu8& GetColour(void) const {
                return this->col;
            }

            /**
             * Interpolates between 'this' and 'rhs' linearly based on
             * '0 <= t <= 1'.
             *
             * @param rhs The second point to interpolate to (t=1)
             * @param t The interpolation value (0..1)
             *
             * @return The interpolation result
             */
            Point Interpolate(const Point& rhs, float t) const {
                Point rv;

                rv.pos = this->pos.Interpolate(rhs.pos, t);
                //rv.ori = this->ori.Interpolate(rhs.ori, t); // TODO: Implement me
                rv.radY = this->radY * (1.0f - t) + rhs.radY * t;
                rv.radZ = this->radZ * (1.0f - t) + rhs.radZ * t;
                //rv.col = this->col.Interpolate(rhs.col, t); // TODO: Implement me

                return rv;
            }

            inline void Set(float x, float y, float z, float qx, float qy,
                    float qz, float qw, float ry, float rz, unsigned char cr,
                    unsigned char cg, unsigned char cb) {
                this->pos.Set(x, y, z);
                this->ori.Set(qx, qy, qz, qw);
                this->radY = ry;
                this->radZ = rz;
                this->col.Set(cr, cg, cb, 255);
            }

            inline void Set(const vislib::math::Point<float, 3> &pos,
                    const vislib::math::Quaternion<float> &ori,
                    float ry, float rz,
                    const vislib::graphics::ColourRGBAu8& c) {
                this->pos = pos;
                this->ori = ori;
                this->radY = ry;
                this->radZ = rz;
                this->col = c;
            }

            inline void SetPosition(vislib::math::Point<float, 3>& pos) {
                this->pos = pos;
            }

            inline void SetOrientation(vislib::math::Quaternion<float>& ori) {
                this->ori = ori;
            }

            inline void SetRadiusY(float ry) {
                this->radY = ry;
            }

            inline void SetRadiusZ(float rz) {
                this->radZ = rz;
            }

            inline void SetColour(const vislib::graphics::ColourRGBAu8& col) {
                this->col = col;
            }

            /**
             * Test for equality
             *
             * @param rhs The right hand side operand
             *
             * @return 'true' if 'this' and 'rhs' are equal, 'false' if not
             */
            bool operator==(const Point& rhs) const {
                return (this->pos == rhs.pos)
                    && (this->ori == rhs.ori)
                    && (this->radY == rhs.radY)
                    && (this->radZ == rhs.radZ)
                    && (this->col == rhs.col);
            }

            /**
             * Test for inequality
             *
             * @param rhs The right hand side operand
             *
             * @return 'true' if 'this' and 'rhs' are not equal
             */
            bool operator!=(const Point& rhs) const {
                return (this->pos != rhs.pos)
                    || (this->ori != rhs.ori)
                    || (this->radY != rhs.radY)
                    || (this->radZ != rhs.radZ)
                    || (this->col != rhs.col);
            }

            /**
             * Assignment operator
             *
             * @param rhs The right hand side operand
             *
             * @return Reference to 'this'
             */
            Point& operator=(const Point& rhs) {
                this->pos = rhs.pos;
                this->ori = rhs.ori;
                this->radY = rhs.radY;
                this->radZ = rhs.radZ;
                this->col = rhs.col;
                return *this;
            }

            /**
             * Calculates the difference vector of the positions of 'this' and
             * 'rhs'.
             *
             * @param rhs The right hand side operand
             *
             * @return The difference vector of the positions
             */
            vislib::math::Vector<float, 3>
            operator-(const Point& rhs) const {
                return this->pos - rhs.pos;
            }

        private:

#ifdef _WIN32
#pragma warning (disable: 4251)
#endif /* _WIN32 */
            /** The position */
            vislib::math::Point<float, 3> pos;

            /** The orientation */
            vislib::math::Quaternion<float> ori;

            /** The radii */
            float radY, radZ;

            /** The colour code */
            vislib::graphics::ColourRGBAu8 col;
#ifdef _WIN32
#pragma warning (default: 4251)
#endif /* _WIN32 */

        };

        /**
         * Answer the name of the objects of this description.
         *
         * @return The name of the objects of this description.
         */
        static const char *ClassName(void) {
            return "ExtBezierDataCall";
        }

        /**
         * Gets a human readable description of the module.
         *
         * @return A human readable description of the module.
         */
        static const char *Description(void) {
            return "Call to get bezier data";
        }

        /**
         * Answer the number of functions used for this call.
         *
         * @return The number of functions used for this call.
         */
        static unsigned int FunctionCount(void) {
            return AbstractGetData3DCall::FunctionCount();
        }

        /**
         * Answer the name of the function used for this call.
         *
         * @param idx The index of the function to return it's name.
         *
         * @return The name of the requested function.
         */
        static const char * FunctionName(unsigned int idx) {
            return AbstractGetData3DCall::FunctionName(idx);
        }

        /** Ctor. */
        ExtBezierDataCall(void);

        /** Dtor. */
        virtual ~ExtBezierDataCall(void);

        /**
         * Answer the number of b�zier curves.
         *
         * @return The number of b�zier curves
         */
        VISLIB_FORCEINLINE unsigned int Count(void) const {
            return this->count;
        }

        /**
         * Answer the b�zier curves. Might be NULL! Do not delete the returned
         * memory.
         *
         * @return The b�zier curves
         */
        VISLIB_FORCEINLINE const vislib::math::BezierCurve<Point, 3> *
        Curves(void) const {
            return this->curves;
        }

        /**
         * Sets the data. The object will not take ownership of the memory
         * 'curves' points to. The caller is responsible for keeping the data
         * valid as long as it is used.
         *
         * @param count The number of b�zier curves stored in 'curves'
         * @param curves Pointer to a flat array of b�zier curves.
         */
        void SetData(unsigned int count,
                const vislib::math::BezierCurve<Point, 3> *curves);

        /**
         * Assignment operator.
         * Makes a deep copy of all members. While for data these are only
         * pointers, the pointer to the unlocker object is also copied.
         *
         * @param rhs The right hand side operand
         *
         * @return A reference to this
         */
        ExtBezierDataCall& operator=(const ExtBezierDataCall& rhs);

    private:

        /** Number of curves */
        unsigned int count;

        /** Cubic b�zier curves */
        const vislib::math::BezierCurve<Point, 3> *curves;

    };

    /** Description class typedef */
    typedef CallAutoDescription<ExtBezierDataCall> ExtBezierDataCallDescription;


} /* end namespace misc */
} /* end namespace core */
} /* end namespace megamol */

#endif /* MEGAMOLCORE_EXTBEZIERDATACALL_H_INCLUDED */
