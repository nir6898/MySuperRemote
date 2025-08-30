#include <WiFiS3.h>
#include "wifi_secrets.h"

char ssid[] = SECRET_SSID;  // your network SSID
char pass[] = SECRET_PASS;  // your network password

int status = WL_IDLE_STATUS;
WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  while (!Serial) { ; }

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }

  server.begin();
  Serial.print("Connected to WiFi. Open in browser: http://");
  Serial.println(WiFi.localIP());
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    String request = client.readStringUntil('\r');
    client.flush();

    // ---- Parse incoming commands ----
    if (request.indexOf("cmd=ac_on") != -1) {
      Serial.println("Command: AC ON");
      // TODO: add digitalWrite() to control AC relay
    }
    if (request.indexOf("cmd=ac_off") != -1) {
      Serial.println("Command: AC OFF");
    }
    if (request.indexOf("cmd=fan_0") != -1) {
      Serial.println("Command: Fan OFF");
    }
    if (request.indexOf("cmd=fan_1") != -1) {
      Serial.println("Command: Fan Level 1");
    }
    if (request.indexOf("cmd=fan_2") != -1) {
      Serial.println("Command: Fan Level 2");
    }
    if (request.indexOf("cmd=fan_3") != -1) {
      Serial.println("Command: Fan Level 3");
    }
    for (int i = 1; i <= 5; i++) {
      String winUp = "cmd=window" + String(i) + "_up";
      String winStop = "cmd=window" + String(i) + "_stop";
      String winDown = "cmd=window" + String(i) + "_down";
      if (request.indexOf(winUp) != -1) {
        Serial.print("Command: Open window ");
        Serial.println(i);
      }
      if (request.indexOf(winStop) != -1) {
        Serial.print("Command: Stopping window ");
        Serial.println(i);
      }
      if (request.indexOf(winDown) != -1) {
        Serial.print("Command: Close window ");
        Serial.println(i);
      }
    }

    // ---- Send Webpage ----
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println();
    client.println(R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Smart Home Control</title>
<style>
  body { font-family: Arial, sans-serif; text-align: center; margin: 0; background: #f2f2f2; }
  h2 { background: #007BFF; color: white; padding: 10px; margin: 0; }
  .section { background: white; margin: 10px; padding: 15px; border-radius: 10px; }
  button {
    width: 100%; padding: 15px; margin: 5px 0;
    font-size: 18px; border: none; border-radius: 8px;
    background: #ddd; cursor: pointer;
  }
  button.active { background: #007BFF; color: white; }
  .fan-grid { display: grid; grid-template-columns: repeat(3, 1fr); gap: 10px; }
  .fan-off { grid-column: span 3; font-size: 20px; font-weight: bold; }
  .window-controls button { width: 32%; display: inline-block; }
</style>
</head>
<body>

<h2>Smart Home Control</h2>

<div class="section">
  <h3>AC Control</h3>
  <button id="ac-toggle">Turn ON</button>
  <p id="ac-status">Status: OFF</p>
</div>

<div class="section">
  <h3>Fan Control</h3>
  <div class="fan-grid">
    <button onclick="setFan(1)">Level 1</button>
    <button onclick="setFan(2)">Level 2</button>
    <button onclick="setFan(3)">Level 3</button>
    <button class="fan-off" onclick="setFan(0)">OFF</button>
  </div>
  <p id="fan-status">Status: Off</p>
</div>

<div class="section">
  <h3>Window Controls</h3>
  <div id="windows">
    <div>
      <button onclick="toggleWindow(1)">Window 1</button>
      <div id="win1" class="window-controls" style="display:none;">
        <button onclick="moveWindow(1, 'up')">Open</button>
        <button onclick="moveWindow(1, 'stop')">Stop</button>
        <button onclick="moveWindow(1, 'down')">Close</button>
      </div>
    </div>
    <div>
      <button onclick="toggleWindow(2)">Window 2</button>
      <div id="win2" class="window-controls" style="display:none;">
        <button onclick="moveWindow(2, 'up')">Open</button>
        <button onclick="moveWindow(2, 'stop')">Stop</button>
        <button onclick="moveWindow(2, 'down')">Close</button>
      </div>
    </div>
    <div>
      <button onclick="toggleWindow(3)">Window 3</button>
      <div id="win3" class="window-controls" style="display:none;">
        <button onclick="moveWindow(3, 'up')">Open</button>
        <button onclick="moveWindow(3, 'stop')">Stop</button>
        <button onclick="moveWindow(3, 'down')">Close</button>
      </div>
    </div>
    <div>
      <button onclick="toggleWindow(4)">Window 4</button>
      <div id="win4" class="window-controls" style="display:none;">
        <button onclick="moveWindow(4, 'up')">Open</button>
        <button onclick="moveWindow(4, 'stop')">Stop</button>
        <button onclick="moveWindow(4, 'down')">Close</button>
      </div>
    </div>
    <div>
      <button onclick="toggleWindow(5)">Window 5</button>
      <div id="win5" class="window-controls" style="display:none;">
        <button onclick="moveWindow(5, 'up')">Open</button>
        <button onclick="moveWindow(5, 'stop')">Stop</button>
        <button onclick="moveWindow(5, 'down')">Close</button>
      </div>
    </div>
  </div>
  <p id="window-status">No action yet</p>
</div>

<script>
  function sendCommand(cmd) {
    fetch("/?cmd=" + cmd).catch(err => console.error(err));
  }

  // AC
  let acOn = false;
  document.getElementById("ac-toggle").addEventListener("click", function() {
    acOn = !acOn;
    this.textContent = acOn ? "Turn OFF" : "Turn ON";
    document.getElementById("ac-status").textContent = "Status: " + (acOn ? "ON" : "OFF");
    this.classList.toggle("active", acOn);
    sendCommand(acOn ? "ac_on" : "ac_off");
  });

  // Fan
  function setFan(level) {
    const buttons = document.querySelectorAll(".fan-grid button");
    buttons.forEach(btn => btn.classList.remove("active"));
    buttons[level === 0 ? 3 : level-1].classList.add("active");
    const levels = ["Off", "Level 1", "Level 2", "Level 3"];
    document.getElementById("fan-status").textContent = "Status: " + levels[level];
    sendCommand("fan_" + level);
  }

  // Windows
  function toggleWindow(num) {
    for (let i = 1; i <= 5; i++) {
      document.getElementById("win"+i).style.display = (i === num && document.getElementById("win"+i).style.display === "none") ? "block" : "none";
    }
  }
  function moveWindow(num, dir) {
    document.getElementById("window-status").textContent = dir + " window " + num + "...";
    sendCommand("window"+num+"_"+dir);
  }
</script>

</body>
</html>
)rawliteral");
    client.println();
    client.stop();
  }
}
