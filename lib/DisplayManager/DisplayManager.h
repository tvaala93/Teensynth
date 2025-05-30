#include <Wire.h>
//#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MenusOLED.h> 

#define BASE_ICON_Y 32

// =================================================================================
// What are the sections of the display?
// =================================================================================
/*

// Text-focussed menu - FUNCTIONAL
+---------------------+
|<     TextMenu    **>|
|#####################|
|0: ZERO             ^|
|1: ONE               |
|2: TWO               |
|3: THREE             |
|4: FOUR              |
|5: FIVE             v|
+---------------------+

// Icon-focussed menu
+---------------------+
|<     IconMenu    **>|
|#####################|
| ASDF ASDF ASDF ASDF |
| /--\ /--\ /--\ /--\ |
| |  | |  | |  | |  | |
| |  | |  | |  | |  | |
| \--/ \--/ \--/ \--/ |
| WORD WORD WORD WORD |
+---------------------+

// Grid-focussed menu - used for mod matrix
+---------------------+
|<     GridMenu    **>|
|#####################|
|<  > END0 END1 END2 ^|
|INP0| *  |    |    | |
|INP1|    | *  |    | |
|INP2|    |    | *  | |
|INP3|    | *  |    | |
|INP4| *  |    |    |V|
+---------------------+

// ADSR-focussed menu (Line)
+---------------------+
|<     ADSRMenu    **>|
|#####################|
|   Loop Time: 6.9    |
|    /\               |
|   /  \__________    |
|  /              \   |
| /                \  |
| A:1.2 D/S:2.3 R:3.4 |
+---------------------+

*/


class MenuBar{};

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
        
        //Icon slot0 = {nullptr, "SLT0",/*"BLU"*/nullptr, "0.0"};
        //Icon slot1 = {nullptr, "SLT1",/*"GRN"*/nullptr, "0.0"};
        //Icon slot2 = {nullptr, "SLT2",/*"YLW"*/nullptr, "0.0"};
        //Icon slot3 = {nullptr, "SLT3",/*"ONG"*/nullptr, "0.0"};
        

    public:
        DisplayManager(Adafruit_SSD1306& displayy, MenusOLED* mainMenu) : 
            display(displayy), 
            optionIndex(0),
            textOffset(0),
            currentMenu(mainMenu),
            baseMenu(mainMenu) {}
        void drawIcon(int slot, int dir);
        std::array<int8_t,2> drawADSR(int slot);
        //void drawText(int x, int y, const String& text, bool highlight);        
        //void clear();
        //void display();
        // Display Functions
        void drawMenuArea(bool color);
        void drawLogo(bool color);
        void drawLRArrows(bool color);
        
        void show(bool color);        
        void showText();
        void showIcons();
        void showADSR();
        void highlight(int8_t i);

        // Screen Navigation
        void handleNavigation(int encDir);
        void handleTextNavigation(int encDir);
        void handleIconNavigation(int encDir);
        void navigateForward();
        void navigateBackward();

        // Helper Functions
        void fillArc2(int x, int y, int start_angle, int seg_count, int r, int w, bool colour);
        MenusOLED* getCurrentMenu();
};