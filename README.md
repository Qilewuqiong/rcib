#'rcib'

RCIB(running codes in background js) is a node.js package for providing ability to run js codes in background or a way to extend node.js by C++ closures.

**[Follow me on github!](https://github.com/classfellow/rcib)**


##Compile & Link
> 
npm install -g node-gyp
> 
node-gyp configure
> 
node-gyp build

##How it works

![rcib](http://www.outman.com/images/rcib-interpret.png)



## Usage

###example:

```javascript
>var path = require('path') <br>
 var rcib = require('../index.js') <br>
 rcib.postTask(path.join(__dirname, './background.js'), 'globalFunction2', <br>
   &nbsp;&nbsp;&nbsp; JSON.stringify({a:100, b: 200}), function (err, value){ <br>
   &nbsp;&nbsp;&nbsp; if(err) console.error(err) <br>
   &nbsp;&nbsp;&nbsp; else console.log(value) <br>
})

>rcib.postDelayedTask(path.join(__dirname, './background.js'), 'globalFunction2', <br> 
  &nbsp;&nbsp;&nbsp;JSON.stringify({a:100, b: 200}), 1000, function (err, value) { <br>
  &nbsp;&nbsp;&nbsp;  if(err) console.error(err) <br>
  &nbsp;&nbsp;&nbsp;  else console.log(value) <br>
})

```

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


## Extends node in command mode or C++ closures

###exmple:

```C++ code

>class Red : public base::RefCounted { <br>
  &nbsp;&nbsp;&nbsp;void Func() {} <br>
  &nbsp;&nbsp;&nbsp;// ... <br>
}; <br>
local_thread_.Get().message_loop()->PostTask(base::Bind((new Red), <br>
    &nbsp;&nbsp;&nbsp;&Red::Func, param1, param2)); <br>

```

 Derive your class from base::RefCounted

 new a Red

 post a c++ closure

 
