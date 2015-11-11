var path = require('path')
var rcib = require('../index.js')

console.log('running...')

var loop = setInterval(function(){
  rcib.postTask(path.join(__dirname, './background.js'), 'globalFunction', 
  JSON.stringify({a:100, b: 200}), function (err, value){
    if(err) console.error(err)
    else console.log(value)
  })

  rcib.postDelayedTask(path.join(__dirname, './background.js'), 'globalFunction', 
  JSON.stringify({a:100, b: 200}), 1000, function (err, value){
    if(err) console.error(err)
    else console.log(value)
  })
}, 500)

process.on('SIGINT', function(){
  clearInterval(loop)
})

process.on('SIGHUP', function(){
  clearInterval(loop)
})
