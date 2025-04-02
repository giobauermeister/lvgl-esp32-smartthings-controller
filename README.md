# LVGL ESP32 SmartThings Controller

Converting lottie to .h header

```
cd main/ui/assets
python3 ../../../managed_components/lvgl__lvgl/scripts/filetohex.py ac_on.json --filter-character --null-terminate > ac_on_json.h
```

edit the .h header to add consts and 0x00 (nul termination)
```
const uint8_t ac_on_json[] = {
    0x00
};
const size_t ac_on_json_size = sizeof(ac_on_json);
```
