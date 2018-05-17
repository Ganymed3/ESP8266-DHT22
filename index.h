const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
  <title>ESP8266 Web Server</title>
 <style>
  body { background-color: #000000; font-family: Arial, Helvetica, Sans-Serif; Color: #00ff00; font-size: 50px } 
  table, th, td { border: 1px solid gray } 
  #TempDesc      {Color: #ff5050;}
  #HumidityDesc  {Color: #6699ff;}
  #TempValue     {Color: #ff5050; font-weight: bold;}
  #HumidityValue {Color: #6699ff; font-weight: bold;}
 </style>
</head>
<body>

<table border='1'>
 <tr>
  <td><span id="TempDesc">Temperature [*C]:</span></td><td><span id="TempValue">0</span></td>
 </tr>
 <tr>
  <td><span id="HumidityDesc">Humidity [%]:</span></td><td><span id="HumidityValue">0</span></td>
 </tr>
 <tr>
  <td>IP (Station):</td><td><span id="IPsta">0</span></td>
 </tr>
 <tr>
  <td>IP (SW AP):</td><td><span id="IPap">0</span></td>
 </tr>
 <tr>
  <td>IP (Public):</td><td><span id="IPpub">0</span></td>
 </tr>
 <!--/*<tr>
  <td>WiFi channel:</td><td><b><span id="WifiChannel">0</span></b></td>
 </tr>*/-->
</table>
<!-- /*
<div id="channels">
  <button type="button" onclick="sendData(0)">0</button>
  <button type="button" onclick="sendData(1)">1</button>
  <button type="button" onclick="sendData(2)">2</button>
  <button type="button" onclick="sendData(3)">3</button>
  <button type="button" onclick="sendData(4)">4</button>
  <button type="button" onclick="sendData(5)">5</button>
  <button type="button" onclick="sendData(6)">6</button>
  <button type="button" onclick="sendData(7)">7</button>
  <button type="button" onclick="sendData(8)">8</button>
  <button type="button" onclick="sendData(9)">9</button>
  <button type="button" onclick="sendData(10)">10</button>
  <button type="button" onclick="sendData(11)">11</button>
  <button type="button" onclick="sendData(12)">12</button>
  <button type="button" onclick="sendData(13)">13</button>
  <br>
</div>
*/-->
<script>

function sendData(ch) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == XMLHttpRequest.DONE && this.status == 200) {
      document.getElementById("WifiChannel").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "setChannel?ChannelValue="+ch, true);
  xhttp.send();
}

setInterval(function() {
  // Call a function repetatively with 4 Second interval
  getData();
}, 4000); // 4000mSeconds update rate

function getData() {
  var xhttp = new XMLHttpRequest();
  
  xhttp.onreadystatechange = function() {
    if (this.readyState == XMLHttpRequest.DONE && this.status == 200) {
      var responseArray = this.responseText.split(";");
      document.getElementById("TempValue").innerHTML = responseArray[0];
      document.getElementById("HumidityValue").innerHTML = responseArray[1];
      document.getElementById("IPsta").innerHTML = responseArray[2];
      document.getElementById("IPap").innerHTML = responseArray[3];
      document.getElementById("IPpub").innerHTML = responseArray[4];
    }
  };
  xhttp.open("GET", "readTemp", true);
  xhttp.send();
}
</script>
<br>
</body>
</html>
)=====";

