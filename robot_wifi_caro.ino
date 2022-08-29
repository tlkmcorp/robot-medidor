 
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h> 



#define M1A 16 //Motor 1A D0
#define M1B 5 //Motor 1B  D1
#define M2A 4 //Motor 2A  D2
#define M2B 0 //Motor 2B  D3


//medidor de distancia
#define TRIGGER 14  //Pin digital D5 para el Trigger del sensor (salida)
#define ECHO 12     //Pin digital D6 para el Echo del sensor    (entrada)


unsigned long previousMillisSensor = 0;        // guarda el ultimo momento de update
long intervalSensor = 2000;           // intervalo

const char *ssid_AP = "ESP8266_AP";
const char *password_AP = "12345678";

ESP8266WebServer server(80); 





void setup() { 
     Serial.begin(9600);

     pinMode(M1A, OUTPUT);
     pinMode(M1B, OUTPUT);
     pinMode(M2A, OUTPUT);
     pinMode(M2B, OUTPUT);

     pinMode(TRIGGER, OUTPUT); //pin como salida
     pinMode(ECHO, INPUT);  //pin como entrada
     digitalWrite(TRIGGER, LOW);//Inicializamos el pin con 0


      WiFi.mode(WIFI_AP); 
      WiFi.softAP(ssid_AP, password_AP);

      WiFi.printDiag(Serial); 
      Serial.println(); 
  
      Serial.print("AP dirección IP: ");       
      Serial.println(WiFi.softAPIP()); 

      server.on("/", root);
      server.on("/direccion", direccion);
      server.on("/medicion", medicion);


 
      // Ruteo para URI desconocida
      server.onNotFound(handleNotFound);

  
      server.begin();       
      Serial.println("Servidor inicializado."); 
      
 
}     


void loop() {
   server.handleClient();

}//end loop


void handleNotFound() 
{
   server.send(404, "text/plain", "Not found");
}

void direccion() 
{
 String orden="";
  for (int i = 0; i < server.args(); i++) 
  {
      //Serial.print(server.argName(i));
      //Serial.print("-");
      //Serial.println(server.arg(i));
      
      if (server.argName(i).equals("dir"))
      {
        orden=server.arg(i);
        envia_orden(orden);
  	  }
  }  

  String cadena = orden;      
  server.send(200, "text/html",cadena);
}

void medicion() 
{  

  String cadena = calculoMedicion();      
  server.send(200, "text/html",cadena);
}


String calculoMedicion() 
{

  long t; //timepo que demora en llegar el eco
  long d; //distancia
  
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10);       //Enviamos un pulso de 10us
  digitalWrite(TRIGGER, LOW);
  
  t = pulseIn(ECHO, HIGH); //obtenemos el ancho del pulso
  d= t/59;             //escalamos el tiempo a una distancia en cm

  Serial.print("Distancia: ");
  Serial.print(d);      //Enviamos serialmente el valor de la distancia
  Serial.print("cm");
  Serial.println();

  String cadena = String(d);   

  return cadena;
}




void envia_orden(String valor){

  //N, NE, E, SE, S, SW, W, NW y C  
  if (valor.equals("N")){
    ADE();    
  }else if (valor.equals("S")){
    ATR();    
  }else if (valor.equals("E")){
    DER();    
  }else if (valor.equals("W")){
    IZQ();    
  }else if (valor.equals("NE")){
    ADE_DER();    
  }else if (valor.equals("NW")){
    ADE_IZQ();    
  }else if (valor.equals("SE")){
    ATR_DER();    
  }else if (valor.equals("SW")){
    ATR_IZQ();    
  }else if (valor.equals("C")){
    STOP();    
  }

  Serial.print("valor ejecutado: ");Serial.println(valor);  
}

void ADE(){
 digitalWrite(M1A, LOW);
 digitalWrite(M1B, HIGH);
 digitalWrite(M2A, LOW);
 digitalWrite(M2B, HIGH);
 }

 void ATR(){
 digitalWrite(M1A, HIGH);
 digitalWrite(M1B, LOW);
 digitalWrite(M2A, HIGH);
 digitalWrite(M2B, LOW);
 }
 
 
 void IZQ(){
 //rueda Izquierda
 digitalWrite(M1A, HIGH);
 digitalWrite(M1B, LOW);
//rueda Derecha
 digitalWrite(M2A, LOW);
 digitalWrite(M2B, HIGH);
 }

 void ADE_IZQ(){
 //rueda Izquierda
 digitalWrite(M1A, LOW);
 digitalWrite(M1B, LOW);
 //rueda Derecha
 digitalWrite(M2A, LOW);
 digitalWrite(M2B, HIGH);
 }

  void ATR_IZQ(){
 //rueda Izquierda
 digitalWrite(M1A, HIGH);
 digitalWrite(M1B, LOW);
//rueda Derecha
 digitalWrite(M2A, LOW);
 digitalWrite(M2B, LOW);
 }
 
 
 void DER(){
 //rueda Izquierda
 digitalWrite(M1A, LOW);
 digitalWrite(M1B, HIGH);
 //rueda Derecha
 digitalWrite(M2A, HIGH);
 digitalWrite(M2B, LOW);
 }


 void ADE_DER(){
 //rueda Izquierda
 digitalWrite(M1A, LOW);
 digitalWrite(M1B, HIGH);
 //rueda Derecha
 digitalWrite(M2A, LOW);
 digitalWrite(M2B, LOW);
 }

  void ATR_DER(){
 //rueda Izquierda
 digitalWrite(M1A, LOW);
 digitalWrite(M1B, LOW);
 //rueda Derecha
 digitalWrite(M2A, HIGH);
 digitalWrite(M2B, LOW);
 }
 
 
 void STOP(){
 digitalWrite(M1A, LOW);
 digitalWrite(M1B, LOW);
 digitalWrite(M2A, LOW);
 digitalWrite(M2B, LOW);
}





void root() 
{
  
String cadena = R"=====(
 <html>
	<head>
		<title>Mando Robot</title>
		<meta charset="utf-8">
<style>
*
{
	box-sizing: border-box;
}
body
{
	margin: 0px;
	padding: 0px;
	font-family: monospace;
}
.row
{
	display: inline-flex;
	clear: both;
}
.columnLateral
{
  float: left;
  width: 15%;
  min-width: 300px;
}
.columnCetral
{
  float: left;
  width: 70%;
  min-width: 300px;
}

#joystick
{
	border: 1px solid #FF0000;
}

</style>
<script>
/*
 * Name          : joy.js
 * @author       : Roberto D'Amico (Bobboteck)
 * Last modified : 09.06.2020
 * Revision      : 1.1.6
 *
 * Modification History:
 * Date         Version     Modified By     Description
 * 2021-12-21   2.0.0       Roberto D'Amico New version of the project that integrates the callback functions, while 
 *                                          maintaining compatibility with previous versions. Fixed Issue #27 too, 
 *                                          thanks to @artisticfox8 for the suggestion.
 * 2020-06-09   1.1.6       Roberto D'Amico Fixed Issue #10 and #11
 * 2020-04-20   1.1.5       Roberto D'Amico Correct: Two sticks in a row, thanks to @liamw9534 for the suggestion
 * 2020-04-03               Roberto D'Amico Correct: InternalRadius when change the size of canvas, thanks to 
 *                                          @vanslipon for the suggestion
 * 2020-01-07   1.1.4       Roberto D'Amico Close #6 by implementing a new parameter to set the functionality of 
 *                                          auto-return to 0 position
 * 2019-11-18   1.1.3       Roberto D'Amico Close #5 correct indication of East direction
 * 2019-11-12   1.1.2       Roberto D'Amico Removed Fix #4 incorrectly introduced and restored operation with touch 
 *                                          devices
 * 2019-11-12   1.1.1       Roberto D'Amico Fixed Issue #4 - Now JoyStick work in any position in the page, not only 
 *                                          at 0,0
 * 
 * The MIT License (MIT)
 *
 *  This file is part of the JoyStick Project (https://github.com/bobboteck/JoyStick).
 *	Copyright (c) 2015 Roberto D'Amico (Bobboteck).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


let StickStatus =
{
    xPosition: 0,
    yPosition: 0,
    x: 0,
    y: 0,
    cardinalDirection: "C"
};

/**
 * @desc Principal object that draw a joystick, you only need to initialize the object and suggest the HTML container
 * @costructor
 * @param container {String} - HTML object that contains the Joystick
 * @param parameters (optional) - object with following keys:
 *  title {String} (optional) - The ID of canvas (Default value is 'joystick')
 *  width {Int} (optional) - The width of canvas, if not specified is setted at width of container object (Default value is the width of container object)
 *  height {Int} (optional) - The height of canvas, if not specified is setted at height of container object (Default value is the height of container object)
 *  internalFillColor {String} (optional) - Internal color of Stick (Default value is '#00AA00')
 *  internalLineWidth {Int} (optional) - Border width of Stick (Default value is 2)
 *  internalStrokeColor {String}(optional) - Border color of Stick (Default value is '#003300')
 *  externalLineWidth {Int} (optional) - External reference circonference width (Default value is 2)
 *  externalStrokeColor {String} (optional) - External reference circonference color (Default value is '#008000')
 *  autoReturnToCenter {Bool} (optional) - Sets the behavior of the stick, whether or not, it should return to zero position when released (Default value is True and return to zero)
 * @param callback {StickStatus} - 
 */
var JoyStick = (function(container, parameters, callback)
{
    parameters = parameters || {};
    var title = (typeof parameters.title === "undefined" ? "joystick" : parameters.title),
        width = (typeof parameters.width === "undefined" ? 0 : parameters.width),
        height = (typeof parameters.height === "undefined" ? 0 : parameters.height),
        internalFillColor = (typeof parameters.internalFillColor === "undefined" ? "#00AA00" : parameters.internalFillColor),
        internalLineWidth = (typeof parameters.internalLineWidth === "undefined" ? 2 : parameters.internalLineWidth),
        internalStrokeColor = (typeof parameters.internalStrokeColor === "undefined" ? "#003300" : parameters.internalStrokeColor),
        externalLineWidth = (typeof parameters.externalLineWidth === "undefined" ? 2 : parameters.externalLineWidth),
        externalStrokeColor = (typeof parameters.externalStrokeColor ===  "undefined" ? "#008000" : parameters.externalStrokeColor),
        autoReturnToCenter = (typeof parameters.autoReturnToCenter === "undefined" ? true : parameters.autoReturnToCenter);

    callback = callback || function(StickStatus) {};

    // Create Canvas element and add it in the Container object
    var objContainer = document.getElementById(container);
    
    // Fixing Unable to preventDefault inside passive event listener due to target being treated as passive in Chrome [Thanks to https://github.com/artisticfox8 for this suggestion]
    objContainer.style.touchAction = "none";

    var canvas = document.createElement("canvas");
    canvas.id = title;
    if(width === 0) { width = objContainer.clientWidth; }
    if(height === 0) { height = objContainer.clientHeight; }
    canvas.width = width;
    canvas.height = height;
    objContainer.appendChild(canvas);
    var context=canvas.getContext("2d");

    var pressed = 0; // Bool - 1=Yes - 0=No
    var circumference = 2 * Math.PI;
    var internalRadius = (canvas.width-((canvas.width/2)+10))/2;
    var maxMoveStick = internalRadius + 5;
    var externalRadius = internalRadius + 30;
    var centerX = canvas.width / 2;
    var centerY = canvas.height / 2;
    var directionHorizontalLimitPos = canvas.width / 10;
    var directionHorizontalLimitNeg = directionHorizontalLimitPos * -1;
    var directionVerticalLimitPos = canvas.height / 10;
    var directionVerticalLimitNeg = directionVerticalLimitPos * -1;
    // Used to save current position of stick
    var movedX=centerX;
    var movedY=centerY;

    // Check if the device support the touch or not
    if("ontouchstart" in document.documentElement)
    {
        canvas.addEventListener("touchstart", onTouchStart, false);
        document.addEventListener("touchmove", onTouchMove, false);
        document.addEventListener("touchend", onTouchEnd, false);
    }
    else
    {
        canvas.addEventListener("mousedown", onMouseDown, false);
        document.addEventListener("mousemove", onMouseMove, false);
        document.addEventListener("mouseup", onMouseUp, false);
    }
    // Draw the object
    drawExternal();
    drawInternal();

    /******************************************************
     * Private methods
     *****************************************************/

    /**
     * @desc Draw the external circle used as reference position
     */
    function drawExternal()
    {
        context.beginPath();
        context.arc(centerX, centerY, externalRadius, 0, circumference, false);
        context.lineWidth = externalLineWidth;
        context.strokeStyle = externalStrokeColor;
        context.stroke();
    }

    /**
     * @desc Draw the internal stick in the current position the user have moved it
     */
    function drawInternal()
    {
        context.beginPath();
        if(movedX<internalRadius) { movedX=maxMoveStick; }
        if((movedX+internalRadius) > canvas.width) { movedX = canvas.width-(maxMoveStick); }
        if(movedY<internalRadius) { movedY=maxMoveStick; }
        if((movedY+internalRadius) > canvas.height) { movedY = canvas.height-(maxMoveStick); }
        context.arc(movedX, movedY, internalRadius, 0, circumference, false);
        // create radial gradient
        var grd = context.createRadialGradient(centerX, centerY, 5, centerX, centerY, 200);
        // Light color
        grd.addColorStop(0, internalFillColor);
        // Dark color
        grd.addColorStop(1, internalStrokeColor);
        context.fillStyle = grd;
        context.fill();
        context.lineWidth = internalLineWidth;
        context.strokeStyle = internalStrokeColor;
        context.stroke();
    }

    /**
     * @desc Events for manage touch
     */
    function onTouchStart(event) 
    {
        pressed = 1;
    }

    function onTouchMove(event)
    {
        if(pressed === 1 && event.targetTouches[0].target === canvas)
        {
            movedX = event.targetTouches[0].pageX;
            movedY = event.targetTouches[0].pageY;
            // Manage offset
            if(canvas.offsetParent.tagName.toUpperCase() === "BODY")
            {
                movedX -= canvas.offsetLeft;
                movedY -= canvas.offsetTop;
            }
            else
            {
                movedX -= canvas.offsetParent.offsetLeft;
                movedY -= canvas.offsetParent.offsetTop;
            }
            // Delete canvas
            context.clearRect(0, 0, canvas.width, canvas.height);
            // Redraw object
            drawExternal();
            drawInternal();

            // Set attribute of callback
            StickStatus.xPosition = movedX;
            StickStatus.yPosition = movedY;
            StickStatus.x = (100*((movedX - centerX)/maxMoveStick)).toFixed();
            StickStatus.y = ((100*((movedY - centerY)/maxMoveStick))*-1).toFixed();
            StickStatus.cardinalDirection = getCardinalDirection();
            callback(StickStatus);
        }
    } 

    function onTouchEnd(event) 
    {
        pressed = 0;
        // If required reset position store variable
        if(autoReturnToCenter)
        {
            movedX = centerX;
            movedY = centerY;
        }
        // Delete canvas
        context.clearRect(0, 0, canvas.width, canvas.height);
        // Redraw object
        drawExternal();
        drawInternal();

        // Set attribute of callback
        StickStatus.xPosition = movedX;
        StickStatus.yPosition = movedY;
        StickStatus.x = (100*((movedX - centerX)/maxMoveStick)).toFixed();
        StickStatus.y = ((100*((movedY - centerY)/maxMoveStick))*-1).toFixed();
        StickStatus.cardinalDirection = getCardinalDirection();
        callback(StickStatus);
    }

    /**
     * @desc Events for manage mouse
     */
    function onMouseDown(event) 
    {
        pressed = 1;
    }

    /* To simplify this code there was a new experimental feature here: https://developer.mozilla.org/en-US/docs/Web/API/MouseEvent/offsetX , but it present only in Mouse case not metod presents in Touch case :-( */
    function onMouseMove(event) 
    {
        if(pressed === 1)
        {
            movedX = event.pageX;
            movedY = event.pageY;
            // Manage offset
            if(canvas.offsetParent.tagName.toUpperCase() === "BODY")
            {
                movedX -= canvas.offsetLeft;
                movedY -= canvas.offsetTop;
            }
            else
            {
                movedX -= canvas.offsetParent.offsetLeft;
                movedY -= canvas.offsetParent.offsetTop;
            }
            // Delete canvas
            context.clearRect(0, 0, canvas.width, canvas.height);
            // Redraw object
            drawExternal();
            drawInternal();

            // Set attribute of callback
            StickStatus.xPosition = movedX;
            StickStatus.yPosition = movedY;
            StickStatus.x = (100*((movedX - centerX)/maxMoveStick)).toFixed();
            StickStatus.y = ((100*((movedY - centerY)/maxMoveStick))*-1).toFixed();
            StickStatus.cardinalDirection = getCardinalDirection();
            callback(StickStatus);
        }
    }

    function onMouseUp(event) 
    {
        pressed = 0;
        // If required reset position store variable
        if(autoReturnToCenter)
        {
            movedX = centerX;
            movedY = centerY;
        }
        // Delete canvas
        context.clearRect(0, 0, canvas.width, canvas.height);
        // Redraw object
        drawExternal();
        drawInternal();

        // Set attribute of callback
        StickStatus.xPosition = movedX;
        StickStatus.yPosition = movedY;
        StickStatus.x = (100*((movedX - centerX)/maxMoveStick)).toFixed();
        StickStatus.y = ((100*((movedY - centerY)/maxMoveStick))*-1).toFixed();
        StickStatus.cardinalDirection = getCardinalDirection();
        callback(StickStatus);
    }

    function getCardinalDirection()
    {
        let result = "";
        let horizontal = movedX - centerX;
        let vertical = movedY - centerY;
        
        if(vertical >= directionVerticalLimitNeg && vertical <= directionVerticalLimitPos)
        {
            result = "C";
        }
        if(vertical < directionVerticalLimitNeg)
        {
            result = "N";
        }
        if(vertical > directionVerticalLimitPos)
        {
            result = "S";
        }
        
        if(horizontal < directionHorizontalLimitNeg)
        {
            if(result === "C")
            { 
                result = "W";
            }
            else
            {
                result += "W";
            }
        }
        if(horizontal > directionHorizontalLimitPos)
        {
            if(result === "C")
            { 
                result = "E";
            }
            else
            {
                result += "E";
            }
        }
        
        return result;
    }

    /******************************************************
     * Public methods
     *****************************************************/

    /**
     * @desc The width of canvas
     * @return Number of pixel width 
     */
    this.GetWidth = function () 
    {
        return canvas.width;
    };

    /**
     * @desc The height of canvas
     * @return Number of pixel height
     */
    this.GetHeight = function () 
    {
        return canvas.height;
    };

    /**
     * @desc The X position of the cursor relative to the canvas that contains it and to its dimensions
     * @return Number that indicate relative position
     */
    this.GetPosX = function ()
    {
        return movedX;
    };

    /**
     * @desc The Y position of the cursor relative to the canvas that contains it and to its dimensions
     * @return Number that indicate relative position
     */
    this.GetPosY = function ()
    {
        return movedY;
    };

    /**
     * @desc Normalizzed value of X move of stick
     * @return Integer from -100 to +100
     */
    this.GetX = function ()
    {
        return (100*((movedX - centerX)/maxMoveStick)).toFixed();
    };

    /**
     * @desc Normalizzed value of Y move of stick
     * @return Integer from -100 to +100
     */
    this.GetY = function ()
    {
        return ((100*((movedY - centerY)/maxMoveStick))*-1).toFixed();
    };

    /**
     * @desc Get the direction of the cursor as a string that indicates the cardinal points where this is oriented
     * @return String of cardinal point N, NE, E, SE, S, SW, W, NW and C when it is placed in the center
     */
    this.GetDir = function()
    {
        return getCardinalDirection();
    };
});

  var ajax_dir;
  var ajax_med;
  
  function funcionCallbackDir()
  {
  	// Comprobamos si la peticion se ha completado (estado 4)
  	if( ajax_dir.readyState == 4 )
  	{
  		// Comprobamos si la respuesta ha sido correcta (resultado HTTP 200)
  		if( ajax_dir.status == 200 )
  		{
  			var cadena = ajax_dir.responseText;
  		}//fin if status
  	}//fin ajax ready
  }

  function enviaDireccion(dir)
  {
  	// Creamos el control XMLHttpRequest segun el navegador en el que estemos 
  	if( window.XMLHttpRequest )
  		ajax_dir = new XMLHttpRequest(); // No Internet Explorer
  	else
  		ajax_dir = new ActiveXObject("Microsoft.XMLHTTP"); // Internet Explorer
  
  	// Almacenamos en el control al funcion que se invocara cuando la peticion
  	// cambie de estado	
  	ajax_dir.onreadystatechange = funcionCallbackDir;
  
  	// Enviamos la peticion
  	ajax_dir.open( "GET", "direccion?dir="+dir, true );
  	ajax_dir.send("");
  }

  function funcionCallbackMed()
  {
    // Comprobamos si la peticion se ha completado (estado 4)
    if( ajax_med.readyState == 4 )
    {
      // Comprobamos si la respuesta ha sido correcta (resultado HTTP 200)
      if( ajax_med.status == 200 )
      {
        var cadena = ajax_med.responseText;
        document.getElementById("obstaculo").value=cadena;
      }//fin if status
    }//fin ajax ready
  }

  function medicion()
  {
    // Creamos el control XMLHttpRequest segun el navegador en el que estemos 
    if( window.XMLHttpRequest )
      ajax_med = new XMLHttpRequest(); // No Internet Explorer
    else
      ajax_med = new ActiveXObject("Microsoft.XMLHTTP"); // Internet Explorer
  
    // Almacenamos en el control al funcion que se invocara cuando la peticion
    // cambie de estado 
    ajax_med.onreadystatechange = funcionCallbackMed;
  
    // Enviamos la peticion
    ajax_med.open( "GET", "medicion?rand="+(new Date()).getTime(), true );
    ajax_med.send("");
  }


		</script>
	</head>
	<body>

<div id="joy1Div" style="width:500px;height:500px;margin:50px;position: absolute;left: 10%;top: 10%;">
 Dirección <input id="joy1Dir" readonly type="text" /><br/>
 Obstáculo (cm) <input id="obstaculo" readonly type="text" /> <button onclick="medicion()">Obtener distancia</button>
</div>

		<script type="text/javascript">

    var joy1Dir = document.getElementById("joy1Dir");
    var joy1OldDir ="C";
    
    // Create JoyStick object into the DIV 'joy1Div'    
    var joy1Param = {};
    var Joy1 = new JoyStick('joy1Div', joy1Param, function(stickData) {
    joy1Dir.value = stickData.cardinalDirection;
      if (joy1Dir.value!=joy1OldDir){
      	    joy1OldDir = joy1Dir.value;
      		//alert(joy1Dir.value);
      		enviaDireccion(joy1Dir.value);
      }	
    });



		</script>
	</body>
</html>
)=====";


   
server.send(200, "text/html",cadena);
}
