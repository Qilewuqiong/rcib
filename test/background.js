

var globalFunction = function(v){

	return 'test memory using'
}


var globalFunction2 = function(v){
	var obj
	try {
	   obj = JSON.parse(v)
	}
	
	catch(e){
		return 'err'
	}

	var a = obj.a
	var b = obj.b

	var i
	// simulate CPU intensive process...
	for(i = 0; i < 95550000; ++i) {
		a += b
		a -= b
	}
	return '95550000 addition and subtraction cycles'
}