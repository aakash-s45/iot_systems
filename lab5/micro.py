import network

ap = network.WLAN(network.AP_IF)
ap.active(True)
ap.config(essid="NodeMCU", authmode=network.AUTH_WPA_WPA2_PSK, password="hellonode")


