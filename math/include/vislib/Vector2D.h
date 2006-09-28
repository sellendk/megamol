/*
 * Vector2D.h  14.09.2006 (mueller)
 *
 * Copyright (C) 2006 by Universitaet Stuttgart (VIS). Alle Rechte vorbehalten.
 */

#ifndef VISLIB_VECTOR2D_H_INCLUDED
#define VISLIB_VECTOR2D_H_INCLUDED
#if (_MSC_VER > 1000)
#pragma once
#endif /* (_MSC_VER > 1000) */


#include "vislib/AbstractVector2D.h"
#include "vislib/Vector.h"


namespace vislib {
namespace math {

    /**
     * Specialisation for a two-dimensional vector. See Vector for additional
     * remarks.
     */
    template<class T> class Vector2D : public AbstractVector2D<T, T[2]> {

    public:

        /**
         * Create a null vector.
         */
        inline Vector2D(void) {
            this->components[0] = this->components[1] = static_cast<T>(0);
        }

        /**
         * Create a new vector initialised with 'components'. 'components' must
         * not be a NULL pointer. 
         *
         * @param components The initial vector components.
         */
        explicit inline Vector2D(const T *components) {
            ASSERT(components != NULL);
            ::memcpy(this->components, components, 2 * sizeof(T));
        }

        /**
         * Clone 'rhs'.
         *
         * @param rhs The object to be cloned.
         */
        inline Vector2D(const Vector2D& rhs) {
            ::memcpy(this->components, rhs.components, 2 * sizeof(T));
        }

        /**
         * Create a copy of 'vector'. This ctor allows for arbitrary vector to
         * vector conversions.
         *
         * @param rhs The vector to be cloned.
         */
        template<class Tp, unsigned int Dp, class Sp>
        inline Vector2D(const AbstractVector<Tp, Dp, Sp>& rhs) {
            this->components[0] = (Dp < 1) ? static_cast<T>(0) 
                                           : static_cast<T>(rhs[0]);
            this->components[1] = (Dp < 2) ? static_cast<T>(0) 
                                           : static_cast<T>(rhs[1]);
        }

        /**
         * Create a new vector.
         *
         * @param x The x-component of the new vector.
         * @param y The y-component of the new vector.
         */
        inline Vector2D(const T& x, const T& y) {
            this->components[0] = x;
            this->components[1] = y;
        }

        /** Dtor. */
        ~Vector2D(void);

        /**
         * Assignment.
         *
         * @param rhs The right hand side operand.
         *
         * @return *this
         */
        inline Vector2D& operator =(const Vector2D& rhs) {
            Super::operator =(rhs);
            return *this;
        }

        /**
         * Assigment for arbitrary vectors. A valid static_cast between T and Tp
         * is a precondition for instantiating this template.
         *
         * This operation does <b>not</b> create aliases. 
         *
         * If the two operands have different dimensions, the behaviour is as 
         * follows: If the left hand side operand has lower dimension, the 
         * highest (Dp - D) dimensions are discarded. If the left hand side
         * operand has higher dimension, the missing dimensions are filled with 
         * zero components.
         *
         * @param rhs The right hand side operand.
         *
         * @return *this
         */
        template<class Tp, unsigned int Dp, class Sp>
        inline Vector2D& operator =(const AbstractVector<Tp, Dp, Sp>& rhs) {
            Super::operator =(rhs);
            return *this;
        }

    protected:

        /** A typedef for the super class. */
        typedef AbstractVector2D<T, T[2]> Super;

    };


    /*
     * vislib::math::Vector2D<T>::~Vector2D
     */
    template<class T> Vector2D<T>::~Vector2D(void) {
    }

} /* end namespace math */
} /* end namespace vislib */

#endif /* VISLIB_VECTOR2D_H_INCLUDED */
