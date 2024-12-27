# Arduino-M5Stack-AtomS3-Puzzle

### Arduino - M5Stack - AtomS3 Lite - 2 x Puzzle Unit

change values in: 
```sh
config.h
```
sample text sent to mqtt server:
```sh
mosquitto_pub -h mqtt_host -p mqtt_port -i mqtt_client_id -u mqtt_username -P mqtt_password -t mqtt_subscribe_topic -m "scrolling text on M5Stack AtomS3 lite with puzzle unit,#AA7733"
```
