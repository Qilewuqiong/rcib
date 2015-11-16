// Copyright Joyent, Inc. and other Node contributors.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <node.h>
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

using namespace rcib;

namespace rcib{
  extern bool bterminating_;
}
extern base::LazyInstance<base::Thread> local_thread_;

// Post a task, runs in other thread
void PostTask(const v8::FunctionCallbackInfo<v8::Value>& args) {
  //process existing
  if (bterminating_) return;

  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope scope(isolate);

  if(args.Length() != 4 || !args[0]->IsString() || !args[1]->IsString() || !args[2]->IsString() 
    || !args[3]->IsFunction() ) {
    isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "param error")));
    return;
  }

  v8::String::Utf8Value file(args[0]);
  v8::String::Utf8Value func(args[1]);
  v8::String::Utf8Value param(args[2]);

  std::string strfilePath = *file;
  std::string strfuncName = *func;
  std::string strParam = *param;
  v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(args[3]);

  async_req* req = new async_req;
  req->req.data = req;
  req->isolate = isolate;
  req->out = NULL;
  req->callback.Reset(isolate, callback);

  local_thread_.Get().message_loop()->PostTask(base::Bind(base::Unretained(RcibHelper::GetInstance()),
    &RcibHelper::DoTask, strfilePath, strfuncName, strParam, req));
}

// Post a delayed task, runs in other thread
void PostDelayedTask(const v8::FunctionCallbackInfo<v8::Value>& args) {
  //process existing
  if (bterminating_) return;

  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope scope(isolate);
  
  if(args.Length() != 5 || !args[0]->IsString() || !args[1]->IsString() || !args[2]->IsString() 
    || !args[3]->IsNumber() || !args[4]->IsFunction()) {
    isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "param error")));
    return;
  }

  v8::String::Utf8Value file(args[0]);
  v8::String::Utf8Value func(args[1]);
  v8::String::Utf8Value param(args[2]);
  INT64 delayed = args[3]->ToUint32()->Value();

  std::string strfilePath = *file;
  std::string strfuncName = *func;
  std::string strParam = *param;
  v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(args[4]);

  async_req* req = new async_req;
  req->req.data = req;
  req->isolate = isolate;
  req->out = NULL;
  req->callback.Reset(isolate, callback);

  local_thread_.Get().message_loop()->PostDelayedTask(base::Bind(base::Unretained(RcibHelper::GetInstance()),
    &RcibHelper::DoTask, strfilePath, strfuncName, strParam, req ), 
    base::TimeDelta::FromMilliseconds(delayed) );
}

// Post a node task, a node task means to extend node abilities by c/c++
void PostNodeTask(const v8::FunctionCallbackInfo<v8::Value>& args) {
  //process existing
  if (bterminating_) return;

  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope scope(isolate);
  
}

// Terminate thread
void Terminate(const v8::FunctionCallbackInfo<v8::Value>& args) {
  bterminating_ = true;
  RcibHelper::GetInstance()->Terminate();
}

void Init(v8::Local<v8::Object> target) {
  NODE_SET_METHOD(target, "postTask", PostTask);
  NODE_SET_METHOD(target, "postDelayedTask", PostDelayedTask);
  NODE_SET_METHOD(target, "terminate", Terminate);
  //NODE_SET_METHOD(target, "postNodeTask", PostNodeTask);
  //do init jobs
  RcibHelper::GetInstance()->Init();
}

NODE_MODULE(rcib, Init)