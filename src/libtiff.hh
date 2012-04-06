#ifndef NODE_LIBTIFF_H_
#define NODE_LIBTIFF_H_

#include <cstring>
#include <node.h>
#include <v8.h>

#include "tiffio.h"

namespace node {
  class TIFFFile : public node::ObjectWrap {
    public:
      ~TIFFFile() {
        TIFFClose(tif);
        delete[] filename;
      };

      static void Initialize(v8::Handle<v8::Object> target);

      static v8::Handle<v8::Value> New(const v8::Arguments &args);

      static inline bool HasInstance(v8::Handle<v8::Value> val) {
        if (!val->IsObject()) return false;
        v8::Local<v8::Object> obj = val->ToObject();
        return constructor_template->HasInstance(obj);
      }
      static v8::Handle<v8::Value> pushAsPage(const v8::Arguments &args);

    private:
      TIFFFile(TIFF *tifHandle, const char* filename, bool forWrite) {
        tif = tifHandle;
        this->filename = new char [strlen(filename) + 1];
        strcpy(this->filename, filename);
        this->forWrite = forWrite;
        if (forWrite) {
          pageNumber = 0;
        }
      };

      static v8::Handle<v8::Value> fieldsGetter(v8::Local<v8::String> property, const v8::AccessorInfo &info);

      TIFF* tif;
      char* filename;
      bool forWrite;
      uint16 pageNumber;
      static v8::Persistent<v8::FunctionTemplate> constructor_template;
  };
}

#endif
