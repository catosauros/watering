#define RED_LED_PIN 4
#define YELLOW_LED_PIN 5
#define GREEN_LED_PIN 7
#define BLUE_LED_PIN 6

#define SENSOR_PIN A0
#define RELAY_PIN 3
#define BUTTON_PIN 2

#define WATER_DURATION 15  // pumps about 250ml

// tank is ~1.9l
// 1900ml / 250ml = 7.6
// don't want the pump to run dry so round down
#define MAX_WATER_TIMES 7


int times_watered = 0;
bool water_override = false;  // enables manual watering

void setup() {

  // Serial.begin(9600);

  // initialize pins
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);

  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);

  pinMode(SENSOR_PIN, INPUT);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), button_press, RISING);

  //  startup "animation"
  leds_startup();
}

void loop() {

  if (times_watered >= MAX_WATER_TIMES) {
    leds_warn();
  }

  if (water_override) {
    water_plant(WATER_DURATION);
  }

  int moisture = analogRead(SENSOR_PIN);
  // Serial.println(moisture);

  if (moisture <= 200) {
    leds_control(1, 0, 0, 0);
  } else if (moisture > 200 && moisture <= 350) {
    leds_control(0, 1, 0, 0);
  } else if (moisture > 350 && moisture <= 500) {
    leds_control(0, 0, 1, 0);
  } else {
    leds_control(0, 0, 0, 1);
    delay(1000);

    if (times_watered < MAX_WATER_TIMES) {
      water_plant(WATER_DURATION);
    }
  }
  delay(1000);
}

void water_plant(int seconds) {
  // Serial.println("Watering!");
  leds_control(0, 0, 0, 0);
  digitalWrite(RELAY_PIN, LOW);

  for (int i = 0; i < seconds; i++) {
    led_fade(BLUE_LED_PIN);  //each fade cycle is ~1s long
  }
  digitalWrite(RELAY_PIN, HIGH);


  for (int i = 0; i < 30 * 1000 / (4 * 200); i++) {  // 30 seconds with 4*200ms delay per cycle
    leds_loading();
  }
  // delay(30000);  // give water time to reach sensor

  times_watered++;
  water_override = false;
}

void button_press() {
  if (times_watered < MAX_WATER_TIMES) {
    water_override = true;
  } else {
    times_watered = 0;
  }
}

void leds_control(bool blue, bool green, bool yellow, bool red) {
  digitalWrite(BLUE_LED_PIN, blue);
  digitalWrite(GREEN_LED_PIN, green);
  digitalWrite(YELLOW_LED_PIN, yellow);
  digitalWrite(RED_LED_PIN, red);
}

void leds_startup() {
  leds_control(1, 0, 0, 0);
  delay(200);

  leds_control(0, 1, 0, 0);
  delay(200);

  leds_control(0, 0, 1, 0);
  delay(200);

  leds_control(0, 0, 0, 1);
  delay(200);

  leds_control(1, 1, 1, 1);
  delay(2000);

  leds_control(0, 0, 0, 0);
  delay(1000);
}

void leds_loading() {
  leds_control(1, 0, 0, 0);
  delay(200);

  leds_control(0, 1, 0, 0);
  delay(200);

  leds_control(0, 0, 1, 0);
  delay(200);

  leds_control(0, 0, 0, 1);
  delay(200);
}

void leds_warn() {
  leds_control(0, 0, 0, 0);
  delay(1000);

  for (int i = 0; i < 3; i++) {
    leds_control(1, 0, 0, 1);
    delay(1000);
    leds_control(0, 0, 0, 0);
    delay(1000);
  }
}

void led_fade(int led_pin) {
  for (int i = 0; i < 256; i += 16) {
    analogWrite(led_pin, i);
    delay(30);
  }
  for (int i = 255; i > 0; i -= 16) {
    analogWrite(led_pin, i);
    delay(30);
  }
  // about 1 second period
}
