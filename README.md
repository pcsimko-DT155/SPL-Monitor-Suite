
## Raspberry Pi Decibel Monitor

This repo contains the necessary code to run a Raspberry Pi 3+ as a decibel monitor in kiosk mode.
The software is separated into two independent components: A monitor application that periodically reads a decibel 
meter and writes a running average of measured values to a TCP socket and a GUI client that reads 
from the socket and displays the data via the web. Decoupling the monitor and GUI allows the monitor to serve dB
data across the LAN for potential future consumption by my Windows desktop.

### Hardware

1. **Raspberry Pi 3+** I'm using a RPi that I had in a junk drawer.
   I don't see any reason why other versions wouldn't work.
2. **HDMI 5" 800x480 Display Backpack** I chose this for its small size and easy connectivity. Having a touchscreen is nice but not a requirement.
3. **PCB Artists I2C Decibel Meter** Good price and easy connectivity. Accuracy is &#177;2dB which is enough for my purposes. Manual [here](https://pcbartists.com/product-documentation/i2c-decibel-meter-programming-manual/).

### Sound Monitor

The Sound monitor interfaces with the I2C decibel meter, periodically reading the value in a monitor thread and adding it to a fixed-size deque used to calculate a running average. A separate thread handling the TCP connection
periodically writes the running average to the socket. The monitor executable can launched as a startup service with systemd.

### GUI Client

The GUI client leverages flask python with a single (very small) top-level script that reads the TCP socket from a thread. This can be run from a WSGI launched at startup.

### Startup script
Not strictly required by the application, the `kiosk.sh` script is given as a simple guideline to be followed or ignored as desired. Fully disabling the mouse cursor may require changing 
settings is raspi-config.
