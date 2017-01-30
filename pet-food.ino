#include <Servo.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "config.h"

WiFiClient espClient;
PubSubClient mqtt(espClient);
Servo servo;

#define SERVO_PIN 0
#define BUTTON_PIN 2

void setup()
{
    pinMode(SERVO_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT);

    Serial.begin(115200);
    setup_wifi();
    mqtt.setServer(MQTT_SERVER, MQTT_PORT);
}

static void setup_wifi()
{
    delay(10);

    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PSK);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void loop()
{
    if (!mqtt.connected()) {
        reconnect();
    }

    if (digitalRead(BUTTON_PIN) == LOW) {
        serve();
    }
}

static void reconnect()
{
    while (!mqtt.connected()) {
        Serial.print("Attempting MQTT connection... ");

        if (mqtt.connect("esp8266", MQTT_USER, MQTT_PASSWORD)) {
            Serial.println("connected");
        }
        else {
            Serial.print("failed, rc=");
            Serial.println(mqtt.state());

            delay(5000);
        }
    }
}

static void serve()
{
    open_cup();
    delay(500);
    notify();
    close_cup();
}

static void open_cup()
{
    servo_move(1, false);
}

static void close_cup()
{
    servo_move(1, true);
}

static void servo_move(double duration, bool inverted)
{
    int speed = 180;

    if (inverted) {
        speed *= -1;
    }

    servo.attach(SERVO_PIN);
    servo.write(speed);
    delay(duration * 1000);
    servo.write(90);
    delay(500);
    servo.detach();
}

static void notify()
{
    mqtt.publish("cat/food", "serve");
}
