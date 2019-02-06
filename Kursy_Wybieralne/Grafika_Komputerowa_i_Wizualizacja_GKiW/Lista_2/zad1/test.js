/* vertex shader source code */
var vertexShaderSrc= ""+
    "attribute vec4 aVertexPosition; \n"+
    "uniform vec3 uMove; \n"+
    "void main( void ) { \n"+
    "  gl_PointSize=5.0; \n"+
    "  gl_Position= aVertexPosition+ vec4( uMove, 0); \n"+
    "} \n";

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

var dataInit= function(){
    data={};
    data.background=[0,0,0,1];

    data.objects = [];
    
    var object1 = {};
    object1.position=[0,0,0];
    object1.colorRGB=[0, 1.0, 0];
    object1.bufferId = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, object1.bufferId );
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([-0.95,0.95, -0.8, 0.8]) , gl.STATIC_DRAW ); // load object's shape
    object1.floatsPerVertex=2;
    object1.NumberOfVertices=2;
    object1.drawMode=gl.POINTS;
    data.objects.push(object1);
    
    
    var object2 = {};
    object2.position=[0,0,0];
    object2.colorRGB=[0, 1.0, 1.0];
    object2.bufferId = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, object2.bufferId );
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([-0.65,0.95, -0.6, 0.75, -0.35, 0.7]) , gl.STATIC_DRAW ); // load object's shape
    object2.floatsPerVertex=2;
    object2.NumberOfVertices=3;
    object2.drawMode=gl.LINE_STRIP;
    data.objects.push(object2);
    
    var object3 = {};
    object3.position=[0,0,0];
    object3.colorRGB=[1.0, 1.0, 1.0];
    object3.bufferId = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, object3.bufferId );
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([0,0.95, 0.3, 0.95, 0.2, 0.7]) , gl.STATIC_DRAW ); // load object's shape
    object3.floatsPerVertex=2;
    object3.NumberOfVertices=3;
    object3.drawMode=gl.LINE_LOOP;
    data.objects.push(object3);
    
    var object4 = {};
    object4.position=[0,0,0];
    object4.colorRGB=[1.0, 0, 0];
    object4.bufferId = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, object4.bufferId );
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([0.5, 0.95, 
                                                    0.7, 0.95, 
                                                    0.5, 0.7, 
                                                    0.7, 0.7]) , gl.STATIC_DRAW ); // load object's shape
    object4.floatsPerVertex=2;
    object4.NumberOfVertices=4;
    object4.drawMode=gl.LINES;
    data.objects.push(object4);
    
    var object5 = {};
    object5.position=[0,0,0];
    object5.colorRGB=[1.0, 0, 0];
    object5.bufferId = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, object5.bufferId );
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([-0.7, 0, 
                                                    -0.7, -0.5, 
                                                    -0.5, -0.6, 
                                                    -0.8, -0.8]) , gl.STATIC_DRAW ); // load object's shape
    object5.floatsPerVertex=2;
    object5.NumberOfVertices=4;
    object5.drawMode=gl. TRIANGLE_STRIP;
    data.objects.push(object5);

    var object6 = {};
    object6.position=[0,0,0];
    object6.colorRGB=[0, 0, 1.0];
    object6.bufferId = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, object6.bufferId );
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([0, 0,
                                                     0, 0.2,
                                                     0.1, 0.1,
                                                     0.2, 0.05,
                                                     0.3, -0.1,
                                                     0.2, -0.2,
                                                     0.1, -0.2]) , gl.STATIC_DRAW ); // load object's shape
    object6.floatsPerVertex=2;
    object6.NumberOfVertices=7;
    object6.drawMode=gl. TRIANGLE_FAN;
    data.objects.push(object6);
    
     var object7 = {};
    object7.position=[0,0,0];
    object7.colorRGB=[1.0, 1.0, 0];
    object7.bufferId = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, object7.bufferId );
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([0.5, -0.7,
                                                     0.7, -0.9,
                                                     0.8, -0.8,
                                                     0.5, 0,
                                                     0.5, -0.4,
                                                     0.9, -0.4]) , gl.STATIC_DRAW ); // load object's shape
    object7.floatsPerVertex=2;
    object7.NumberOfVertices=6;
    object7.drawMode=gl.TRIANGLES;
    data.objects.push(object7);
}

var drawObject=function( obj ) {
    /* draw object obj */
    gl.useProgram( glObjects.shaderProgram );
    gl.lineWidth(5);
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
    /* draw objects */
    for (var i = 0; i < data.objects.length; i++)
        drawObject(data.objects[i]);
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
        gl.bindAttribLocation(shaderProgram, 1, "aVertexPosition");
    gl.linkProgram(shaderProgram);
    if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
        console.log("Could not initialise shaders");
        console.log(gl);
        return null;
    }
    // SUCCESS 
    return shaderProgram;
};



window.onload= function(){
    htmlInit();
    glInit( html.canvas );
    dataInit();
    redraw(); 
    
    const numAttribs = gl.getProgramParameter(glObjects.shaderProgram, gl.ACTIVE_ATTRIBUTES);
    for (let i = 0; i < numAttribs; ++i) {
      const info = gl.getActiveAttrib(glObjects.shaderProgram, i); 
      console.log('Attribut name:', info.name, 'type:', info.type, 'size:', info.size); 
    }

    const numUniforms = gl.getProgramParameter(glObjects.shaderProgram, gl.ACTIVE_UNIFORMS);
    for (let i = 0; i < numUniforms; ++i) {
      const info = gl.getActiveUniform(glObjects.shaderProgram, i); 
      console.log('Uniform name:', info.name, 'type:', info.type, 'size:', info.size); 
    }
    console.log("Attibut aVertexPosition location= "+glObjects.aVertexPositionLocation);
};