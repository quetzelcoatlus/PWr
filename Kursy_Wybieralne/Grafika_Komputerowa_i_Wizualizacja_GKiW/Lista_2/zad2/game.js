/* SHADER PROGRAM */
/* vertex shader source code */
var vertexShaderSrc= ""+
    "attribute vec4 aVertexPosition; \n"+
    "uniform vec3 uMove; \n"+
    "void main( void ) { \n";
   

/* fragment shader source code */
var fragmentShaderSrc= ""+
    "precision mediump float; \n"+ 
    "uniform vec3 uColorRGB; \n"+ 
    "void main( void ) { \n"+
    "  gl_FragColor = vec4( uColorRGB, 1.0 ); \n"+
    "} \n";



var gl; // GL context
var glObjects; // references to various GL objects
var html; // HTML objects
var data; // user data
var size = 15;  
var delta = 100;
var end = false;

var dataInit= function(){
    data={};
    data.background=[0,0,0,1];
    data.speed = 2 / size;
    data.snake = [];
    
    /* animated object */
    var head={};
    head.direction= [0,0,0];
    head.prevDirection= [1,0,0];
    // parameters for drawObject
    head.colorRGB=[1.0, 1.0, 0];
    var mid = Math.floor(size/2);
    head.position = [-1 + data.speed*(mid+0.5),-1 + data.speed*(mid+0.5),-1];
    head.bufferId = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, head.bufferId );
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([0,0]) , gl.STATIC_DRAW ); // load head's shape
    head.floatsPerVertex=2;
    head.NumberOfVertices=1;
    head.drawMode=gl.POINTS;
    
    data.snake.push(head);
    
    data.food={};
    // parameters for drawObject
    data.food.colorRGB=[1.0, 0, 0];
    var randx = Math.floor(Math.random()*size);
    var randy = Math.floor(Math.random()*size);
    if (randx > size-1)
        randx = size-1;
     if (randy > size-1)
        randy = size-1;
    data.food.position=[-1 + data.speed*(randx+0.5), -1 + data.speed*(randy+0.5), 0];
    data.food.bufferId = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER,  data.food.bufferId );
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([0,0]) , gl.STATIC_DRAW ); // load object's shape
    data.food.floatsPerVertex=2;
    data.food.NumberOfVertices=1;
    data.food.drawMode=gl.POINTS;
    
    /* animation */
    data.animation={};
    data.animation.requestId=0;

    
    data.kot=[];
    var kolorKota = [0.7, 0.7, 0.7];
    var kolorKota2 = [0.2, 0.2, 0.2];
    var kolorKota3 = [0, 0.1, 0.8];
    
    var object2 = {};
    // parameters for drawObject
    object2.position=[0,0, 0.7];
    object2.colorRGB=kolorKota2;
    object2.bufferId = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, object2.bufferId );
    gl.bufferData(gl.ARRAY_BUFFER, 
		  new Float32Array([ -0.94,  0.75, 
				      -0.94,  -0.02, 
				      -0.59,  0.38] ) , gl.STATIC_DRAW ); // load object's shape
    object2.floatsPerVertex=2;
    object2.NumberOfVertices=3;
    object2.drawMode=gl.TRIANGLE_FAN;
    data.kot.push(object2);
    
    var object3 = {};
    object3.position=[0,0, 0.7];
    object3.colorRGB=kolorKota2;
    object3.bufferId = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, object3.bufferId );
    gl.bufferData(gl.ARRAY_BUFFER, 
		  new Float32Array([ -0.59,  0.38, 
				      -0.22,  0.75, 
				      -0.22,  -0.02] ) , gl.STATIC_DRAW ); // load object's shape
   object3.floatsPerVertex=2;
    object3.NumberOfVertices=3;
    object3.drawMode=gl.TRIANGLE_FAN;
    data.kot.push(object3);
    
    var object4 = {};
    object4.position=[0,0, 0.7];
    object4.colorRGB=kolorKota;
    object4.bufferId = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, object4.bufferId );
    gl.bufferData(gl.ARRAY_BUFFER, 
		  new Float32Array([ -0.59,  0.38, 
				      -0.22,  -0.02, 
                      -0.59,  -0.38,
				      -0.94,  -0.02] ) , gl.STATIC_DRAW ); // load object's shape
   object4.floatsPerVertex=2;
    object4.NumberOfVertices=4;
    object4.drawMode=gl.TRIANGLE_FAN;
    data.kot.push(object4);
    
    var object5 = {};
    object5.position=[0,0, 0.7];
    object5.colorRGB=kolorKota;
    object5.bufferId = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, object5.bufferId );
    gl.bufferData(gl.ARRAY_BUFFER, 
		  new Float32Array([ -0.22,  -0.02, 
                      -0.59,  -0.38,
                      -0.22,  -0.76,
                      0.16,  -0.38] ) , gl.STATIC_DRAW ); // load object's shape
   object5.floatsPerVertex=2;
    object5.NumberOfVertices=4;
    object5.drawMode=gl.TRIANGLE_FAN;
    data.kot.push(object5);
    
    var object5 = {};
    object5.position=[0,0, 0.7];
    object5.colorRGB=kolorKota2;
    object5.bufferId = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, object5.bufferId );
    gl.bufferData(gl.ARRAY_BUFFER, 
		  new Float32Array([ -0.59,  -0.38,
                      -0.22,  -0.76, 
				      -0.94,  -0.76] ) , gl.STATIC_DRAW ); // load object's shape
   object5.floatsPerVertex=2;
    object5.NumberOfVertices=3;
    object5.drawMode=gl.TRIANGLE_FAN;
    data.kot.push(object5);
    
     var object6 = {};
    object6.position=[0,0, 0.7];
    object6.colorRGB=kolorKota2;
    object6.bufferId = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, object6.bufferId );
    gl.bufferData(gl.ARRAY_BUFFER, 
		  new Float32Array([ -0.22,  -0.76,
                      0.16,  -0.38, 
				      0.53,  -0.76] ) , gl.STATIC_DRAW ); // load object's shape
   object6.floatsPerVertex=2;
    object6.NumberOfVertices=3;
    object6.drawMode=gl.TRIANGLE_FAN;
    data.kot.push(object6);
    
    var object7 = {};
    object7.position=[0,0, 0.7];
    object7.colorRGB=kolorKota;
    object7.bufferId = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, object7.bufferId );
    gl.bufferData(gl.ARRAY_BUFFER, 
		  new Float32Array([ -0.22,  -0.02, 
                      0.16,  -0.38,
                      0.53, -0.02,
                      0.53,  -0.76] ) , gl.STATIC_DRAW ); // load object's shape
   object7.floatsPerVertex=2;
    object7.NumberOfVertices=4;
    object7.drawMode=gl.TRIANGLE_FAN;
    data.kot.push(object7);
    
    var object8 = {};
    object8.position=[0,0, 0.7];
    object8.colorRGB=kolorKota;
    object8.bufferId = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, object8.bufferId );
    gl.bufferData(gl.ARRAY_BUFFER, 
		  new Float32Array([  0.53,  -0.76,
                      0.53, -0.02,
                      0.87, -0.38] ) , gl.STATIC_DRAW ); // load object's shape
   object8.floatsPerVertex=2;
    object8.NumberOfVertices=3;
    object8.drawMode=gl.TRIANGLE_FAN;
    data.kot.push(object8);
    
     var object9 = {};
    object9.position=[0,0, 0.7];
    object9.colorRGB=kolorKota;
    object9.bufferId = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, object9.bufferId );
    gl.bufferData(gl.ARRAY_BUFFER, 
		  new Float32Array([  0.53,  -0.02,
                      0.87, -0.38,
                      0.87, 0.38] ) , gl.STATIC_DRAW ); // load object's shape
   object9.floatsPerVertex=2;
    object9.NumberOfVertices=3;
    object9.drawMode=gl.TRIANGLE_FAN;
    data.kot.push(object9);
    
    var object10 = {};
    object10.position=[0,0, 0.7];
    object10.colorRGB=kolorKota2;
    object10.bufferId = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, object10.bufferId );
    gl.bufferData(gl.ARRAY_BUFFER, 
		  new Float32Array([  0.53,  -0.02,
                      0.87, 0.38,
                       0.53,  0.75] ) , gl.STATIC_DRAW ); // load object's shape
   object10.floatsPerVertex=2;
    object10.NumberOfVertices=3;
    object10.drawMode=gl.TRIANGLE_FAN;
    data.kot.push(object10);
    
    var object11 = {};
    object11.position=[0,0, 0.6];
    object11.colorRGB=kolorKota3;
    object11.bufferId = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, object11.bufferId );
    gl.bufferData(gl.ARRAY_BUFFER, 
		  new Float32Array([  -0.75,  0.11,
                      -0.62,  0.11,
                       -0.69, 0.05] ) , gl.STATIC_DRAW ); // load object's shape
   object11.floatsPerVertex=2;
    object11.NumberOfVertices=3;
    object11.drawMode=gl.TRIANGLE_FAN;
    data.kot.push(object11);
    
    var object12 = {};
    object12.position=[0,0, 0.6];
    object12.colorRGB=kolorKota3;
    object12.bufferId = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, object12.bufferId );
    gl.bufferData(gl.ARRAY_BUFFER, 
		  new Float32Array([  -0.53,  0.11,
                      -0.40,  0.11,
                       -0.46, 0.05] ) , gl.STATIC_DRAW ); // load object's shape
   object12.floatsPerVertex=2;
    object12.NumberOfVertices=3;
    object12.drawMode=gl.TRIANGLE_FAN;
    data.kot.push(object12);
}


var drawObject=function( obj ) {
    /* draw object obj */
    gl.useProgram( glObjects.shaderProgram );
    gl.lineWidth(3);
    gl.enableVertexAttribArray(glObjects.aVertexPositionLocation);
    gl.bindBuffer(gl.ARRAY_BUFFER, obj.bufferId ); /* refer to the buffer */
    gl.vertexAttribPointer(glObjects.aVertexPositionLocation, obj.floatsPerVertex, gl.FLOAT, false, 0 /* stride */, 0 /*offset */);
    gl.uniform3fv( glObjects.uMoveLocation, obj.position );
    gl.uniform3fv( glObjects.uColorRGBLocation, obj.colorRGB );
    gl.drawArrays(obj.drawMode, 0 /* offset */, obj.NumberOfVertices);
}

var redraw = function() {
    var bg = data.background;
    /* prepare clean screen */
    gl.clearColor(bg[0], bg[1], bg[2], bg[3]);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    drawObject(data.food);
    /* draw snake */
    for (var i = 0; i < data.snake.length; i++)
    drawObject(data.snake[i]);

    for (var i = 0; i < data.kot.length; i++)
        drawObject(data.kot[i]);

}

function distance(x, y){
    return Math.sqrt(Math.pow(x[0] - y[0], 2) + Math.pow(x[1] - y[1], 2));
}
function collision(one, two) {
    return (distance(one.position,two.position) < data.speed/2);
}

function collisionWithSnake(one){
    for(var i = 0; i < data.snake.length; i++)
        if(collision(one, data.snake[i]))
            return true;
    return false;
}

function autoCollision(){
    for(var i = 1; i < data.snake.length; i++){
       // console.log(distance(data.snake[0].position, data.snake[i].position));
        if(collision(data.snake[0],data.snake[i])){
            console.log("TRUE");
            return true;
        }
    }
    return false;
}

var animate=function( time ) {
    var timeDelta= time-data.animation.lastTime;
    end = false;
    
    if(timeDelta > delta){
        data.animation.lastTime= time ;
        for (var i = data.snake.length - 1; i > 0; i--){
            data.snake[i].position[0] = data.snake[i-1].position[0];
            data.snake[i].position[1] = data.snake[i-1].position[1];
        }
        var x=  data.snake[0].position[0]+data.snake[0].direction[0]* data.speed;
        var y=  data.snake[0].position[1]+data.snake[0].direction[1]* data.speed;

        
        data.snake[0].prevDirection[0] = data.snake[0].direction[0];
        data.snake[0].prevDirection[1] = data.snake[0].direction[1];

        data.snake[0].position[0]= (x+3)%2 -1;
        data.snake[0].position[1]= (y+3)%2 -1;
         
        redraw();
        gl.finish(); 
        
        if(collision(data.snake[0], data.food)){
            var segment={};
            segment.colorRGB=[0, 1.0, 0];
            //object.position = [data.snake[data.snake.length -1].position[0],data.snake[data.snake.length -1].position[1],0];
            segment.position =[10,10,0];
            segment.bufferId = gl.createBuffer();
            gl.bindBuffer(gl.ARRAY_BUFFER, segment.bufferId );
            gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([0,0]) , gl.STATIC_DRAW ); // load segment's shape
            segment.floatsPerVertex=2;
            segment.NumberOfVertices=1;
            segment.drawMode=gl.POINTS;
            data.snake.push(segment);
            
            while (collisionWithSnake(data.food)){
                var randx = Math.floor(Math.random()*size);
                var randy = Math.floor(Math.random()*size);
                if (randx > size-1)
                    randx = size-1;
                 if (randy > size-1)
                    randy = size-1;
                   data.food.position=[-1 + data.speed*(randx+0.5), -1 + data.speed*(randy+0.5), 0];
            }
        }
            
        if(autoCollision()){
            console.log("ZJADLES SIEBIE");
            animationStop();
            end = true;
            var canvas2 = document.getElementById("can");
            var ctx = canvas2.getContext('2d');
            var pix = canvas2.width/8;
            ctx.font = ""+pix+"px Comic Sans MS";
        //    ctx.fillStyle = "blue"
            var gradient=ctx.createLinearGradient(0,0,canvas2.width,0);
            gradient.addColorStop("0","green");
            gradient.addColorStop("0.5","orange");
            gradient.addColorStop("1.0","red");;
            ctx.fillStyle=gradient;
            ctx.fillText("Game Over",html.canvas.width/5,html.canvas.height/2);
        }
    
    }
    
    if(!end)
        data.animation.requestId = window.requestAnimationFrame(animate);
}

var animationStart= function(){
    data.animation.lastTime = window.performance.now();
    data.animation.requestId = window.requestAnimationFrame(animate);
}

var animationStop= function(){
    if (data.animation.requestId)
	window.cancelAnimationFrame(data.animation.requestId);
    data.animation.requestId = 0;
    redraw();
}

var htmlInit= function() {
    html={};
    html.html=document.querySelector('#htmlId');
    html.canvas= document.querySelector('#canvasId');
};

var glInit= function(canvas) {
    gl = canvas.getContext("experimental-webgl");
    gl.viewportWidth = canvas.width;
    gl.viewportHeight = canvas.height;
    var pixelSize = canvas.width/(1.1*size);
    vertexShaderSrc = vertexShaderSrc +  
        "  gl_PointSize="+pixelSize+"; \n"+
        "  gl_Position= aVertexPosition+ vec4( uMove, 0); \n"+
        "} \n";
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    glObjects={}; 

    /* create executable shader program */
    glObjects.shaderProgram=compileAndLinkShaderProgram( gl, vertexShaderSrc, fragmentShaderSrc );
    /* attributes */
    glObjects.aVertexPositionLocation = gl.getAttribLocation(glObjects.shaderProgram, "aVertexPosition");
    /* uniform variables */ 
    glObjects.uMoveLocation = gl.getUniformLocation(glObjects.shaderProgram, "uMove");
    glObjects.uColorRGBLocation = gl.getUniformLocation(glObjects.shaderProgram, "uColorRGB");

};

var compileAndLinkShaderProgram=function ( gl, vertexShaderSource, fragmentShaderSource ){
    var vertexShader = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShader, vertexShaderSource);
    gl.compileShader(vertexShader);
    if (!gl.getShaderParameter(vertexShader, gl.COMPILE_STATUS)) {
	console.log(gl.getShaderInfoLog(vertexShader));
	console.log(gl);
	return null;
    }

    var fragmentShader =gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShader, fragmentShaderSource);
    gl.compileShader(fragmentShader);
    if (!gl.getShaderParameter(fragmentShader, gl.COMPILE_STATUS)) {
	console.log(gl.getShaderInfoLog(fragmentShader));
	console.log(gl);
	return null;
    }

    var shaderProgram = gl.createProgram();
    gl.attachShader(shaderProgram, vertexShader);
    gl.attachShader(shaderProgram, fragmentShader);
    gl.linkProgram(shaderProgram);
    if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
	console.log("Could not initialise shaders");
	console.log(gl);
	return null;
    }

    // SUCCESS 
    return shaderProgram;
};


var callbackOnKeyDown =function (e){
    var code= e.which || e.keyCode;
    switch(code)
    {
    case 38: // up
    case 73: // I
    case 87: // W
            const numAttribs = gl.getProgramParameter(glObjects.shaderProgram, gl.ACTIVE_ATTRIBUTES);
for (let i = 0; i < numAttribs; ++i) {
  const info = gl.getActiveAttrib(glObjects.shaderProgram, i); 
  console.log('name:', info.name, 'type:', info.type, 'size:', info.size); 
}
        if(data.snake[0].prevDirection[1] != -1)
            data.snake[0].direction=[0,1];
	if( data.animation.requestId == 0 && !end) animationStart();
	break;
    case 40: // down
    case 75: // K
    case 83: // S
        if(data.snake[0].prevDirection[1] != 1)
            data.snake[0].direction=[0,-1];
	if( data.animation.requestId == 0 && !end) animationStart();
	break;
    case 37: // left
    case 74:// J
    case 65: // A
        if(data.snake[0].prevDirection[0] != 1)
            data.snake[0].direction=[-1,0];
	if( data.animation.requestId == 0 && !end) animationStart();
	break;
    case 39:// right
    case 76: // L
    case 68: // D
        if(data.snake[0].prevDirection[0] != -1)
            data.snake[0].direction=[1,0];
	if( data.animation.requestId == 0 && !end) animationStart();
	break;
    }
}

window.onload= function(){
    htmlInit();
    glInit( html.canvas );
    dataInit();


    redraw(); 
    window.onkeydown=callbackOnKeyDown;
};