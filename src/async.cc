// Copyright (c) 2015 Anatoly Ikorsky

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#include <nan.h>
#include <vector>
#include "tiff_multipage.h"
#include "async.h"

using Nan::AsyncWorker;
using Nan::HandleScope;
using Nan::Callback;

using v8::Array;
using v8::Function;
using v8::Local;
using v8::Value;

class TiffMultipageWorker : public AsyncWorker
{
public:
    TiffMultipageWorker(Callback *callback, std::string output, std::vector<std::string> input)
        : AsyncWorker(callback), result(NULL), output(output), input(input) {}
    ~TiffMultipageWorker() {}

    void Execute() {
        result = join(output, input);
    }

    void HandleOKCallback() {
        HandleScope scope;

        if (result == NULL) {
            Local<Value> argv[] = {
                Nan::Null(),
                Nan::Null(),
            };

            callback->Call(2, argv);
        } else {
            Local<Value> argv[] = {
                Nan::Error(result),
                Nan::Null(),
            };

            callback->Call(2, argv);
        }
    }

private:
    const char * result;
    std::string output;
    std::vector<std::string> input;
};

NAN_METHOD(JoinAsync) {
    if (info.Length() != 3) {
        return Nan::ThrowError(
            "Expected arguments: (\"output/file/name\", [\"file\", \"names\", \"to\", \"join\"])");
    } else if (! info[0]->IsString()) {
        return Nan::ThrowTypeError("First argument is not a String");
    } else if (! info[1]->IsArray()) {
        return Nan::ThrowTypeError("Second argument is not an Array");
    } else if (! info[2]->IsFunction()) {
        return Nan::ThrowTypeError("Third argument is not a Function");
    }

    const Nan::Utf8String out_filename(info[0]);
    std::string output(*out_filename, out_filename.length());

    const Local<Array> in_filenames = info[1].As<Array>();
    const uint32_t in_filenames_len = in_filenames->Length();

    if (in_filenames_len == 0) {
        return Nan::ThrowError("Non input files");
    }

    std::vector<std::string> input(in_filenames_len);

    for (uint32_t i = 0; i < in_filenames_len; ++i)
    {
        Local<Value> name = Nan::Get(in_filenames, i).ToLocalChecked();
        if (! name->IsString()) {
            return Nan::ThrowTypeError("Non string found in input file names array");
        }
        Nan::Utf8String input_file_name(name);
        input[i] = std::string(*input_file_name, input_file_name.length());
    }

    Callback *callback = new Callback(info[2].As<Function>());

    AsyncQueueWorker(new TiffMultipageWorker(callback, output, input));
}
