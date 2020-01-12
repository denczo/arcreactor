#include <Adafruit_NeoPixel.h>

#define LED_COUNT 7
#define PIN 2
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LED_COUNT, PIN);

int redValue = 255;
int greenValue = 255;
int blueValue = 255;
int blackValue = 0;
int oldTime;
int timeSteps = 100;
int startTimeSteps = timeSteps;
int minimumSteps = 10;
int oldLed = 0;
int centerLed = 0;
bool started = false;

int centerRed = 50;
int centerGreen = 50;
int centerBlue = 60;

bool failure = false;
bool reactorOffline = false;
int failureTime = 2000;
int startFailureTime = failureTime;
int coolDown = 100;
int rebootTime = 30000;
bool reboot = false;

struct color {
  int red;
  int green;
  int blue;
};

struct color criticalColor;
struct color currentColor;
struct color blackColor;
struct color optimalColor;
struct color startColor;

int timeColorChange = 5;
bool unstable = false;

void setup() {

  criticalColor.red = 255;
  criticalColor.green = 0;
  criticalColor.blue = 0;

  optimalColor.red = 168;
  optimalColor.green = 221;
  optimalColor.blue = 225;

  blackColor.red = 0;
  blackColor.green = 0;
  blackColor.blue = 0;

  currentColor.red = 50;
  currentColor.green = 50;
  currentColor.blue = 60;
  
  startColor.red = 50;
  startColor.green = 50;
  startColor.blue = 60;

  
  pixels.begin();
  pixels.show();
  pixels.setBrightness(50);
  oldTime = millis();
}

void loop() {

  int currentTime = millis();
  int elapsedTime = currentTime - oldTime;

  //STARTUP ANIMATION
  if (!reboot) {

    if (elapsedTime >= timeSteps && !started) {
      if (oldLed == (LED_COUNT - 1) &&  timeSteps > minimumSteps) {
        timeSteps -= 10;
      }
      else if (timeSteps <= minimumSteps) {
        started = true;
      }
      int currentLed = getNextLed(oldLed, LED_COUNT);
      oldTime = millis();
      for (int i = 0; i < pixels.numPixels(); i++) {

        if (currentLed == centerLed) {
          currentLed += 1;
        }

        if (i == currentLed) {
          pixels.setPixelColor(i, redValue, greenValue, blueValue);
          if (i > 1) {
            pixels.setPixelColor(i - 1, redValue - 150, greenValue - 150, blueValue - 150);
          }
          else {
            pixels.setPixelColor(i + 5, redValue - 150, greenValue - 150, blueValue - 150);
          }
        } else {
          pixels.setPixelColor(i, blackValue, blackValue, blackValue);
        }
      }
      oldLed = currentLed;
    }

    if (started) {
      bool reached = reachedCriticalColor(currentColor, optimalColor);
      if (elapsedTime > 10 && !reached) {
        oldTime = millis();
        currentColor.red = raiseValue(currentColor.red, optimalColor.red);
        currentColor.green = raiseValue(currentColor.green, optimalColor.green);
        currentColor.blue = raiseValue(currentColor.blue, optimalColor.blue);
      }
      else if(reached && elapsedTime > rebootTime){
        reboot = true;
        started = false;
      }

      setPixels(currentColor);
    }

  }

 else {

  //ANIMATION FAILURE
  if (reachedCriticalColor(currentColor, criticalColor)) {
    if (elapsedTime >= failureTime && !failure) {
      if (failureTime > 200) {
        oldTime = millis();
        failureTime = (int) failureTime / 2;
        failure = true;
      }else if(elapsedTime > 3000){
        reboot = false;
        currentColor = startColor;
        timeSteps = startTimeSteps;
        failureTime = startFailureTime;
      }
      setPixels(blackColor);
    }
    else if (elapsedTime >= coolDown && failure) {
      oldTime = millis();
      failure = false;
      setPixels(currentColor);
    }
  }
  //ANIMATION OVERHEATING
  else {
    if (elapsedTime > 100) {
      oldTime = millis();
      currentColor.red = raiseValue(currentColor.red, criticalColor.red);
      currentColor.green = lowerValue(currentColor.green, criticalColor.green);
      currentColor.blue = lowerValue(currentColor.blue, criticalColor.blue);
      setPixels(currentColor);
    }
  }
}
pixels.show();
}


void setPixels(color pixelColor) {
  for (int i = 0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(i, pixelColor.red, pixelColor.green, pixelColor.blue);
  }
}

int getNextLed(int currentLed, int ledCount) {
  if (currentLed >= (ledCount - 1)) {
    return 0;
  }
  else {
    return (currentLed += 1);
  }
}

int raiseValue(int currentValue, int maxValue) {
  if (currentValue < maxValue) {
    currentValue += 1;
  }
  return currentValue;
}

int lowerValue(int currentValue, int minValue) {
  if (currentValue > minValue) {
    currentValue -= 1;
  }
  return currentValue;
}

bool reachedCriticalColor(color currentColor, color criticalColor) {
  if (currentColor.red == criticalColor.red && currentColor.green == criticalColor.green && currentColor.blue == criticalColor.blue) {
    return true;
  }
  return false;
}
