const char MAIN_page[] = R"=====(
<!DOCTYPE html>
<html>
<body>

<h1>Berry Home App</h1>
<div id="demo">
  <h3>Turn Based on steps</h3> 
  <label>Time Delay</label>
  <input id="TimeDelayClock" value="1"/>
  <label>Steps</label>
  <input id="clockStepsClock" value="1000"/>
  <button type="button" onclick="sendData(1, 0)">Turn Clockwise</button>
  <br/>
  <br/>
  <br/>
  <label>Time Delay</label>
  <input id="TimeDelayCount" value="1"/>
  <label>Steps</label>
  <input id="clockStepsCount" value="1000"/>
  <button type="button" onclick="sendData(0, 0)">Turn Counter Clock</button><BR>
</div>
<div id="demo2">
  <h3>Full Rotation Turns</h3> 
  <label>Time Delay</label>
  <input id="TimeDelayClock" value="1"/>
  <label>Rotations</label>
  <input id="clockRotationsClock" value="2"/>
  <button type="button" onclick="sendData(1, 1)">Turn Clockwise</button>
  <br/>
  <br/>
  <br/>
  <label>Time Delay</label>
  <input id="TimeDelayCount" value="1"/>
  <label>Rotations</label>
  <input id="clockRotationsCount" value="2"/>
  <button type="button" onclick="sendData(0, 1)">Turn Counter Clock</button><BR>
</div>
<br/>
  <br/>
<div id="openClose">
  <label>Steps Per Rotation</label>
  <input id="StepsPerRotation" value="4096"/>
  <button type="button" onclick="SetStepsPerRot()">Set Steps</button>
  <br/>
  <br/>
  <br/>
  <label>Set Current Location (1 = Up, 2 = Middle)</label>
  <input id="CurrentLocation" value="1"/>
  <button type="button" onclick="SetCurrentLocation()">Set Current Location</button>
  <br/>
  <br/>
  <br/>
  <label>Set Open Close Steps</label>
  <input id="OpenCloseStepsValue" value="0"/>
  <button type="button" onclick="SetOpenCloseSteps()">Set Open Close</button>
</div>
  <br/>
  <br/>
<div id="RunOpenClose">
  <button type="button" onclick="OpenCloseBlinds(0)">Open</button>
  <br/>
  <br/>
  <button type="button" onclick="OpenCloseBlinds(1)">Closed</button>
</div>


  <br/>
  <br/>
  <br/>
<div>
  <label>stepsPerRotation</label> %stepsPerRotation%
  <br/>
  <label>currentLocation</label> %currentLocation%
  <br/>
  <label>OpenCloseSteps</label> %OpenCloseSteps%
  <br/>
</div>

<br/>
<br/>
<div>
    Status <br/> : <span id="LEDState">NA</span>
</div>

<script>


function sendData(rotation, turnType) {
  document.getElementById("LEDState").innerHTML = "Processing"
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("LEDState").innerHTML = this.responseText;
    }
  };
  var rotateValue = document.getElementById("clockStepsClock").value;
  var rotationsValue = document.getElementById("clockRotationsClock").value;
  var delayValue = document.getElementById("TimeDelayClock").value;
  if(rotation == 0){
    rotateValue = document.getElementById("clockStepsCount").value;
    rotationsValue = document.getElementById("clockRotationsClock").value;
    delayValue = document.getElementById("TimeDelayCount").value;
  }
  if(turnType == 0){
    xhttp.open("GET", "SetMotor?CLOCK=" + rotation + "&STEPS=" + rotateValue + "&TIMEDELAY=" + delayValue, true);  
  }
  else
  {
    xhttp.open("GET", "SetMotor?CLOCK=" + rotation + "&ROTATIONS=" + rotationsValue + "&TIMEDELAY=" + delayValue, true);
  }
  
  xhttp.send();
}


function SetStepsPerRot() {
  document.getElementById("LEDState").innerHTML = "Processing"
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("LEDState").innerHTML = this.responseText;
    }
  };
  var rotateValue = document.getElementById("StepsPerRotation").value;
  xhttp.open("GET", "SetStepsPerRotation?STEPS=" + rotateValue + "", true);  
  xhttp.send();
}


function SetOpenCloseSteps() {
  document.getElementById("LEDState").innerHTML = "Processing"
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("LEDState").innerHTML = this.responseText;
    }
  };
  var rotateValue = document.getElementById("OpenCloseStepsValue").value;
  xhttp.open("GET", "SetOpenCloseSteps?STEPS=" + rotateValue + "", true);  
  xhttp.send();
}


function OpenCloseBlinds(openCloseVal) {
  document.getElementById("LEDState").innerHTML = "Processing"
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("LEDState").innerHTML = this.responseText;
    }
  };
  if(openCloseVal == 0) {
    xhttp.open("GET", "SetBlindOpen", true);    
  }
  else {
    xhttp.open("GET", "SetBlindClosed", true);    
  }
  
  xhttp.send();
}

</script>
<br><br>
</body>
</html>
)=====";
