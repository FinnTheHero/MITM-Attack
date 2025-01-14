# Man-In-The-Middle (MITM) Attack

This is a Demo for Cyber Security project.

This project uses 2 devices and ESP.

1st device as server (preferably laptop or pc), ESP (ESP32-C3 in this case) as middle man and 2nd device as target (can be laptop, pc or mobile).

Target will connect to ESP and access the server running on device 1.

ESP will intercept the request, extract the login information and forward traffic to the server.

## Getting Started

Make sure you have `Go` installed as the server is written in it.

First of all, clone this repo and cd into it.

### Server
In the repo root directory run `go run main.go` (if you get errors you might need to run `go mod tidy` or install go if you dont have it)

Open your browser, type `localhost:8080` in address bar and make sure its working.

Open terminal and get the local ip of device running the server `ipconfig` or `ip addr` (looks like 192.168.X.X)

### Sketch
The `sketch_jan14a` is specifically made for `ESP32-C3 Super Mini` you might have to modify it for your board.

Set credentials of your wifi in the sketch `REAL_SSID` and `REAL_PASSWORD`.

Set the local ip address of the device running the server in `TARGET_SERVER`.

Flash the sketch to esp and listen to serial output, wait untill you see the `FAKE AP IP: ` in serial monitor.


*on target device (the one NOT running the server)*

Connect to the wifi called `FakeAP` with `fakepassword`.

Open browser and type the fake ap ip from serial monitor in the address bar.

---

Project could've been much better and optimized to handle more traffic redirection, but due to time constraints this will do as demo.
