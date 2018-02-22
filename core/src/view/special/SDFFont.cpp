/*
 * SDFFont.cpp
 *
 * Copyright (C) 2006 - 2010 by Visualisierungsinstitut Universitaet Stuttgart.
 * Alle Rechte vorbehalten.
 */

#include "mmcore/view/special/SDFFont.h"

#include "mmcore/misc/PngBitmapCodec.h"

#include "vislib/graphics/gl/IncludeAllGL.h"
#include "vislib/graphics/gl/ShaderSource.h"
#include "vislib/math/Vector.h"
#include "vislib/sys/Log.h"
#include "vislib/sys/File.h"
#include "vislib/sys/FastFile.h"
#include "vislib/CharTraits.h"
#include "vislib/memutils.h"
#include "vislib/UTF8Encoder.h"
#include "vislib/math/ShallowMatrix.h"
#include "vislib/math/Matrix.h"
#include "vislib/sys/ASCIIFileBuffer.h"


using namespace vislib;

using namespace megamol;
using namespace megamol::core;
using namespace megamol::core::view;
using namespace megamol::core::view::special;


/* PUBLIC ********************************************************************/


/*
 * SDFFont::SDFFont
 */
SDFFont::SDFFont(const BitmapFont bmf) : AbstractFont(),
    renderType(SDFFont::RENDERTYPE_FILL), texture(), shader(), glyphs(), vbos(), glyphIdx(NULL), maxIdx(0) {

    this->loadFont(bmf);
}


/*
 * SDFFont::SDFFont
 */
SDFFont::SDFFont(const BitmapFont bmf,  SDFFont::RenderType render) : AbstractFont(), 
    font(bmf), renderType(render), texture(), shader(), glyphs(), vbos(), glyphIdx(NULL), maxIdx(0) {

    this->loadFont(bmf);
}


/*
 * SDFFont::SDFFont
 */
SDFFont::SDFFont(const BitmapFont bmf, float size)  : AbstractFont(), 
    font(bmf), renderType(SDFFont::RENDERTYPE_FILL), texture(), shader(), glyphs(), vbos(), glyphIdx(NULL), maxIdx(0) {

    this->SetSize(size);

    this->loadFont(bmf);
}


/*
 * SDFFont::SDFFont
 */
SDFFont::SDFFont(const BitmapFont bmf, bool flipY) : AbstractFont(), 
    font(bmf), renderType(SDFFont::RENDERTYPE_FILL), texture(), shader(), glyphs(), vbos(), glyphIdx(NULL), maxIdx(0) {

    this->SetFlipY(flipY);

    this->loadFont(bmf);
}


/*
 * SDFFont::SDFFont
 */
SDFFont::SDFFont(const BitmapFont bmf, SDFFont::RenderType render, bool flipY) : AbstractFont(),
    font(bmf), renderType(render), texture(), shader(), glyphs(), vbos(), glyphIdx(NULL), maxIdx(0) {

    this->SetFlipY(flipY);

    this->loadFont(bmf);
}


/*
 * SDFFont::SDFFont
 */
SDFFont::SDFFont(const BitmapFont bmf, float size, bool flipY) : AbstractFont(), 
    font(bmf), renderType(SDFFont::RENDERTYPE_FILL), texture(), shader(), glyphs(), vbos(), glyphIdx(NULL), maxIdx(0) {

    this->SetSize(size);
    this->SetFlipY(flipY);

    this->loadFont(bmf);
}


/*
 * SDFFont::SDFFont
 */
SDFFont::SDFFont(const BitmapFont bmf, float size, SDFFont::RenderType render) : AbstractFont(), 
    font(bmf), renderType(render), texture(), shader(), glyphs(), vbos(), glyphIdx(NULL), maxIdx(0) {

    this->SetSize(size);

    this->loadFont(bmf);
}


/*
 * SDFFont::SDFFont
 */
SDFFont::SDFFont(const BitmapFont bmf, float size, SDFFont::RenderType render, bool flipY) : AbstractFont(),
        font(bmf), renderType(render), texture(), shader(), glyphs(), vbos(), glyphIdx(NULL), maxIdx(0) {

    this->SetSize(size);
    this->SetFlipY(flipY);

    this->loadFont(bmf);
}


/*
 * SDFFont::SDFFont
 */
SDFFont::SDFFont(const SDFFont& src) : AbstractFont(),
    font(src.font), renderType(src.renderType), texture(), shader(), glyphs(), vbos(), glyphIdx(NULL), maxIdx(0) {

    this->SetSize(src.GetSize());
    this->SetFlipY(src.IsFlipY());

    this->loadFont(src.font);
}


/*
 * SDFFont::SDFFont
 */
SDFFont::SDFFont(const SDFFont& src, SDFFont::RenderType render) : AbstractFont(), 
    font(src.font), renderType(render), texture(), shader(), glyphs(), vbos(), glyphIdx(NULL), maxIdx(0) {

    this->SetSize(src.GetSize());
    this->SetFlipY(src.IsFlipY());

    this->loadFont(src.font);
}


/*
 * SDFFont::SDFFont
 */
SDFFont::SDFFont(const SDFFont& src, float size) : AbstractFont(),
        font(src.font), renderType(src.renderType), texture(), shader(), glyphs(), vbos(), glyphIdx(NULL), maxIdx(0) {

    this->SetSize(size);
    this->SetFlipY(src.IsFlipY());

    this->loadFont(src.font);
}


/*
 * SDFFont::SDFFont
 */
SDFFont::SDFFont(const SDFFont& src, bool flipY) : AbstractFont(),
        font(src.font), renderType(src.renderType), texture(), shader(), glyphs(), vbos(), glyphIdx(NULL), maxIdx(0) {

    this->SetSize(src.GetSize());
    this->SetFlipY(flipY);

    this->loadFont(src.font);
}


/*
 * SDFFont::SDFFont
 */
SDFFont::SDFFont(const SDFFont& src, SDFFont::RenderType render, bool flipY) : AbstractFont(),
        font(src.font), renderType(render), texture(), shader(), glyphs(), vbos(), glyphIdx(NULL), maxIdx(0) {

    this->SetSize(src.GetSize());
    this->SetFlipY(flipY);

    this->loadFont(src.font);
}


/*
 * SDFFont::SDFFont
 */
SDFFont::SDFFont(const SDFFont& src, float size, bool flipY) : AbstractFont(), 
    font(src.font), renderType(src.renderType), texture(), shader(), glyphs(), vbos(), glyphIdx(NULL), maxIdx(0) {

    this->SetSize(size);
    this->SetFlipY(flipY);

    this->loadFont(src.font);
}


/*
 * SDFFont::SDFFont
 */
SDFFont::SDFFont(const SDFFont& src, float size,  SDFFont::RenderType render) : AbstractFont(), 
    font(src.font),  renderType(render), texture(), shader(), glyphs(), vbos(), glyphIdx(NULL), maxIdx(0) {

    this->SetSize(size);
    this->SetFlipY(src.IsFlipY());

    this->loadFont(src.font);
}


/*
 * SDFFont::SDFFont
 */
SDFFont::SDFFont(const SDFFont& src, float size, SDFFont::RenderType render, bool flipY) : AbstractFont(),
        font(src.font), renderType(render), texture(), shader(), glyphs(), vbos(), glyphIdx(NULL), maxIdx(0) {

    this->SetSize(size);
    this->SetFlipY(flipY);

    this->loadFont(src.font);
}


/*
 * SDFFont::~SDFFont
 */
SDFFont::~SDFFont(void) {

    this->Deinitialise();
}


/*
 * SDFFont::BlockLines
 */
unsigned int SDFFont::BlockLines(float maxWidth, float size, const char *txt) const {

    return this->lineCount(this->buildGlyphRun(txt, maxWidth / size), true);
}


/*
 * SDFFont::BlockLines
 */
unsigned int SDFFont::BlockLines(float maxWidth, float size, const wchar_t *txt) const {

    return this->lineCount(this->buildGlyphRun(txt, maxWidth / size), true);
}


/*
 * SDFFont::DrawString
 */
void SDFFont::DrawString(float x, float y, float size, bool flipY, const char *txt, AbstractFont::Alignment align) const {

    int *run = this->buildGlyphRun(txt, FLT_MAX);

    if ((align == ALIGN_CENTER_MIDDLE) || (align == ALIGN_LEFT_MIDDLE) || (align == ALIGN_RIGHT_MIDDLE)) {
        y += static_cast<float>(this->lineCount(run, false)) * 0.5f * size * (flipY ? 1.0f : -1.0f);

    } else if ((align == ALIGN_CENTER_BOTTOM) || (align == ALIGN_LEFT_BOTTOM) || (align == ALIGN_RIGHT_BOTTOM)) {
        y += static_cast<float>(this->lineCount(run, false)) * size * (flipY ? 1.0f : -1.0f);
    }

    this->draw(run, x, y, 0.0f, size, flipY, align);

    delete[] run;
}


/*
* SDFFont::DrawString
*/
void SDFFont::DrawString(float x, float y, float size, bool flipY, const wchar_t *txt, AbstractFont::Alignment align) const {

    int *run = this->buildGlyphRun(txt, FLT_MAX);

    if ((align == ALIGN_CENTER_MIDDLE) || (align == ALIGN_LEFT_MIDDLE) || (align == ALIGN_RIGHT_MIDDLE)) {
        y += static_cast<float>(this->lineCount(run, false)) * 0.5f * size * (flipY ? 1.0f : -1.0f);

    }
    else if ((align == ALIGN_CENTER_BOTTOM) || (align == ALIGN_LEFT_BOTTOM) || (align == ALIGN_RIGHT_BOTTOM)) {
        y += static_cast<float>(this->lineCount(run, false)) * size * (flipY ? 1.0f : -1.0f);
    }
    
    this->draw(run, x, y, 0.0f, size, flipY, align);

    delete[] run;
}


/*
 * SDFFont::DrawString
 */
void SDFFont::DrawString(float x, float y, float w, float h, float size, bool flipY, const char *txt, AbstractFont::Alignment align) const {

    int *run = this->buildGlyphRun(txt, w / size);

    if (flipY) y += h;

    switch (align) {
    case ALIGN_CENTER_BOTTOM:
        x += w * 0.5f;
        y += (flipY ? -1.0f : 1.0f) * (h - this->lineCount(run, false) * size);
        break;
    case ALIGN_CENTER_MIDDLE:
        x += w * 0.5f;
        y += (flipY ? -1.0f : 1.0f) * (h - this->lineCount(run, false) * size) * 0.5f;
        break;
    case ALIGN_CENTER_TOP:
        x += w * 0.5f;
        break;
    case ALIGN_LEFT_BOTTOM:
        y += (flipY ? -1.0f : 1.0f) * (h - this->lineCount(run, false) * size);
        break;
    case ALIGN_LEFT_MIDDLE:
        y += (flipY ? -1.0f : 1.0f) * (h - this->lineCount(run, false) * size) * 0.5f;
        break;
    case ALIGN_RIGHT_BOTTOM:
        x += w;
        y += (flipY ? -1.0f : 1.0f) * (h - this->lineCount(run, false) * size);
        break;
    case ALIGN_RIGHT_MIDDLE:
        x += w;
        y += (flipY ? -1.0f : 1.0f) * (h - this->lineCount(run, false) * size) * 0.5f;
        break;
    case ALIGN_RIGHT_TOP:
        x += w;
        break;
    default:
        break;
    }

    this->draw(run, x, y, 0.0f, size, flipY, align);

    delete[] run;
}


/*
 * SDFFont::DrawString
 */
void SDFFont::DrawString(float x, float y, float w, float h, float size,  bool flipY, const wchar_t *txt, AbstractFont::Alignment align) const {

    int *run = this->buildGlyphRun(txt, w / size);

    if (flipY) y += h;

    switch (align) {
    case ALIGN_CENTER_BOTTOM:
        x += w * 0.5f;
        y += (flipY ? -1.0f : 1.0f) * (h - this->lineCount(run, false) * size);
        break;
    case ALIGN_CENTER_MIDDLE:
        x += w * 0.5f;
        y += (flipY ? -1.0f : 1.0f) * (h - this->lineCount(run, false) * size) * 0.5f;
        break;
    case ALIGN_CENTER_TOP:
        x += w * 0.5f;
        break;
    case ALIGN_LEFT_BOTTOM:
        y += (flipY ? -1.0f : 1.0f) * (h - this->lineCount(run, false) * size);
        break;
    case ALIGN_LEFT_MIDDLE:
        y += (flipY ? -1.0f : 1.0f) * (h - this->lineCount(run, false) * size) * 0.5f;
        break;
    case ALIGN_RIGHT_BOTTOM:
        x += w;
        y += (flipY ? -1.0f : 1.0f) * (h - this->lineCount(run, false) * size);
        break;
    case ALIGN_RIGHT_MIDDLE:
        x += w;
        y += (flipY ? -1.0f : 1.0f) * (h - this->lineCount(run, false) * size) * 0.5f;
        break;
    case ALIGN_RIGHT_TOP:
        x += w;
        break;
    default:
        break;
    }

    this->draw(run, x, y, 0.0f, size, flipY, align);

    delete[] run;
}


/*
* SDFFont::DrawString
*/
void SDFFont::DrawString(float x, float y, float z, float size, bool flipY, const char * txt, Alignment align) const {

    int *run = this->buildGlyphRun(txt, FLT_MAX);

    if ((align == ALIGN_CENTER_MIDDLE) || (align == ALIGN_LEFT_MIDDLE) || (align == ALIGN_RIGHT_MIDDLE)) {
        y += static_cast<float>(this->lineCount(run, false)) * 0.5f * size * (flipY ? 1.0f : -1.0f);
    }
    else if ((align == ALIGN_CENTER_BOTTOM) || (align == ALIGN_LEFT_BOTTOM) || (align == ALIGN_RIGHT_BOTTOM)) {
        y += static_cast<float>(this->lineCount(run, false)) * size * (flipY ? 1.0f : -1.0f);
    }

    this->draw(run, x, y, 0.0f, size, flipY, align);

    delete[] run;
}


/*
* SDFFont::DrawString
*/
void SDFFont::DrawString(float x, float y, float z, float size, bool flipY, const wchar_t * txt, Alignment align) const {

    int *run = this->buildGlyphRun(txt, FLT_MAX);

    if ((align == ALIGN_CENTER_MIDDLE) || (align == ALIGN_LEFT_MIDDLE) || (align == ALIGN_RIGHT_MIDDLE)) {
        y += static_cast<float>(this->lineCount(run, false)) * 0.5f * size  * (flipY ? 1.0f : -1.0f);
    }
    else if ((align == ALIGN_CENTER_BOTTOM) || (align == ALIGN_LEFT_BOTTOM) || (align == ALIGN_RIGHT_BOTTOM)) {
        y += static_cast<float>(this->lineCount(run, false)) * size * (flipY ? 1.0f : -1.0f);
    }

    this->draw(run, x, y, 0.0f, size, flipY, align);

    delete[] run;
}


/*
* SDFFont::LineWidth
*/
float SDFFont::LineWidth(float size, const char *txt) const {

    int *run = this->buildGlyphRun(txt, FLT_MAX);
    int *i = run;
    float len = 0.0f;
    float comlen = 0.0f;
    while (*i != 0) {
        comlen = this->lineWidth(i, true);
        if (comlen > len) {
            len = comlen;
        }
    }
    delete[] run;
    return len * size;
}


/*
* SDFFont::LineWidth
*/
float SDFFont::LineWidth(float size, const wchar_t *txt) const {

    int *run = this->buildGlyphRun(txt, FLT_MAX);
    int *i = run;
    float len = 0.0f;
    float comlen = 0.0f;
    while (*i != 0) {
        comlen = this->lineWidth(i, true);
        if (comlen > len) {
            len = comlen;
        }
    }
    delete[] run;
    return len * size;
}


/* PRIVATE ********************************************************************/


/*
 * SDFFont::initialise
 */
bool SDFFont::initialise(void) {

    // unused so far ....

    return true;
}


/*
 * SDFFont::deinitialise
 */
void SDFFont::deinitialise(void) {

    // Texture
    this->texture.Release();
    // Shader
    this->shader.Release();
    // VBOs
    for (unsigned int i = 0; i < (unsigned int)this->vbos.size(); i++) {
        glDeleteBuffers(1, &this->vbos[i].handle);
    }
    this->vbos.clear();
    // VAO
    glDeleteVertexArrays(1, &this->vaoHandle);
    // glyphIdx
    if (this->glyphIdx != NULL) {
        for (unsigned int i = 0; i < this->maxIdx; i++) {
            this->glyphIdx[i] = NULL;
        }
    }
}


/*
* SDFFont::lineCount
*/
int SDFFont::lineCount(int *run, bool deleterun) const {
    if ((run == NULL) || (run[0] == 0)) return 0;
    int i = 1;
    for (int j = 0; run[j] != 0; j++) {
        if (run[j] < 0) i++;
    }
    if (deleterun) delete[] run;

return i;
}


/*
* SDFFont::lineWidth
*/
float SDFFont::lineWidth(int *&run, bool iterate) const {

    int *i = run;
    float len = 0.0f;
    while (*i != 0) {
        len += this->glyphIdx[(*i)]->xadvance; // No check -> requires valid run!
        i++;
        if (*i < 0) break;
    }
    if (iterate) run = i;

    return len;
}


/*
* SDFFont::buildGlyphRun
*/
int *SDFFont::buildGlyphRun(const char *txt, float maxWidth) const {

    vislib::StringA txtutf8;
    if (!vislib::UTF8Encoder::Encode(txtutf8, txt)) {
        // encoding failed ... how?
        char *t = txtutf8.AllocateBuffer(vislib::CharTraitsA::SafeStringLength(txt));
        for (; *txt != 0; txt++) {
            if ((*txt & 0x80) == 0) {
                *t = *txt;
                t++;
            }
        }
        *t = 0;
    }

    return this->buildUpGlyphRun(txtutf8, maxWidth);
}


/*
* SDFFont::buildGlyphRun
*/
int *SDFFont::buildGlyphRun(const wchar_t *txt, float maxWidth) const {

    vislib::StringA txtutf8;
    if (!vislib::UTF8Encoder::Encode(txtutf8, txt)) {
        // encoding failed ... how?
        char *t = txtutf8.AllocateBuffer(vislib::CharTraitsW::SafeStringLength(txt));
        for (; *txt != 0; txt++) {
            if ((*txt & 0x80) == 0) {
                *t = static_cast<char>(*txt);
                t++;
            }
        }
        *t = 0;
    }

    return this->buildUpGlyphRun(txtutf8, maxWidth);
}


/*
* SDFFont::buildUpGlyphRun
*/
int *SDFFont::buildUpGlyphRun(const char *txtutf8, float maxWidth) const {

    SIZE_T txtlen = static_cast<SIZE_T>(CharTraitsA::SafeStringLength(txtutf8));
    SIZE_T pos = 0;
    int *glyphrun = new int[txtlen + 1];
    bool knowLastWhite = false;
    bool blackspace = true;
    SIZE_T lastWhiteGlyph = 0;
    SIZE_T lastWhiteSpace = 0;
    float lineLength = 0.0f;
    bool nextAsNewLine = false;
    unsigned int folBytes = 0; // following bytes
    unsigned int idx, tmpIdx;
    ::memset(glyphrun, 0, sizeof(int) * (txtlen + 1));

    // > 0 1+index of the glyph to use
    // < 0 -(1+index) of the glyph and new line
    // = 0 end

    if (sizeof(idx) < 4) {
        vislib::sys::Log::DefaultLog.WriteError("[SDFFont] [buildUpGlyphRun] UNSIGNED INT int must have 4 bytes, but has %i. \n", sizeof(idx));
    }

    // build glyph run
    for (SIZE_T i = 0; i < txtlen; i++) {

        if (txtutf8[i] == '\n') { // special handle new lines
            nextAsNewLine = true;
            continue;
        }

        // --------------------------------------------------------------------
        // UTF8-Bytes to Decimal

        // ! Following variables must be used "unisgned" so that always zeros are shifted and not ones ...
        // !so far: NO CHECK FOR INVALID UTF8 byte sequences ...

        unsigned char byte = txtutf8[i];
        // If byte >= 0 -> ASCII-Byte: 0XXXXXXX = 0...127
        if (byte < 128) { 
            idx = static_cast<unsigned int>(txtutf8[i]);
        }
        // ... if byte >= 128 => UTF8-Byte: 1XXXXXXX 
        else { 
            // Supporting UTF8 for up to 3 bytes:
            if (byte >= (unsigned char)(0b11100000)) { //>224 1110XXXX -> start 3-Byte UTF8, 2 bytes are following
                folBytes = 2;
                idx = (unsigned int)(byte & (unsigned char)(0b00001111)); // consider only last 4 bits
                idx = (idx << 12); // 2*6 Bits are following
                continue;
            }
            else if (byte >= (unsigned char)(0b11000000)) { //>192 110XXXXX -> start 2-Byte UTF8, 1 byte is following
                folBytes = 1;
                idx = (unsigned int)(byte & (unsigned char)(0b00011111)); // consider only last 5 bits
                idx = (idx << 6); // 1*6 Bits are following
                continue;
            }
            else if (byte >= (unsigned char)(0b10000000)) { //> 128 10XXXXXX -> "following" 1-2 bytes
                folBytes--;
                tmpIdx = (unsigned int)(byte & (unsigned char)(0b00111111)); // consider only last 6 bits
                idx    = (idx | (tmpIdx << (folBytes*6))); // shift tmpIdx depending on following byte and 'merge' (|) with idx
                if (folBytes > 0) {
                    continue;
                }
            }
            else {
                vislib::sys::Log::DefaultLog.WriteError("[SDFFont] [buildUpGlyphRun] BUG ...\n");
            }
        }

        if (idx > this->maxIdx) {
            // Glyph not available ....
            //vislib::sys::Log::DefaultLog.WriteError("[SDFFont] [buildUpGlyphRun] Glyph not available ...\n");
            continue;
        }
        if (this->glyphIdx[idx] == NULL) {
            // Glyph not available ....
            //vislib::sys::Log::DefaultLog.WriteError("[SDFFont] [buildUpGlyphRun] Glyph not available ...\n");
            continue;
        }

        // --------------------------------------------------------------------

        // add glyph to run
        if (txtutf8[i] == ' ') { // the only special white-space
            glyphrun[pos++] = static_cast<int>(1 + idx);
            lineLength += this->glyphIdx[idx]->xadvance;
            // no test for soft break here!
            if (!knowLastWhite || blackspace) {
                knowLastWhite  = true;
                blackspace     = false;
                lastWhiteGlyph = pos - 1;
            }
            lastWhiteSpace = i;
        }
        else if (nextAsNewLine) {
            nextAsNewLine   = false;
            glyphrun[pos++] = -static_cast<int>(1 + idx);
            knowLastWhite   = false;
            blackspace      = true;
            lineLength      = this->glyphIdx[idx]->xadvance;
        }
        else {
            blackspace      = true;
            glyphrun[pos++] = static_cast<int>(1 + idx);
            lineLength     += this->glyphIdx[idx]->xadvance;
            // test for soft break
            if (lineLength > maxWidth) {
                // soft break
                if (knowLastWhite) {
                    i             = lastWhiteSpace;
                    pos           = lastWhiteGlyph + 1;
                    lineLength    = 0.0f;
                    knowLastWhite = false;
                    nextAsNewLine = true;
                }
                else {
                    // last word to long
                    glyphrun[pos - 1] = -glyphrun[pos - 1];
                    lineLength        = this->glyphIdx[idx]->xadvance;
                }
            }
        }
    }

    return glyphrun;
}


/*
* SDFFont::draw
*/
void SDFFont::draw(int *run, float x, float y, float z, float size, bool flipY, Alignment align) const {

    // Check texture
    if (!this->texture.IsValid()) {
        vislib::sys::Log::DefaultLog.WriteError("[SDFFont] [draw] Texture is not valid. \n");
        return;
    }
    // Check shader
    if (!this->shader.IsValidHandle(this->shader.ProgramHandle())) {
        vislib::sys::Log::DefaultLog.WriteError("[SDFFont] [draw] Shader handle is not valid. \n");
        return;
    }

    // ------------------------------------------------------------------------
    // Generate data buffers

    // Data buffers
    unsigned cnt = 0;
    int *tmpRun = run;
    while ((*tmpRun) != 0) {
        tmpRun++;
        cnt++;
    }
    GLfloat *posData = new GLfloat[cnt * 12];
    GLfloat *texData = new GLfloat[cnt * 8];

    float gx = x;
    float gy = y;
    float sy = (flipY)?(-size):(size);
    unsigned int charCnt = 0;

    while ((*run) != 0) {

        const SDFGlyphInfo *glyph = this->glyphIdx[((*run) < 0) ? (-1 - (*run)) : ((*run) - 1)];

        if ((*run) < 0) {
            gx = x;

            if ((align == ALIGN_CENTER_BOTTOM) || (align == ALIGN_CENTER_MIDDLE) || (align == ALIGN_CENTER_TOP)) {
                gx -= this->lineWidth(run, false) * size * 0.5f;
            }
            else if ((align == ALIGN_RIGHT_BOTTOM) || (align == ALIGN_RIGHT_MIDDLE) || (align == ALIGN_RIGHT_TOP)) {
                gx -= this->lineWidth(run, false) * size;
            }

            gy += (sy);
        }

        // --------------------------------------------------------------------
        // TODO: Kerning



        // --------------------------------------------------------------------

        // Position
        posData[charCnt * 12 + 0]  = size * (glyph->xoffset)                 + gx; // X0
        posData[charCnt * 12 + 1]  = sy   * (glyph->yoffset)                 + gy; // Y0
        posData[charCnt * 12 + 2]  =                                            z; // Z0
        posData[charCnt * 12 + 3]  = size * (glyph->xoffset + glyph->width)  + gx; // X1
        posData[charCnt * 12 + 4]  = sy   * (glyph->yoffset)                 + gy; // Y1
        posData[charCnt * 12 + 5]  =                                            z; // Z1
        posData[charCnt * 12 + 6]  = size * (glyph->xoffset + glyph->width)  + gx; // X2
        posData[charCnt * 12 + 7]  = sy   * (glyph->yoffset + glyph->height) + gy; // Y2
        posData[charCnt * 12 + 8]  =                                            z; // Z2
        posData[charCnt * 12 + 9]  = size * (glyph->xoffset)                 + gx; // X3
        posData[charCnt * 12 + 10] = sy   * (glyph->yoffset + glyph->height) + gy; // Y3
        posData[charCnt * 12 + 11] =                                            z; // Z3

        // Texture  
        texData[charCnt * 8 + 0] = glyph->texX0; // X0
        texData[charCnt * 8 + 1] = glyph->texY0; // Y0
        texData[charCnt * 8 + 2] = glyph->texX1; // X1
        texData[charCnt * 8 + 3] = glyph->texY0; // Y1
        texData[charCnt * 8 + 4] = glyph->texX1; // X2
        texData[charCnt * 8 + 5] = glyph->texY1; // Y2
        texData[charCnt * 8 + 6] = glyph->texX0; // X3
        texData[charCnt * 8 + 7] = glyph->texY1; // Y3

        // ...
        charCnt++;
        gx += (glyph->xadvance * size);

        run++;
    }

    for (unsigned int i = 0; i < (unsigned int)this->vbos.size(); i++) {
        glBindBuffer(GL_ARRAY_BUFFER, this->vbos[i].handle);
        if (this->vbos[i].index == (GLuint)VBOAttrib::POSITION) {
            glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)charCnt * 12 * sizeof(GLfloat), posData, GL_STATIC_DRAW);
        }
        else if (this->vbos[i].index == (GLuint)VBOAttrib::TEXTURE) {
            glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)charCnt * 8 * sizeof(GLfloat), texData, GL_STATIC_DRAW);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    delete[] posData;
    delete[] texData;

    // ------------------------------------------------------------------------
    // Draw data buffers

    // Get current matrices 
    GLfloat modelViewMatrix_column[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMatrix_column);
    vislib::math::ShallowMatrix<GLfloat, 4, vislib::math::COLUMN_MAJOR> modelViewMatrix(&modelViewMatrix_column[0]);
    GLfloat projMatrix_column[16];
    glGetFloatv(GL_PROJECTION_MATRIX, projMatrix_column);
    vislib::math::ShallowMatrix<GLfloat, 4, vislib::math::COLUMN_MAJOR> projMatrix(&projMatrix_column[0]);
    // Compute modelviewprojection matrix
    vislib::math::Matrix<GLfloat, 4, vislib::math::COLUMN_MAJOR> modelViewProjMatrix = projMatrix * modelViewMatrix;

    // Get current color
    GLfloat color[4];
    glGetFloatv(GL_CURRENT_COLOR, color);

    // Store/Set blending
    GLint blendSrc;
    GLint blendDst;
    glGetIntegerv(GL_BLEND_SRC, &blendSrc);
    glGetIntegerv(GL_BLEND_DST, &blendDst);
    bool blendEnabled = glIsEnabled(GL_BLEND);
    if (!blendEnabled) {
        glEnable(GL_BLEND);
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(this->vaoHandle);

    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->texture.GetId()); // instead of this->texture.Bind() => because draw() is CONST

    glUseProgram(this->shader.ProgramHandle()); // instead of this->shader.Enable() => because draw() is CONST

    // Vertex shader
    glUniformMatrix4fv(this->shader.ParameterLocation("mvpMat"), 1, GL_FALSE, modelViewProjMatrix.PeekComponents());
    glUniform1f(this->shader.ParameterLocation("fontSize"), size);
    // Fragment shader
    glUniform4fv(this->shader.ParameterLocation("color"), 1, color);
    glUniform1i(this->shader.ParameterLocation("fontTex"), 0);

    glDrawArrays(GL_QUADS, 0, (GLsizei)charCnt * 4);

    glUseProgram(0); // instead of this->shader.Disable() => because draw() is CONST
    glBindVertexArray(0);
    glDisable(GL_TEXTURE_2D);

    // Reset blending
    if (!blendEnabled) {
        glDisable(GL_BLEND);
    }
    glBlendFunc(blendSrc, blendDst);

}


/*
* SDFFont::loadFont
*/
bool SDFFont::loadFont(BitmapFont bmf) {

    // Convert BitmapFont to string
    vislib::StringA fontName = "";
    switch (bmf) {
        case  (BitmapFont::EVOLVENTA): fontName = "evolventa"; break;
        case  (BitmapFont::VERDANA): fontName = "verdana"; break;
        default: break;
    }

    // Folder holding font data
    vislib::StringA folder = ".\\fonts\\";

    // (1) Load buffers --------------------------------------------------------
    if (!this->loadFontBuffers()) {
        vislib::sys::Log::DefaultLog.WriteError("[SDFFont] [loadFont] Failed to load buffers. \n");
        return false;
    }
    
    // (2) Load font information -----------------------------------------------
    vislib::StringA infoFile = folder;
    infoFile.Append(fontName);
    infoFile.Append(".fnt");
    if (!this->loadFontInfo(infoFile)) {
        vislib::sys::Log::DefaultLog.WriteError("[SDFFont] [loadFont] Failed to load font info file. \n");
        return false;
    }

    // (3) Load texture --------------------------------------------------------
    vislib::StringA textureFile = folder;
    textureFile.Append(fontName);
    textureFile.Append(".png");
    if (!this->loadFontTexture(textureFile)) {
        vislib::sys::Log::DefaultLog.WriteError("[SDFFont] [loadFont] Failed to loda font texture. \n");
        return false;
    }

    // (4) Load shaders --------------------------------------------------------
    vislib::StringA vertShaderFile = folder;
    vertShaderFile.Append("vertex.shader");
    vislib::StringA fragShaderFile = folder;
    fragShaderFile.Append("fragment.shader");
    if (!this->loadFontShader(vertShaderFile, fragShaderFile)) {
        vislib::sys::Log::DefaultLog.WriteError("[SDFFont] [loadFont] Failed to load font shaders. \n");
        return false;
    }  

    return true;
}


/*
* SDFFont::loadFontBuffers
*/
bool SDFFont::loadFontBuffers() {


    // Reset 
    if (glIsVertexArray(this->vaoHandle)) {
        glDeleteVertexArrays(1, &this->vaoHandle);
    }
    for (unsigned int i = 0; i < (unsigned int)this->vbos.size(); i++) {
        glDeleteBuffers(1, &this->vbos[i].handle);
    }
    this->vbos.clear();

    // Declare data buffers ---------------------------------------------------

    // Init vbos
    SDFVBO newVBO;

    // VBO for position data
    newVBO.name = "inVertPos";
    newVBO.index = (GLuint)VBOAttrib::POSITION;
    newVBO.dim = 3;
    newVBO.handle = 0; // Default init
    this->vbos.push_back(newVBO);


    // VBO for texture data
    newVBO.name = "inVertTexCoord";
    newVBO.index = (GLuint)VBOAttrib::TEXTURE;
    newVBO.dim = 2;
    newVBO.handle = 0; // Default init
    this->vbos.push_back(newVBO);

    // ------------------------------------------------------------------------

    // Create Vertex Array Object 
    glGenVertexArrays(1, &this->vaoHandle);
    glBindVertexArray(this->vaoHandle);

    for (unsigned int i = 0; i < (unsigned int)this->vbos.size(); i++) {
        glGenBuffers(1, &this->vbos[i].handle);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbos[i].handle);
        // Create empty buffer
        glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);
        // Bind buffer to vertex attribute
        glEnableVertexAttribArray(this->vbos[i].index); 
        glVertexAttribPointer(this->vbos[i].index, this->vbos[i].dim, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
    }
   
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    for (unsigned int i = 0; i < (unsigned int)this->vbos.size(); i++) {
        glDisableVertexAttribArray(this->vbos[i].index);
    }

    return true;
}


/*
* SDFFont::loadFontInfo
*
* Bitmap Font file format: http://www.angelcode.com/products/bmfont/doc/file_format.html
*
*/
bool SDFFont::loadFontInfo(vislib::StringA filename) {

    // Reset font info
    this->glyphs.clear();
    this->glyphIdx = NULL;
    this->maxIdx   = 0;

    // Load file
    vislib::sys::ASCIIFileBuffer file;
    if (!file.LoadFile(filename)) {
        vislib::sys::Log::DefaultLog.WriteError("[SDFFont] [loadfontCharacters] Could not load file as ascii buffer: \"%s\". \n", filename.PeekBuffer());
        return false;
    }

    float texWidth    = 0.0f;
    float texHeight   = 0.0f;
    float lineHeight  = 0.0f;
    
    // Read info file line by line
    int idx;
    unsigned int second;
    vislib::StringA line;
    float width;
    float height;
    unsigned int maxId = 0;;

    size_t lineCnt = 0;
    while (lineCnt < file.Count()) {
        line = static_cast<vislib::StringA>(file.Line(lineCnt));
        // (1) Parse common info line
        if (line.StartsWith("common ")) { 

            idx = line.Find("scaleW=", 0);
            texWidth = (float)std::atof(line.Substring(idx + 7, 4));

            idx = line.Find("scaleH=", 0);
            texHeight = (float)std::atof(line.Substring(idx + 7, 4));

            idx = line.Find("lineHeight=", 0);
            lineHeight = (float)std::atof(line.Substring(idx + 11, 4));
        }
        // (2) Parse character info
        else if (line.StartsWith("char ")) { 
            SDFGlyphInfo newChar;

            idx = line.Find("id=", 0);
            newChar.id = (unsigned int)std::atoi(line.Substring(idx + 3, 5)); 

            if (maxId < newChar.id) {
                maxId = newChar.id;
            }

            idx = line.Find("x=", 0);
            newChar.texX0 = (float)std::atof(line.Substring(idx + 2, 4)) / texWidth;

            idx = line.Find("y=", 0);
            newChar.texY0 = (float)std::atof(line.Substring(idx + 2, 4)) / texHeight;

            idx = line.Find("width=", 0);
            width = (float)std::atof(line.Substring(idx + 6, 4));

            idx = line.Find("height=", 0);
            height = (float)std::atof(line.Substring(idx + 7, 4));

            newChar.width  = width / lineHeight;
            newChar.height = height / lineHeight;

            idx = line.Find("xoffset=", 0);
            newChar.xoffset = (float)std::atof(line.Substring(idx + 8, 4)) / lineHeight;

            idx = line.Find("yoffset=", 0);
            newChar.yoffset  = (float)std::atof(line.Substring(idx + 8, 4)) / lineHeight;

            idx = line.Find("xadvance=", 0);
            newChar.xadvance = (float)std::atof(line.Substring(idx + 9, 4)) / lineHeight;

            newChar.kernings.clear();

            newChar.texX1 = newChar.texX0 + width / texWidth;
            newChar.texY1 = newChar.texY0 + height / texHeight;

            this->glyphs.push_back(newChar);
        }
        // (3) Parse kerning info
        else if (line.StartsWith("kerning ")) { 

            idx = line.Find("second=", 0);
            second = (unsigned int)std::atoi(line.Substring(idx+7, 4));

            SDFGlyphKerning newKern;

            idx = line.Find("first=", 0);
            newKern.previous = (unsigned int)std::atoi(line.Substring(idx+6, 4));
            idx = line.Find("amount=", 0);
            newKern.amount = (int)std::atoi(line.Substring(idx+7, 4));

            // Assumption: Character data is already read
            for (unsigned int i = 0; i < (unsigned int)this->glyphs.size(); i++) {
                if (this->glyphs[i].id == second) {
                    this->glyphs[i].kernings.push_back(newKern);
                }
            }
        }
        // Proceed with next line ...
        lineCnt++;
    }
    //Clear ascii file buffer
    file.Clear();

    // Building character index array -----------------------------------------
    this->maxIdx = maxId++;
    this->glyphIdx = new SDFGlyphInfo*[this->maxIdx];
    // Init pointers 
    for (unsigned int i = 0; i < this->maxIdx; i++) {
        this->glyphIdx[i] = NULL;
    }
    // Set pointers to available glyph info
    for (unsigned int i = 0; i < (unsigned int)this->glyphs.size(); i++) {
        if (this->glyphs[i].id > this->maxIdx) {
            vislib::sys::Log::DefaultLog.WriteError("[SDFFont] [loadFontInfo] Character is out of range: \"%i\". \n", this->glyphs[i].id);
            return false;
        }
        this->glyphIdx[this->glyphs[i].id] = &this->glyphs[i];
    }

    return true;
}


/*
* SDFFont::loadTexture
*/
bool SDFFont::loadFontTexture(vislib::StringA filename) {

    // Reset font texture
    this->texture.Release();

    static vislib::graphics::BitmapImage img;
    static sg::graphics::PngBitmapCodec  pbc;
    pbc.Image() = &img;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    void   *buf  = NULL;
    size_t  size = 0;

    if ((size = this->loadFile(filename, &buf)) <= 0) {
        vislib::sys::Log::DefaultLog.WriteError("[SDFFont] [loadTexture] Could not find texture: \"%s\". \n", filename.PeekBuffer());
        ARY_SAFE_DELETE(buf);
        return false;
    }

    if (pbc.Load(buf, size)) {
        // (Using template with minimum channels containing alpha)
        img.Convert(vislib::graphics::BitmapImage::TemplateByteGrayAlpha); 
        // (Red channel is Gray value - Green channel is alpha value from png)
        if (this->texture.Create(img.Width(), img.Height(), false, img.PeekDataAs<BYTE>(), GL_RG) != GL_NO_ERROR) { 
            vislib::sys::Log::DefaultLog.WriteError("[SDFFont] [loadTexture] Could not load texture: \"%s\". \n", filename.PeekBuffer());
            ARY_SAFE_DELETE(buf);
            return false;
        }
        this->texture.SetFilter(GL_LINEAR, GL_LINEAR);
        this->texture.SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        ARY_SAFE_DELETE(buf);
    }
    else {
        vislib::sys::Log::DefaultLog.WriteError("[SDFFont] [loadTexture] Could not read texture: \"%s\". \n", filename.PeekBuffer());
        ARY_SAFE_DELETE(buf);
        return false;
    }

    return true;
}


/*
* SDFFont::loadFontShader
*/
bool SDFFont::loadFontShader(vislib::StringA vert, vislib::StringA frag) {

    // Reset shader
    this->shader.Release();

    const char *shaderName = "SDFFont";
    size_t size = 0;

    // Load shaders from file
    
    void *vertBuf = NULL;
    if ((size = this->loadFile(vert, &vertBuf)) <= 0) {
        vislib::sys::Log::DefaultLog.WriteError("[SDFFont] [loadShader] Could not find vertex shader: \"%s\". \n", vert.PeekBuffer());
        ARY_SAFE_DELETE(vertBuf);
        return false;
    }
    ((char *)vertBuf)[size-1] = '\0'; // Terminating buffer with '\0' is mandatory for being able to compile shader
    
    void *fragBuf = NULL;
    if ((size = this->loadFile(frag, &fragBuf)) <= 0) {
        vislib::sys::Log::DefaultLog.WriteError("[SDFFont] [loadShader] Could not find fragment shader: \"%s\". \n", frag.PeekBuffer());
        ARY_SAFE_DELETE(fragBuf);
        return false;
    }
    ((char *)fragBuf)[size-1] = '\0'; // Terminating buffer with '\0' is mandatory for being able to compile shader
    

    try {
        // Compiling shaders
        if (!this->shader.Compile((const char **)(&vertBuf), (size_t)1, (const char **)(&fragBuf), (size_t)1)) {
            vislib::sys::Log::DefaultLog.WriteError("[SDFFont] [loadShader] Unable to compile \"%s\"-shader: Unknown error. \n", shaderName);
            //ARY_SAFE_DELETE(vertBuf);
            ARY_SAFE_DELETE(fragBuf);
            return false;
        }

        // Bind vertex shader attributes (before linking shaders!)
        for (unsigned int i = 0; i < (unsigned int)this->vbos.size(); i++) {
            glBindAttribLocation(this->shader.ProgramHandle(), this->vbos[i].index, this->vbos[i].name.PeekBuffer());
        }

        // Linking shaders
        if (!this->shader.Link()) {
            vislib::sys::Log::DefaultLog.WriteError("[SDFFont] [loadShader] Unable to link \"%s\"-shader: Unknown error. \n", shaderName);
            //ARY_SAFE_DELETE(vertBuf);
            ARY_SAFE_DELETE(fragBuf);
            return false;
        }

        //ARY_SAFE_DELETE(vertBuf);
        ARY_SAFE_DELETE(fragBuf);
    }
    catch (vislib::graphics::gl::AbstractOpenGLShader::CompileException ce) {
        vislib::sys::Log::DefaultLog.WriteError("[SDFFont] [loadShader] Unable to compile \"%s\"-shader (@%s): %s. \n", shaderName,
            vislib::graphics::gl::AbstractOpenGLShader::CompileException::CompileActionName(ce.FailedAction()), ce.GetMsgA());
        return false;
    }
    catch (vislib::Exception e) {
        vislib::sys::Log::DefaultLog.WriteError("[SDFFont] [loadShader] Unable to compile \"%s\"-shader: %s. \n", shaderName, e.GetMsgA());
        return false;
    }
    catch (...) {
        vislib::sys::Log::DefaultLog.WriteError("[SDFFont] [loadShader] Unable to compile \"%s\"-shader: Unknown exception. \n", shaderName);
        return false;
    }

    return true;
}


/*
* SDFFont::loadFile
*/
size_t SDFFont::loadFile(vislib::StringA filename, void **outData) {

    // Reset out data
    *outData = NULL;


    vislib::StringW name = static_cast<vislib::StringW>(filename);
    if (name.IsEmpty()) {
        vislib::sys::Log::DefaultLog.WriteError("[SDFFont] [loadFile] Unable to load file: No name given. \n");
        return false;
    }
    if (!vislib::sys::File::Exists(name)) {
        vislib::sys::Log::DefaultLog.WriteError("[SDFFont] [loadFile] Unable to load not existing file: \"%s\". \n", filename.PeekBuffer());
        return false;
    }

    size_t size = static_cast<size_t>(vislib::sys::File::GetSize(name));
    if (size < 1) {
        vislib::sys::Log::DefaultLog.WriteError("[SDFFont] [loadFile] Unable to load empty file: \"%s\". \n", filename.PeekBuffer());
        return false;
    }

    vislib::sys::FastFile f;
    if (!f.Open(name, vislib::sys::File::READ_ONLY, vislib::sys::File::SHARE_READ, vislib::sys::File::OPEN_ONLY)) {
        vislib::sys::Log::DefaultLog.WriteError("[SDFFont] [loadFile] Unable to open file: \"%s\". \n", filename.PeekBuffer());
        return false;
    }

    *outData = new BYTE[size];
    size_t num = static_cast<size_t>(f.Read(*outData, size));
    if (num != size) {
        vislib::sys::Log::DefaultLog.WriteError("[SDFFont] [loadFile] Unable to read whole file: \"%s\". \n", filename.PeekBuffer());
        ARY_SAFE_DELETE(*outData);
        return false;
    }

    return num;
}

