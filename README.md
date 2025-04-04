# LVGL ESP32 SmartThings Controller

Converting lottie to .h header

```
cd main/ui/assets
python3 ../../../../managed_components/lvgl__lvgl/scripts/filetohex.py ac_on.json --filter-character --null-terminate > ac_on_json.h
```

edit the .h header to add consts and 0x00 (nul termination)
```
const uint8_t ac_on_json[] = {
    0x00
};
const size_t ac_on_json_size = sizeof(ac_on_json);
```

Create .env file with WiFi SSID and password, and Samsung SmartThings API token
```
WIFI_SSID="YourSSID"
WIFI_PASSWORD="YourPass"
MQTT_BROKER="12345-your-token-here-6789"
```

Generate certificates for smartthings api
```
openssl s_client -showcerts -connect api.smartthings.com:443 </dev/null | awk '/BEGIN/,/END/{ print $0 }' > main/api/certs/smartthings_root_cert.pem
```