
// spectralimage.h

#ifndef _JUDE_RAD_SPECTRALIMAGE
#define _JUDE_RAD_SPECTRALIMAGE

#include "rad/rendertarget.h"
#include "rad/bandspec.h"
#include "base/jerror.h"

namespace jude {
namespace rad {


template <typename SD>
class SpectralImage : public RenderTarget {

  int x_size;
  int y_size;

  int band_count;

  const BandSpec* bs;

  typename SD::elem_type* elem_data;
  bool* mask_data;

  void alloc() { 
    elem_data = new typename SD::elem_type[x_size*y_size*band_count];
    mask_data = new bool[x_size * y_size]; 
  }

  void freeAlloc() { 
    delete[] elem_data;
    delete[] mask_data;
    elem_data = 0;
    mask_data = 0;
    x_size = 0;
    y_size = 0;
    band_count = 0; 
  }

public:

    /*! \brief Construct an empty image. */
  SpectralImage() :
    x_size(0),
    y_size(0),
    band_count(0),
    bs(0),
    elem_data(0),
    mask_data(0) { }

    /*! \brief Construct an image of dimensions \a xs by \a ys with \a bc bands. */
  SpectralImage(int xs, int ys, int bc) :
    x_size(xs),
    y_size(ys),
    band_count(bc),
    bs(0) { alloc(); }

    /*! \brief Destructor. */
  ~SpectralImage() { freeAlloc(); }

    /*! \brief Returns the factory ID needed to reconstruct this object on loading. */
  const JString factoryID() const { return staticFactoryID(); }
    /*! \brief Static version of the above function. Note that both assume SD is not a virtual type. */
  static const JString staticFactoryID() { return "sif_"+staticIdOf<SD>(); }

    /*! \brief Returns band specification assocociated with this image or null. */
  const BandSpec* bandSpec() const { return bs; }
    /*! \brief Set a pointer to the band specification that corresponds to this image. */
  void setBandSpec(const BandSpec& b) { delete bs; bs = new BandSpec(b); }

    /*! \brief Returns the width (or x-size) of the image in pixels. */
  int width() const { return x_size; }
    /*! \brief Returns the height (or y-size) of the image in pixels. */
  int height() const { return y_size; }

    /*! \brief Returns the number of bands of the image. */
  int bandCount() const { return band_count; }

    /*! \brief Equivalent to calling clearData() and fillMask(). */
  void clear() {
    clearData();
    fillMask();
  }

    /*! \brief Clears all data values to whatever constructor SD(0) produces, i.e. usually zero. */
  void clearData() {
    typename SD::elem_type* ptr = elem_data;
    typename SD::elem_type* const ptr_end = elem_data + (x_size*y_size*band_count);
    for (; ptr!=ptr_end; ptr++) (*ptr) = typename SD::elem_type(0); 
  }

    /*! \brief Clears the mask, i.e. so the whole image is unmasked. */
  void clearMask() {
    bool* ptr = mask_data;
    bool* const ptr_end = mask_data + (x_size*y_size);
    for (; ptr!=ptr_end; ptr++) (*ptr) = false; 
  }

    /*! \brief Fills the mask, i.e. so the whole image is masked. */
  void fillMask() {
    bool* ptr = mask_data;
    bool* const ptr_end = mask_data + (x_size*y_size);
    for (; ptr!=ptr_end; ptr++) (*ptr) = true; 
  }

  void renderPixel(int x, int y, const vector<float>& v) {
    if (!jassert(v.size() == band_count)) { jlog::es << __FUNCTION__ << ": band_count=" << band_count << " v.size()=" << ((int)v.size()) << "\n"; internalError(); } 
    int offset = y*width() + x;
    mask_data[offset] = false;
    offset *= band_count;
    for (int i=0; i<band_count; i++,offset++) elem_data[offset] = v(i); 
  }

  void getPixel(int x, int y, vector<float>& v, bool& is_masked) {
    int offset = y*width() + x;
    is_masked = mask_data[offset];
    if (is_masked) return;
    v.resize(band_count);
    offset *= band_count;
    for (int i=0; i<band_count; i++,offset++) v(i) = elem_data[offset]; 
  }

    /*! \brief Reads in from a data stream where source data was created by \a saved_version of the Jude library. */
  bool read(JDataStream& str, uns32 saved_version);

  bool write(JDataStream& str) const;

    /*! \brief Not implemented. */
  bool read(JTextStream& str) { return false; }
    /*! \brief Not implemented. */
  bool write(JTextStream& str) const { return false; }


  /*
  void displayXLine() const;

  void write(JDataStream& str) const;
  void read(JDataStream& str);

  SpectralData* dataPtr() { return sd_data; }
  const SpectralData* dataPtr() const { return sd_data; }

  SpectralData& at(int x, int y) { return sd_data[y*xs + x]; }
  const SpectralData& at(int x, int y) const { return sd_data[y*xs + x]; }

  void drawToBuffer(Buffer2D& buff, double sens_k, JColour mask_col) const;

  void clear(const ProjDrawDef& pdd);
  void renderPixel(int x, int y, const ProjDrawDef& pdd, const SpectralData& sd);

  */
};




template <typename SD>
bool SpectralImage<SD>::read(JDataStream& str, uns32 saved_version) {

  freeAlloc();

  str >> x_size;
  str >> y_size;
  str >> band_count;

  bool has_band_spec;
  str >> has_band_spec;
  if (has_band_spec) bs = new BandSpec(str, saved_version);

  alloc();

  typename SD::elem_type* ptr = elem_data;
  typename SD::elem_type* const ptr_end = elem_data + (x_size*y_size*band_count);
  for (; ptr!=ptr_end; ptr++) str >> (*ptr);

  bool* mptr = mask_data;
  bool* const mptr_end = mask_data + (x_size*y_size);
  for (; mptr!=mptr_end; mptr++) str >> (*mptr); 

  return true;
}



template <typename SD>
bool SpectralImage<SD>::write(JDataStream& str) const { 

  str << x_size;
  str << y_size;
  str << band_count;

  bool has_band_spec = (bs != 0);
  str << has_band_spec;
  if (bs) if (!bs->write(str)) return false;

  typename SD::elem_type* ptr = elem_data;
  typename SD::elem_type* const ptr_end = elem_data + (x_size*y_size*band_count);
  for (; ptr!=ptr_end; ptr++) str << (*ptr);

  bool* mptr = mask_data;
  bool* const mptr_end = mask_data + (x_size*y_size);
  for (; mptr!=mptr_end; mptr++) str << (*mptr); 

  return true;
}



} } // namespace jude::rad

#endif // _JUDE_RAD_SPECTRALIMAGE

