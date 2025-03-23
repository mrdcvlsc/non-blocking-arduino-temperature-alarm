// install adafruit dht library first
#include <DHT.h>

// DHT dht(/* PIN: */ 13, DHT11);
DHT dht(/* PIN: */ 13, DHT22); // DHT22 is way more accurate

int PIN_BUZZER = 2;
int PIN_BTN_ALARM_TOGGLER = 4; // button for alarm on and off
int PIN_LED_STATE_BTN = 7;    // lights up if button is working
int PIN_LED_STATE_ALARM = 8;  // lights up when alarm is on

bool is_alarm_on = true;
bool is_buzz = false;

bool alarm_led_state = false;
unsigned long t_alarm_led_last_blink = 0;
const long t_alarm_led_blink_interval = 1500; // 1.5 sec

unsigned long t_last_temperature_measure_time = 0;
const long t_temperature_measure_interval = 5000; // 5 sec
float last_temperature = 30.0f;
float min_celsius = 25.75f;
float max_celsius = 33.0f;

int last_btn_alarm_toggler_state = LOW;
int btn_alarm_toggler_state = LOW;

unsigned long t_last_btn_debounce_time = 0;
const long t_btn_debounce_delay = 50;

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_BTN_ALARM_TOGGLER, INPUT);
  pinMode(PIN_LED_STATE_BTN, OUTPUT);
  pinMode(PIN_LED_STATE_ALARM, OUTPUT);
}

void loop() {
  unsigned long current_time = millis();

  if (is_alarm_on) {
    if (current_time - t_alarm_led_last_blink >= t_alarm_led_blink_interval) {
      t_alarm_led_last_blink = current_time;
      alarm_led_state = !alarm_led_state;
      digitalWrite(PIN_LED_STATE_ALARM, alarm_led_state);
    }

  } else {
    digitalWrite(PIN_LED_STATE_ALARM, LOW);
  }

  int btn_toggler_state = digitalRead(PIN_BTN_ALARM_TOGGLER);
  if (btn_toggler_state != last_btn_alarm_toggler_state) {
    t_last_btn_debounce_time = current_time;
  }

  if ((current_time - t_last_btn_debounce_time) > t_btn_debounce_delay) {
    if (btn_toggler_state != btn_alarm_toggler_state) {
      btn_alarm_toggler_state = btn_toggler_state;
      if (btn_alarm_toggler_state == HIGH) {
        is_alarm_on = !is_alarm_on;
      }
    }
  }

  last_btn_alarm_toggler_state = btn_toggler_state;

  if (btn_toggler_state == HIGH) {
    digitalWrite(PIN_LED_STATE_BTN, HIGH);
    digitalWrite(PIN_BUZZER, HIGH);
  } else {
    digitalWrite(PIN_LED_STATE_BTN, LOW);
    digitalWrite(PIN_BUZZER, LOW);
  }

  if (is_buzz && is_alarm_on) {
    digitalWrite(PIN_BUZZER, HIGH);
  } else {
    digitalWrite(PIN_BUZZER, LOW);
  }

  if (current_time - t_last_temperature_measure_time >= t_temperature_measure_interval && is_alarm_on) {
    t_last_temperature_measure_time = current_time;

    float humidity = dht.readHumidity();
    float celcius = dht.readTemperature();

    Serial.print("humidity:");
    Serial.print(humidity);
    Serial.print(", ");
    Serial.print("temperature:");
    Serial.println(celcius);

    if (celcius < min_celsius || celcius > max_celsius) {
      is_buzz = true;
    } else {
      is_buzz = false;
    }
  }
}