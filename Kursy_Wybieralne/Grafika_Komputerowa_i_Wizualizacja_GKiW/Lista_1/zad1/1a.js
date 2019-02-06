var PIXEL_RATIO = (function () {
    var ctx = document.createElement("canvas").getContext("2d"),
        dpr = window.devicePixelRatio || 1,
        bsr = ctx.webkitBackingStorePixelRatio ||
              ctx.mozBackingStorePixelRatio ||
              ctx.msBackingStorePixelRatio ||
              ctx.oBackingStorePixelRatio ||
              ctx.backingStorePixelRatio || 1;

    return dpr / bsr;
})();


createHiDPICanvas = function(w, h, ratio) {
    if (!ratio) { ratio = PIXEL_RATIO; }
    var can = document.createElement("canvas");
    can.width = w * ratio;
    can.height = h * ratio;
    can.style.width = w + "px";
    can.style.height = h + "px";
    can.getContext("2d").setTransform(ratio, 0, 0, ratio, 0, 0);
    return can;
}

function lex(input) {
	var tokens = [];
	
	for(var i=0; i<input.length; i++) {
		if( isDigit(input[i] ) ) {
			c = extractNum(input, i);
			i += c.len;
			tokens.push({ "type" : "number",
						"value": c.val });
		}
		else if(isChar(input[i] ) ) {
			c = extractChar(input, i);
			i += c.len;
			tokens.push({ "type" : "command",
						  "value": c.val });
		}
		
	}
	return tokens;
}

function isDigit(i) {
	return /[0-9]/.test(i);
}

function extractNum(input, i) {
	var val = 0;
	var len = 0;
	while(i < input.length && isDigit(input[i]) ) {
		val = val*10 + parseInt(input[i]);
		len++;
		i++;
	}
	return {"len" : len,
			"val" : val};
}

function isChar(i) {
	return /[A-Z]/.test(i);
}

function extractChar(input, i) {
	var val = "";
	var len = 0;
	while(i < input.length && isChar(input[i]) ) {
		val = val + input[i];
		len++;
		i++;
	}
	
	return {"len" : len,
			"val" : val};
}

function parse(tokens) {
	var i = 0;
	while(i < tokens.length) {		
		if( tokens[i].type == "command" ) {
			if(tokens[i].value == "FORWARD" || tokens[i].value == "BACK" ) {
				draw(tokens[i].value, tokens[i+1].value);
				i = i+2;
			}
			else if(tokens[i].value == "LEFT" || tokens[i].value == "RIGHT") {
				turn(tokens[i].value, tokens[i+1].value);			
				i = i+2;
			}
			else if(tokens[i].value == "KOCH"){
				koch(tokens[i+1].value, tokens[i+2].value);		
				i = i+3;
			}
			else if(tokens[i].value == "COLOR"){
				color = "rgb("+tokens[i+1].value+","+tokens[i+2].value+","+tokens[i+3].value+")";		
				i = i+4;
			}
            else if(tokens[i].value == "REPEAT"){
                var n = parseInt(tokens[i+1].value);
                i = i+2;
                var ii=i;
                for(var j = 0; j < n; j++){
                    i = ii;
                    while(tokens[i].value != "TAEPER"){
                        if( tokens[i].type == "command" ) {
                            if(tokens[i].value == "FORWARD" || tokens[i].value == "BACK" ) {
                                draw(tokens[i].value, tokens[i+1].value);
                                i = i+2;
                            }
                            else if(tokens[i].value == "LEFT" || tokens[i].value == "RIGHT") {
                                turn(tokens[i].value, tokens[i+1].value);			
                                i = i+2;
                            }
                            else if(tokens[i].value == "KOCH"){
                                koch(tokens[i+1].value, tokens[i+2].value);		
                                i = i+3;
                            }
                            else if(tokens[i].value == "COLOR"){
                                color = "rgb("+tokens[i+1].value+","+tokens[i+2].value+","+tokens[i+3].value+")";		
                                i = i+4;
                            }
                            else {
                                set(tokens[i].value);
                                i++;
                            }
                        }
                        else {
                            i++;
                        }
                    }
                }
            }
			else {
				set(tokens[i].value);
				i++;
			}
		}
		else {
			i++;
		}
	}
}

function line(count,length) {
	if (count == 1)
		draw("FORWARD",length);
	else {
		count--;
		line(count,length);
		turn("LEFT",60);
		line(count,length);
		turn("RIGHT",120);
		line(count,length);
		turn("LEFT",60);
		line(count,length);
	}
}

function koch(count, length) {
	turn("RIGHT",30);
	line(count,length);
	turn("RIGHT",120);
	line(count,length);
	turn("RIGHT",120);
	line(count,length);
    turn("RIGHT",90);
}


function draw( action, steps) {
	
	var stepsX = steps * Math.cos(angle);
	var stepsY = steps * Math.sin(angle);


	if(action == "FORWARD" )
		stepsY *= (-1);

	if (action == "BACK")
			stepsX *= (-1);
	console.log(stepsX);
	console.log(stepsY);
	
	console.log(angle * 180 / Math.PI);
	var moveX = prevX + stepsX;
	var moveY = prevY + stepsY;
	
	
	if( !penup) {
		ctx.beginPath();
		ctx.strokeStyle = color;
		ctx.moveTo(prevX, prevY );
		ctx.lineTo(moveX, moveY);
		ctx.closePath();
		ctx.stroke();
	}
	
	prevX = moveX;
	prevY = moveY;
	drawTurtle();
}

function turn( dir, degrees) {
	var rad = Math.PI * degrees / 180 ;
	if(dir == "RIGHT")
		angle -= rad;
	else
		angle += rad;
	if( angle < 0 )
		angle = 2*Math.PI + angle;
	if (angle > 2*Math.PI)
		angle -= 2*Math.PI;
	drawTurtle();
}

function set(command) {
	if(command == "PENUP")
		setPenUp();
	else if(command == "PENDOWN")
		setPenDown();
}

function run(input) {
	var tokens = lex(input);
	parse(tokens);
}

var canvas, ctx, angle, prevX, prevY, base, penup, color = "#00ff00";

function setPenUp() {
	penup = true;
}
function setPenDown() {
	penup = false;
}
function drawTurtle() {
    var turt = document.getElementById("turtle");
    turt.style.left = prevX;
    turt.style.top = prevY;		

    var turtCtx = turt.getContext("2d");


    turtCtx.clearRect(0, 0, 2* base, 2*base);
    turtCtx.fillStyle = "#000000";


    var turtX, turtY;

    turtX = base * Math.sin(angle);
    turtY = base * Math.cos(angle);
    var X, Y;
    X = base;
    Y = base;


    turtCtx.beginPath();
    turtCtx.moveTo(X - turtX, Y - turtY);
    turtCtx.lineTo(X + turtY, Y - turtX);
    turtCtx.lineTo(X + turtX, Y  + turtY);
    turtCtx.closePath();
    turtCtx.fill();
}

window.onload = function () {
    canvas = createHiDPICanvas(800, 600);
    canvas.id = "canvas";
    canvas.style.marginLeft = window.width/2 + "px";
    canvas.style.border = "solid red 1px";

    ctx = canvas.getContext("2d");
    document.body.appendChild(canvas);


    var turt = createHiDPICanvas(200, 200);
    turt.id = "turtle";

    document.body.appendChild(turt);


    angle = Math.PI/2;
    prevX = 400; prevY = 300;
    base = 10;
    penup = false;
    drawTurtle();
    run("LEFT 45 PENUP FORWARD 250 PENDOWN COLOR 123 20 44 REPEAT 3 FORWARD 100 RIGHT 120 TAEPER");
    run("PENUP LEFT 120 FORWARD 200 COLOR 0 255 0 PENDOWN REPEAT 4 FORWARD 100 LEFT 90 TAEPER");
    run("PENUP LEFT 150 FORWARD 300 COLOR 0 0 255 PENDOWN REPEAT 5 FORWARD 100 RIGHT 72 TAEPER");
    run("PENUP RIGHT 120 FORWARD 250 COLOR 0 0 0 PENDOWN REPEAT 6 FORWARD 100 LEFT 60 TAEPER");
}