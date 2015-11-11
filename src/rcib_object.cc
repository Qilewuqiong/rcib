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

#include <node.h>
//#include <libplatform/libplatform.h>
#include <v8.h>
#include <uv.h>
#if defined _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <malloc.h>
#include <stdlib.h>
#include <string>
#include <memory>
#include <list>
#include <queue>
#include <stack>
#include <map>
#include "rcib/macros.h"
#include "rcib/aligned_memory.h"
#include "rcib/lazy_instance.h"
#include "rcib/ref_counted.h"
#include "rcib/WrapperObj.h"
#include "rcib/WeakPtr.h"
#include "rcib/FastDelegateImpl.h"
#include "rcib/time/time.h"
#include "rcib/MessagePump.h"
#include "rcib/util_tools.h"
#include "rcib/Event/WaitableEvent.h"
#include "rcib/PendingTask.h"
#include "rcib/observer_list.h"
#include "rcib/MessagePumpDefault.h"
#include "rcib/MessageLoop.h"
#include "rcib/Thread.h"
#include "rcib/at_exist.h"

#include "rcib.h"

namespace rcib{
  bool bterminating_ = false;
  base::AtExitManager* g_top_manager = NULL;
} //end rcib

// should be after the previous statement
base::AtExitManager atmgr;
// should be after the previous statement. all global lazyInstance should be defined in this file.
base::LazyInstance<rcib::ArrayBufferAllocator> array_buffer_allocator_ = LAZY_INSTANCE_INITIALIZER;
base::LazyInstance<base::Thread> local_thread_ = LAZY_INSTANCE_INITIALIZER;

namespace rcib {

  void* ArrayBufferAllocator::Allocate(size_t length) {
    void* data = AllocateUninitialized(length);
    return data == NULL ? data : memset(data, 0, length);
  }
  void* ArrayBufferAllocator::AllocateUninitialized(size_t length) { return malloc(length); }
  void ArrayBufferAllocator::Free(void* data, size_t) { free(data); }

  // constructor
  RcibHelper::RcibHelper()
    :cprocessed_(0){
  }
  //static
  RcibHelper* RcibHelper::GetInstance(){
    static RcibHelper This;
    return &This;
  }
  //static
  void RcibHelper::AfterAsync(uv_work_t* r){

    async_req* req = reinterpret_cast<async_req*>(r->data);
    v8::Isolate* isolate = req->isolate;
    v8::HandleScope scope(isolate);

    // there is always at least one argument. "error"
    int argc = 1;

    v8::Local<v8::Value> argv[2];

    if(-1 == req->result || !req->out){
      printf("error: %s\n", req->error.c_str());
      argv[0] = node::UVException(-1, req->error.c_str(), req->func_name_.c_str(), "");
    }else{
      // error value is empty or null for non-error.
      argv[0] = v8::Null(isolate);
      // the data to pass is a string , use json
      argc = 2;
      char *data = (char *)req->out;
      argv[1] = v8::String::NewFromUtf8(isolate, data);
    }

    v8::TryCatch try_catch(isolate);

    v8::Local<v8::Function> callback =
      v8::Local<v8::Function>::New(isolate, req->callback);
    callback->Call(isolate->GetCurrentContext()->Global(), argc, argv);

    // cleanup
    req->callback.Reset();
    if(req->out){
      free(req->out);
      req->out = NULL;
    }
    delete req;

    if (try_catch.HasCaught()) {
      node::FatalException(isolate, try_catch);
    }
  }
  //static
  void RcibHelper::DoAsync(uv_work_t* r){
    async_req* req = reinterpret_cast<async_req*>(r->data);

    if(!req->file_content_) {
      req->result = -1;
      req->error = "read background file failed.";
      printf("%s\n", req->error.c_str());
      return;
    }

    v8::Isolate::CreateParams params;
    params.array_buffer_allocator = &array_buffer_allocator_.Get();
    v8::Isolate* isolate = v8::Isolate::New(params);
    do {
      v8::Locker locker(isolate);

      v8::Isolate::Scope isolate_scope(isolate);
      // Create a stack-allocated handle scope.
      v8::HandleScope handle_scope(isolate);

      // Create a new context.
      v8::Local<v8::Context> context = v8::Context::New(isolate);

      // Enter the context for compiling and running the hello world script.
      v8::Context::Scope context_scope(context);

      v8::TryCatch catcher;
      // Create a string containing the JavaScript source code.
      v8::Local<v8::String> source =
        v8::String::NewFromUtf8(isolate, req->file_content_);

      // Compile the source code.
      v8::Local<v8::Script> script = v8::Script::Compile(context, source).ToLocalChecked();

      // Run the script to get the result.
      v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();

      if (catcher.HasCaught())  {
        req->result = -1;
        req->error = "javascript in background file has error.";
        printf("%s\n", req->error.c_str());
        break;
      }

      v8::Handle<v8::Object> globalObj = isolate->GetCurrentContext()->Global();
      v8::Handle<v8::Value>  globalFunction = globalObj->Get(v8::String::NewFromUtf8(isolate, req->func_name_.c_str()));
      if (globalFunction.IsEmpty() || !globalFunction->IsFunction()) {
        req->result = -1;
        req->error = "Invalid function name";
        printf("%s\n", req->error.c_str());
        break;
      }
      v8::Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(globalFunction);
      v8::Local<v8::Value> v1 = v8::String::NewFromUtf8(isolate, req->param_.c_str());
      v8::Handle<v8::Value> args[1] = { v1 };
      v8::Handle<v8::Value> tmpCallVal = func->Call(globalObj, 1, args);
      v8::Local<v8::String> V8String= tmpCallVal->ToString();
      int len = 1 + V8String->Utf8Length();
      char *p = (char *)malloc(len);
      V8String->WriteUtf8(p);
      //set result
      req->out = p;
      req->result = 1;

    }while(false);

    // Dispose the isolate
    isolate->Dispose();
  }

  void RcibHelper::Init(){
    if(!local_thread_.Get().IsRunning()){
      local_thread_.Get().set_thread_name("distribute_task_thread");
      local_thread_.Get().StartWithOptions(base::Thread::Options());
    }
  }

  void RcibHelper::Terminate(){
    local_thread_.Get().Stop();
  }

  void RcibHelper::DoTask(const std::string &filePath, const std::string& funcName,
    const std::string& param, async_req * req){

    const char * ptrContent = NULL;
    FileIter iter = map_fileContent_.find(filePath);
    if(map_fileContent_.end() == iter){
      // read file into buffer & filePath is a full path
      std::string fileContent = tk::getFileContents(filePath);
      map_fileContent_.insert(std::make_pair(filePath, fileContent));
    }
    iter = map_fileContent_.find(filePath);
    ptrContent = iter->second.c_str();
    if(iter->second.length() == 0) {
      ptrContent = NULL;
    }
    req->file_content_ = ptrContent;
    req->func_name_ = funcName;
    req->param_ = param;
    req->out = NULL;

    uv_queue_work(uv_default_loop(),
      &req->req,
      DoAsync,
      (uv_after_work_cb)AfterAsync);

    cprocessed_++;
  }

} // end rcib
