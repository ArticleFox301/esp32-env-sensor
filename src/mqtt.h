#ifndef MQTT_H
#define MQTT_H

#include <Arduino.h>

// MQTT settings
extern const char* mqtt_server;
extern const int mqtt_port;
extern const char* mqtt_username;
extern const char* mqtt_password;
extern const char* mqtt_client_id;
extern const char* mqtt_topic_sensor_data;
extern const char* mqtt_topic_relay_control;
extern const char* mqtt_topic_relay_status;
extern const char* mqtt_topic_system_status;

void mqtt_init();
void mqtt_connect();
bool mqtt_publish_sensor_data(float temp, float hum, int light, int air);
bool mqtt_publish_relay_status();
void mqtt_loop();

// Callback for receiving MQTT messages
void mqtt_callback(char* topic, byte* payload, unsigned int length);

#endif