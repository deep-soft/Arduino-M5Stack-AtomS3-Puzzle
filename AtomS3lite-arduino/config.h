// v1.1

#define debug_mode

// Wifi config
const char *wifi_ssid            = "ssid";
const char *wifi_password        = "password";
const char *mqtt_host1           = "host1";
const char *mqtt_host2           = "host2";
const int   mqtt_port            = 1883;

const char *mqtt_client_id       = "mqtt-atoms3-neopixel-0001";
const bool  mqtt_use_auth        = true;
const char *mqtt_username        = "mqtt_user";
const char *mqtt_password        = "mqtt_password";

const char *mqtt_subscribe_topic = "neopixel/0001";

const char *config_time_server1  = "192.168.0.1";
const char *config_time_server2  = "pool.ntp.org";
const long gmtOffset_sec         = 2L * 60L * 60L; // GMT + 2
const int  daylightOffset_sec    = 3600;
