const char MAIN_page[] = R"=====(
<!DOCTYPE html>
<html>
<body>

<div id="demo">
  <h1>The ESP8266 NodeMCU Update web page without refresh</h1>
  <label>Time Delay</label>
  <input id="TimeDelayClock" value="100"/>
  <label>Steps</label>
  <input id="clockStepsClock" value="10"/>
  <button type="button" onclick="sendData(1)">Turn Clockwise</button>
  <br/>
  <br/>
  <br/>
  <label>Time Delay</label>
  <input id="TimeDelayCount" value="100"/>
  <label>Steps</label>
  <input id="clockStepsCount" value="10"/>
  <button type="button" onclick="sendData(0)">Turn Counter Clock</button><BR>
</div>
  <br/>
  <br/>
  <br/>
<div>
  ADC Value is : <span id="ADCValue">0</span><br>
    LED State is : <span id="LEDState">NA</span>
</div>
<script>
function sendData(rotation) {
  document.getElementById("LEDState").innerHTML = "Processing"
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("LEDState").innerHTML = this.responseText;
    }
  };
  var rotateValue = document.getElementById("clockStepsClock").value;
  var delayValue = document.getElementById("TimeDelayClock").value;
  if(rotation == 0){
    rotateValue = document.getElementById("clockStepsCount").value;
    delayValue = document.getElementById("TimeDelayCount").value;
  }
  xhttp.open("GET", "SetMotorLocation?CLOCK=" + rotation + "&STEPS=" + rotateValue + "&TIMEDELAY=" + delayValue, true);
  xhttp.send();
}

</script>
<br><br>
</body>
</html>
)=====";
