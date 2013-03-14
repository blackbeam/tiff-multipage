#include "libtiff.hh"

using namespace node;
using namespace v8;

Persistent<FunctionTemplate> TIFFFile::constructor_template;

namespace node {

  void TIFFFile::Initialize(Handle<Object> target) {
    HandleScope scope;

    Local<FunctionTemplate> t = FunctionTemplate::New(TIFFFile::New);
    constructor_template = Persistent<FunctionTemplate>::New(t);
    constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
    constructor_template->SetClassName(String::NewSymbol("TIFFFile"));

    constructor_template->InstanceTemplate()->
      SetAccessor(String::NewSymbol("IMAGEWIDTH"), TIFFFile::fieldsGetter);
    constructor_template->InstanceTemplate()->
      SetAccessor(String::NewSymbol("IMAGELENGTH"), TIFFFile::fieldsGetter);
    constructor_template->InstanceTemplate()->
      SetAccessor(String::NewSymbol("BITSPERSAMPLE"), TIFFFile::fieldsGetter);
    constructor_template->InstanceTemplate()->
      SetAccessor(String::NewSymbol("SAMPLESPERPIXEL"), TIFFFile::fieldsGetter);
    constructor_template->InstanceTemplate()->
      SetAccessor(String::NewSymbol("PLANARCONFIG"), TIFFFile::fieldsGetter);
    constructor_template->InstanceTemplate()->
      SetAccessor(String::NewSymbol("PHOTOMETRIC"), TIFFFile::fieldsGetter);
    constructor_template->InstanceTemplate()->
      SetAccessor(String::NewSymbol("ORIENTATION"), TIFFFile::fieldsGetter);
    constructor_template->InstanceTemplate()->
      SetAccessor(String::NewSymbol("scanlineSize"), TIFFFile::fieldsGetter);
    constructor_template->InstanceTemplate()->
      SetAccessor(String::NewSymbol("filename"), TIFFFile::fieldsGetter);

    NODE_SET_PROTOTYPE_METHOD(constructor_template, "pushAsPage", TIFFFile::pushAsPage);

    target->Set(String::NewSymbol("TIFFFile"), constructor_template->GetFunction());
  }

  Handle<Value> TIFFFile::New(const Arguments &args) {
    HandleScope scope;
    TIFFFile *tifffile;

    /* Validate input parameters */
    if (args.Length() == 2 && args[0]->IsString() && args[1]->IsString()) {
      String::Utf8Value filename(args[0]);
      String::Utf8Value mode(args[1]);

      /* Open tiff file */
      TIFF* tif = TIFFOpen(*filename, *mode);
      /* Check for success */
      if (tif != NULL) {
        /* Create TIFFFile object */
        tifffile = new TIFFFile(tif, *filename, (strcmp(*mode, "r") != 0));
      } else {
        /* Throw an error */
        return ThrowException(Exception::TypeError(String::New("Can't open tiff file.")));
      }
    } else {
      /* Throw an error */
      return ThrowException(Exception::TypeError(String::New("Invalid params (String filename, String mode)")));
    }
    /* Wrap object */
    tifffile->Wrap(args.This());
    /* Return new TIFFFile */
    return args.This();
  }

  Handle<Value> TIFFFile::fieldsGetter(Local< String > property, const AccessorInfo &info) {
        HandleScope scope;
        TIFFFile *self = ObjectWrap::Unwrap<TIFFFile>(info.This());

        String::Utf8Value propName(property);

        if (strcmp(*propName, "IMAGEWIDTH") == 0) {
          uint32 imageWidth;
          TIFFGetField(self->tif, TIFFTAG_IMAGEWIDTH, &imageWidth);
          return scope.Close(Uint32::New(imageWidth));
        }

        if (strcmp(*propName, "IMAGELENGTH") == 0) {
          uint32 imageLength;
          TIFFGetField(self->tif, TIFFTAG_IMAGELENGTH, &imageLength);
          return scope.Close(Uint32::New(imageLength));
        }

        if (strcmp(*propName, "BITSPERSAMPLE") == 0) {
          uint16 bps;
          TIFFGetField(self->tif, TIFFTAG_BITSPERSAMPLE, &bps);
          return scope.Close(Uint32::New(bps));
        }

        if (strcmp(*propName, "SAMPLESPERPIXEL") == 0) {
          uint16 spp;
          TIFFGetField(self->tif, TIFFTAG_SAMPLESPERPIXEL, &spp);
          return scope.Close(Uint32::New(spp));
        }

        if (strcmp(*propName, "PLANARCONFIG") == 0) {
          uint16 pc;
          TIFFGetField(self->tif, TIFFTAG_PLANARCONFIG, &pc);
          return scope.Close(Uint32::New(pc));
        }

        if (strcmp(*propName, "PHOTOMETRIC") == 0) {
          uint16 pm;
          TIFFGetField(self->tif, TIFFTAG_PHOTOMETRIC, &pm);
          return scope.Close(Uint32::New(pm));
        }

        if (strcmp(*propName, "ORIENTATION") == 0) {
          uint16 o9n;
          TIFFGetFieldDefaulted(self->tif, TIFFTAG_ORIENTATION, &o9n);
          return scope.Close(Uint32::New(o9n));
        }

        if (strcmp(*propName, "scanlineSize") == 0) {
          int32 ss;
          ss = TIFFScanlineSize(self->tif);
          return scope.Close(Int32::New(ss));
        }

        if (strcmp(*propName, "filename") == 0) {
          return scope.Close(String::New(self->filename));
        }
  }

  Handle<Value> TIFFFile::pushAsPage(const Arguments &args) {
    HandleScope scope;
    TIFFFile *self = ObjectWrap::Unwrap<TIFFFile>(args.This());

    if (!self->forWrite) {
      return ThrowException(Exception::TypeError(String::New("Destination tiff must be opened for writing.")));
    }

    if (args.Length() != 1 || !TIFFFile::HasInstance(args[0])) {
      return ThrowException(Exception::TypeError(String::New("Invalid params (TIFFFile source)")));
    }
    TIFFFile *src = ObjectWrap::Unwrap<TIFFFile>(args[0]->ToObject());
    if (src->forWrite) {
      return ThrowException(Exception::TypeError(String::New("Source tiff must be opened for reading.")));
    }

    uint32 imageWidth, imageLength;
    uint16 bitsPerSample, samplesPerPixel, planarConfig, photometric, orientation;
    int32 scanlineSize;
    tdata_t buf;

    TIFFGetField(src->tif, TIFFTAG_IMAGEWIDTH, &imageWidth);
    TIFFGetField(src->tif, TIFFTAG_IMAGELENGTH, &imageLength);
    TIFFGetField(src->tif, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);
    TIFFGetField(src->tif, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel);
    TIFFGetField(src->tif, TIFFTAG_PLANARCONFIG, &planarConfig);
    TIFFGetField(src->tif, TIFFTAG_PHOTOMETRIC, &photometric);
    TIFFGetFieldDefaulted(src->tif, TIFFTAG_ORIENTATION, &orientation);
    scanlineSize = TIFFScanlineSize(src->tif);

    TIFFSetField(self->tif, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
    TIFFSetField(self->tif, TIFFTAG_IMAGEWIDTH, imageWidth);
    TIFFSetField(self->tif, TIFFTAG_IMAGELENGTH, imageLength);
    TIFFSetField(self->tif, TIFFTAG_BITSPERSAMPLE, bitsPerSample);
    TIFFSetField(self->tif, TIFFTAG_SAMPLESPERPIXEL, samplesPerPixel);
    TIFFSetField(self->tif, TIFFTAG_PLANARCONFIG, planarConfig);
    TIFFSetField(self->tif, TIFFTAG_PHOTOMETRIC, photometric);
    TIFFSetField(self->tif, TIFFTAG_ORIENTATION, orientation);
    TIFFSetField(self->tif, TIFFTAG_SUBFILETYPE, FILETYPE_PAGE);
    TIFFSetField(self->tif, TIFFTAG_PAGENUMBER, self->pageNumber, self->pageNumber + 1);

    buf = _TIFFmalloc(scanlineSize);
    if (planarConfig == PLANARCONFIG_CONTIG) {
      for (uint32 i = 0; i < imageLength; i++) {
        TIFFReadScanline(src->tif, buf, i);
        TIFFWriteScanline(self->tif, buf, i);
      }
    } else if (planarConfig == PLANARCONFIG_SEPARATE) {
      for (uint16 s = 0; s < samplesPerPixel; s++) {
        for (uint32 i = 0; i < imageLength; i++) {
          TIFFReadScanline(self->tif, buf, i, s);
          TIFFWriteScanline(self->tif, buf, i, s);
        }
      }
      for (uint32 i = 0; i < imageLength; i++) {
        TIFFReadScanline(src->tif, buf, i, 0);
        TIFFWriteScanline(self->tif, buf, i, 0);
      }
    }
    _TIFFfree(buf);

    TIFFWriteDirectory(self->tif);
    self->pageNumber++;
    return args.This();
  }

}

// Exporting function
extern "C" void
init (v8::Handle<v8::Object> target)
{
    HandleScope scope;

    TIFFFile::Initialize(target);
}

NODE_MODULE(tiff_multipage, init);