#include <Wire.h>
//#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MenusOLED.h> 

class DisplayManager{
    private:
        Adafruit_SSD1306& display;
        // Helper functions
        void drawArrows(bool showUp, bool showDown);        
        void printText(int yOffset, const String& text, bool highlight);
        int8_t optionIndex; // Index of the currently highlighted option
        int8_t textOffset; // Offset for text display
        MenusOLED* currentMenu;
        MenusOLED* baseMenu;
    public:
        DisplayManager(Adafruit_SSD1306& displayy, MenusOLED* mainMenu) : 
            display(displayy), 
            optionIndex(0),
            textOffset(0),
            currentMenu(mainMenu),
            baseMenu(mainMenu) {}
        void drawIcon(Icon icon);
        //void drawText(int x, int y, const String& text, bool highlight);        
        //void clear();
        //void display();
        // Display Functions
        void drawMenuArea(bool color);
        void drawLogo(bool color);
        void drawLRArrows(bool color);
        void show(bool color);        
        void showText();
        void showGraphic();
        void highlight(int8_t i);

        // Screen Navigation
        void handleNavigation(int encDir);
        void handleTextNavigation(int encDir);
        void handleGraphicNavigation(int encDir);
        void navigateForward();
        void navigateBackward();
        
};