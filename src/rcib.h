/* 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef RCIB_HELPER_
#define RCIB_HELPER_

namespace rcib {

  struct async_req {
    uv_work_t req;
    const char * file_content_;
    std::string func_name_;
    std::string param_;
    std::string error;
    char *out;
    ssize_t result;
    v8::Isolate* isolate;
    v8::Persistent<v8::Function> callback;
  };

  class ArrayBufferAllocator : public v8::ArrayBuffer::Allocator {
  public:
    virtual void* Allocate(size_t length);
    virtual void* AllocateUninitialized(size_t length);
    virtual void Free(void* data, size_t);
  };

  class RcibHelper {
    public:
      explicit RcibHelper();
      //static
      static RcibHelper* GetInstance();
      static void AfterAsync(uv_work_t* r);
      static void DoAsync(uv_work_t* r);

      void Init();
      void Terminate();
      void DoTask(const std::string &filePath, const std::string& funcName, 
          const std::string& param, async_req * req );
    private:
      INT64 cprocessed_;
      std::map<std::string, std::string> map_fileContent_;
      typedef std::map<std::string, std::string>::iterator FileIter;
  };

} //end rcib

#endif
