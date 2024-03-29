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

/****************************************************************************************
**
* @file     weakPtr.cc

* @brief    

  @example  

* @author   classfellow@qq.com
* @date     2013-10 
* @updata   2014-10-21 
* @warning  
**           
****************************************************************************************/
#ifdef _WIN32
#include <windows.h>
#else
#include <assert.h>
#endif
#include "macros.h"
#include "ref_counted.h"
#include "WrapperObj.h"
#include "WeakPtr.h"

namespace base{
  WeakReference::Flag::~Flag() {
  }

  WeakReference::WeakReference() {
  }

  WeakReference::WeakReference(const Flag* flag) : flag_(flag) {
  }

  WeakReference::~WeakReference() {
  }

  bool WeakReference::is_valid() const { return flag_.get() && flag_->IsValid(); }

  WeakReferenceOwner::WeakReferenceOwner() {
  }

  WeakReferenceOwner::~WeakReferenceOwner() {
    Invalidate();
  }

  WeakReference WeakReferenceOwner::GetRef() const {
    // If we hold the last reference to the Flag then create a new one.
    if (!HasRefs())
      flag_ = new WeakReference::Flag();

    return WeakReference(flag_.get());
  }

  void WeakReferenceOwner::Invalidate() {
    if (flag_.get()) {
      flag_->Invalidate();
      flag_ = NULL;
    }
  }

} //end namespace base