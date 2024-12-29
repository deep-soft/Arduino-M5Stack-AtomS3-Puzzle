# Arduino-M5Stack-AtomS3-Puzzle

### Arduino - M5Stack - AtomS3 Lite - 2 x Puzzle Unit

change values in: 
```sh
config.h
```
sample text sent to mqtt server:
```sh
mosquitto_pub -h mqtt_host -p mqtt_port -i mqtt_client_id -u mqtt_username -P mqtt_password -t mqtt_subscribe_topic -m "Scrolling text on M5Stack AtomS3 lite with puzzle unit,#AA7733/10"
```
```sh
// published message to split in 3 parts:
// 1 - the message (from first char to {,} )
// 2 - the color   (from {,} to  {/}       )
// 3 - the speed   (from {/} to last char  )
// note: 2nd and 3rd part are optional
```
