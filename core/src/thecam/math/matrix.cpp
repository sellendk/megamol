/*
 * thecam/math/matrix.cpp
 *
 * Copyright (C) 2016 TheLib Team (http://www.thelib.org/license)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of TheLib, TheLib Team, nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THELIB TEAM AS IS AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THELIB TEAM BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "mmcore/thecam/math/matrix.h"


#ifdef WITH_THE_XMATH
/*
 * megamol::core::thecam::math::invert
 */
bool megamol::core::thecam::math::invert(matrix<DirectX::XMFLOAT4X4>& matrix) {
    auto m = load_xmmatrix(matrix);
    auto d = DirectX::XMMatrixDeterminant(m);
    if (DirectX::XMVectorGetX(d) == 0.0f) {
        return false;
    } else {
        auto r = DirectX::XMMatrixInverse(&d, m);
        store_xmmatrix(matrix, r);
        return true;
    }
}
#endif /* WITH_THE_XMATH */

#ifdef WITH_THE_GLM
/*
 * thecam::math::invert
 */
bool megamol::core::thecam::math::invert(matrix<glm::mat4>& matrix) {
    auto det = glm::determinant(static_cast<glm::mat4>(matrix));
    if (det == 0.0f) {
        return false;
    } else {
        matrix = glm::inverse(static_cast<glm::mat4>(matrix));
        return true;
    }
}
#endif /* WITH_THE_GLM */