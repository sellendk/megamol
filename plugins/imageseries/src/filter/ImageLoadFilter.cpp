#include "ImageLoadFilter.h"

#include "mmcore/utility/graphics/BitmapCodecCollection.h"

namespace megamol::ImageSeries::filter {

ImageLoadFilter::ImageLoadFilter(Input input) : input(input) {}

ImageLoadFilter::ImageLoadFilter(
    std::shared_ptr<vislib::graphics::BitmapCodecCollection> codecs, std::string filename, std::size_t sizeEstimate) {
    input.codecs = codecs;
    input.filename = filename;
    input.sizeEstimate = sizeEstimate;
}

ImageLoadFilter::ImagePtr ImageLoadFilter::operator()() {
    const char* filename = input.filename.c_str();
    try {
        auto img = std::make_shared<vislib::graphics::BitmapImage>();
        if (!input.codecs->LoadBitmapImage(*img, filename)) {
            throw vislib::Exception("No suitable codec found", __FILE__, __LINE__);
        }
        return img;
    } catch (vislib::Exception& ex) {
        // TODO thread-safe log?
        //Log::DefaultLog.WriteMsg(Log::LEVEL_ERROR, "Unable to load image '%s': %s (%s, %d)\n", filename, ex.GetMsgA(),
        //    ex.GetFile(), ex.GetLine());
    } catch (...) {
        // TODO thread-safe log?
        //Log::DefaultLog.WriteMsg(Log::LEVEL_ERROR, "Unable to load image '%s': unexpected exception\n", filename);
    }

    return nullptr;
}

std::size_t ImageLoadFilter::getByteSize() const {
    return input.sizeEstimate;
}

AsyncImageData2D::Hash ImageLoadFilter::getHash() const {
    return util::computeHash(input.filename);
}

} // namespace megamol::ImageSeries::filter
