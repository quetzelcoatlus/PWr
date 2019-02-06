var ctx, angle = Math.PI*2, prevX = 10.0, prevY=10.0, color = "#00ff00", pointes;

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

function koch(x,y,a, count, length) {
	prevX = x;
	prevY = y;
	angle = a;
	pointes="M"+prevX+" "+prevY;
	turn("RIGHT",30);
	line(count,length);
	turn("RIGHT",120);
	line(count,length);
	turn("RIGHT",120);
	line(count,length);
    
    pointes=pointes+" Z";
    
	var svg = document.createElementNS('http://www.w3.org/2000/svg', 'svg');
	svg.setAttribute('width',1100);
	svg.setAttribute('height',1100);
    svg.setAttribute('margin-top',"50px");
	
	var path = document.createElementNS('http://www.w3.org/2000/svg', 'path');
	path.setAttribute('d',pointes);
	path.setAttribute('style',"fill:#aaaaaa;stroke:black;stroke-width:1");

	svg.appendChild(path);
	document.body.appendChild(svg);
}


function draw( action, steps) {
	
	var stepsX = steps * Math.cos(angle);
	var stepsY = steps * Math.sin(angle);


	stepsY *= (-1);

	console.log(stepsX);
	console.log(stepsY);
	console.log(angle * 180 / Math.PI);

	prevX += stepsX;
	prevY += stepsY;
	
	pointes=pointes+" L"+prevX+" "+prevY;
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
}

function exec(){
    var x = parseInt(document.getElementById("x").value);
    var y = parseInt(document.getElementById("y").value);
    var a = parseFloat(document.getElementById("a").value);
    var c = parseInt(document.getElementById("c").value);
    var s = parseInt(document.getElementById("s").value);

    document.getElementById("x").value=""+x+" "+y+" "+a+" "+c+" "+s;
    document.getElementById("y").value="";
    document.getElementById("a").value="";
    document.getElementById("c").value="";
    document.getElementById("s").value="";
    koch(x,y,a,c,s);
}

/*
koch(400,0,-Math.PI/6.0,3,20);        --> gorna os X
koch(0,400,(Math.PI/3.0),5,3);        --> gorna os Y
*/