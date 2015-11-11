#'rcib'

RCIB(running codes in background js) is a node.js package for providing ability to run js codes in background or a way to extend node.js by C++ closures.

**[Follow me on github!](https://github.com/classfellow/rcib)**


##Compile & Link

npm install -g node-gyp
node-gyp configure
node-gyp build


## Usage

#example:

var path = require('path')
var rcib = require('../index.js')

rcib.postTask(path.join(__dirname, './background.js'), 'globalFunction2', 
 JSON.stringify({a:100, b: 200}), function (err, value){
   if(err) console.error(err)
   else console.log(value)
})

rcib.postDelayedTask(path.join(__dirname, './background.js'), 'globalFunction2', 
  JSON.stringify({a:100, b: 200}), 1000, function (err, value){
    if(err) console.error(err)
    else console.log(value)
})

'postTask' mesans to run a function in background js. It accepts four parameters.
   param1 : full path to a js file
   param2 : function name to run
   param3 : parameters in json whith will be used by param2
   param4 : callback function 

'postDelayedTask' mesans to run a function in background js after specified milliseconds. It accepts five parameters.
   param1 : full path to a js file
   param2 : function name to run
   param3 : parameters in json whith will be used by param2
   param4 : delayed time in millisecond
   param5 : callback function


## Extends node in command mode and C++ closures

#exmple:

class Red : public base::RefCounted {
  void Func() {}
  // ...
};

local_thread_.Get().message_loop()->PostTask(base::Bind((new Red),
    &Red::Func, param1, param2));

 Derive your class from base::RefCounted
 new a Red
 post a c++ closure 
