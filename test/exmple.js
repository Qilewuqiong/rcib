var path = require('path')
var rcib = require('../index.js')

console.log('running...')

rcib.postTask(path.join(__dirname, './background.js'), 'globalFunction2', 
 JSON.stringify({a:100, b: 200}), function (err, value){
   if(err) console.error(err)
   else console.log(value)
})

setTimeout(function(){
  console.log('')
}, 500)
