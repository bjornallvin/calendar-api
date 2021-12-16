// A basic everyday NeoPixel strip test program.

// NEOPIXEL BEST PRACTICES for most reliable operation:
// - Add 1000 uF CAPACITOR between NeoPixel strip's + and - connections.
// - MINIMIZE WIRING LENGTH between microcontroller board and first pixel.
// - NeoPixel strip's DATA-IN should pass through a 300-500 OHM RESISTOR.
// - AVOID connecting NeoPixels on a LIVE CIRCUIT. If you must, ALWAYS
//   connect GROUND (-) first, then +, then data.
// - When using a 3.3V microcontroller with a 5V-powered NeoPixel strip,
//   a LOGIC-LEVEL CONVERTER on the data line is STRONGLY RECOMMENDED.
// (Skipping these may work OK on your workbench but can fail in the field)

#include <Adafruit_NeoPixel.h>

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN 6

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 230

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

#define COLS 6
#define ROWS 4

const int numChars = 600;
char receivedChars[numChars];
boolean newCommand = false;
int receivedCount = 0;

int ledIndexToLedPixel[] = {0, 4, 9, 13, 17, 21,
                            30, 34, 38, 42, 46, 50,
                            60, 64, 68, 72, 76, 80,
                            89, 93, 97, 101, 105, 109};

uint32_t leds[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int numLeds = 24;

uint32_t red = strip.Color(255, 0, 0);   // Red
uint32_t green = strip.Color(0, 255, 0); // Green
uint32_t blue = strip.Color(0, 0, 255);  // Blue
uint32_t black = strip.Color(0, 0, 0);   // Blue

int animation = 0;

// setup() function -- runs once at startup --------------------------------

void setup()
{
  Serial.begin(9600);
  strip.begin();            // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();             // Turn OFF all pixels ASAP
  strip.setBrightness(150); // Set BRIGHTNESS to about 1/5 (max = 255)

  clearBoard();

  for (int i = 0; i < numLeds; i++)
  {
    strip.setPixelColor(ledIndexToLedPixel[i], red);
    strip.show();
    delay(50);
  }

  delay(1000);
  clearBoard();
}

// loop() function -- runs repeatedly as long as board is on ---------------

void loop()
{

  if (animation == 1)
  {
    int i = random(24);
    leds[i] = red;
    showLeds();
    delay(600);
    leds[i] = black;
    showLeds();
  }

  if (animation == 2)
  {
    rainbow(10);
  }

  receiveCommand();
  processCommand();
}

void clearBoard()
{
  for (int j = 0; j < numLeds; j++)
  {
    leds[j] = black;
  }
  showLeds();
}

void showLeds()
{
  for (int j = 0; j < numLeds; j++)
  {
    strip.setPixelColor(ledIndexToLedPixel[j], leds[j]);
  }
  strip.show();
}

int gridToLed(int col, int row)
{

  int led = row * COLS + col;
  return led;
}

void receiveCommand()
{
  static boolean recvInProgress = false;
  static int ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while (Serial.available() > 0 && newCommand == false)
  {
    rc = Serial.read();

    if (recvInProgress == true)
    {
      if (rc != endMarker)
      {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars)
        {
          ndx = numChars - 1;
        }
      }
      else if (rc == endMarker)
      {
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        receivedCount = ndx;
        ndx = 0;
        newCommand = true;
      }
    }

    else if (rc == startMarker)
    {
      recvInProgress = true;
    }
  }
}

void processCommand()
{
  if (newCommand == true)
  {
    Serial.print("This just in ... ");
    Serial.print(receivedCount);
    Serial.print(": ");
    Serial.println(receivedChars);

    char cmd = receivedChars[0];

    //----------------------------------------
    // VALID INCOMING CMDS

    // SLxxy - S(et) L(ed) XX (00-23) Y (0 or 1)
    // SAx - S(et) A(nimation) X (0 or 1 or 2)
    // CL - Clear board
    // S(et) B(oard) XXXXX....  (24 0's or 1's)

    //-----------------------------------------------
    if (receivedChars[0] == 'S') // Set value on or off
    {
      //char state_object = receivedChars[1];
      uint8_t state_object_value = receivedChars[2] - '0';
      //Serial.print("Setting ");
      //Serial.print(state_object);
      //Serial.print(" to ");
      //Serial.println(state_object_value);

      if (receivedChars[1] == 'L')
      {
        int tiotal = receivedChars[2] - '0';
        int ental = receivedChars[3] - '0';
        int ledIndex = tiotal * 10 + ental;

        if (receivedChars[4] == '0')
        {
          leds[ledIndex] = black;
        }
        else
        {
          leds[ledIndex] = red;
        }
        showLeds();
      }
      if (receivedChars[1] == 'A')
      {
        if (receivedChars[2] == '0')
        {
          animation = 0;
        }
        else
        {
          animation = receivedChars[2] - '0';
        }
      }

      if (receivedChars[1] == 'B')
      {
        for (int i = 0; i < numLeds; i++)
        {
          if (receivedChars[i + 2] == '1')
          {
            leds[i] = red;
          }
          else
          {
            leds[i] = black;
          }
        }
        showLeds();
      }
    }
    if (receivedChars[0] == 'C') // Set value on or off
    {
      clearBoard();
    }

    newCommand = false;
  }
}

// Some functions of our own for creating animated effects -----------------

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait)
{
  for (int i = 0; i < strip.numPixels(); i++)
  {                                // For each pixel in strip...
    strip.setPixelColor(i, color); //  Set pixel's color (in RAM)
    strip.show();                  //  Update strip to match
    delay(wait);                   //  Pause for a moment
  }
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait)
{
  for (int a = 0; a < 10; a++)
  { // Repeat 10 times...
    for (int b = 0; b < 3; b++)
    {                //  'b' counts from 0 to 2...
      strip.clear(); //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for (int c = b; c < strip.numPixels(); c += 3)
      {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait)
{
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256)
  {
    for (int i = 0; i < numLeds; i++)
    { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (ledIndexToLedPixel[i] * 65536L / numLeds);
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(ledIndexToLedPixel[i], strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait)
{
  int firstPixelHue = 0; // First pixel starts at red (hue 0)
  for (int a = 0; a < 30; a++)
  { // Repeat 30 times...
    for (int b = 0; b < 3; b++)
    {                //  'b' counts from 0 to 2...
      strip.clear(); //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for (int c = b; c < strip.numPixels(); c += 3)
      {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int hue = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color);                       // Set pixel 'c' to value 'color'
      }
      strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}
