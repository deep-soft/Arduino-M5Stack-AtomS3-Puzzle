# M5Stack AtomS3 lite - WLED configuration for platformio
# 
```sh
cd $HOME
mkdir WLED
cd WLED
sudo apt-get install python3-venv
python3 -m venv platformio
source platformio/bin/activate
pip3 install platformio
git clone https://github.com/Aircoookie/WLED.git
cd WLED/
```

</br>
// edit platformio.ini</br>
// comment out line 13 with all the different envs (chips) as # default_envs = nodemcuv2, esp8266_2m, esp01_1m_full, ....</br>
</br>
// add other line:

```sh
default_envs = m5stack-atoms3lite
```

</br>
// create file platformio_override.ini</br>
// with following lines:

```sh
[platformio]
default_envs = m5stack-atoms3lite

[env:m5stack-atoms3lite]
extends = env:m5stack-atoms3
board_build.flash_mode = dio
build_flags = ${env:m5stack-atoms3.build_flags} ${common.debug_flags}
   -D DATA_PINS=2
   -D LEDPIN=35
   -D BTNPIN=41
   -D IRPIN=4
; Configure default WiFi
   -D CLIENT_SSID='"ssid"'
   -D CLIENT_PASS='"ssid_password"'
; disable specific features
   -D WLED_DISABLE_ALEXA
   -D WLED_DISABLE_HUESYNC
   -D WLED_DISABLE_LOXONE
; enable optional built-in features
   -D WLED_ENABLE_PIXART
```
<br>
// change '"ssid"' and '"ssid_password"' or comment those lines</br>

```sh
pio pkg install
pio run
```

// firmware will be in ~/WLED/build_output/firmware/
