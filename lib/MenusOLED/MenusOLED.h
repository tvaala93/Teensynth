#include <Wire.h>
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3C for 128x64, 0x3D for 128x32

const String menuNames[8] = {
    "MAIN", "OSCILLATOR", "NOISE", "MIXER", "FILTER", "AMPLIFIER", "SAMPLE & HOLD", "EFFECTS"
};

class MenusOLED{
    private:    
        String menuName;
        //MenusOLED& parent;
        Adafruit_SSD1306& display;
    public:
        MenusOLED(String name, /*MenusOLED& parentt,*/ Adafruit_SSD1306& displayy);
        ~MenusOLED();
        void show();
};


class TextMenu: public MenusOLED{
    private:
    public:
        TextMenu(String name, Adafruit_SSD1306& displayy)
};


class GraphicMenu: public MenusOLED{
    private:
    public:
};