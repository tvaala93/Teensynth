#include <Arduino.h>
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>
//#include <typeinfo>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3C for 128x64, 0x3D for 128x32

#define MAX_VISIBLE_OPTIONS 6 // Number of options visible on the screen at once
#define TEXT_AREA_OFFSET 17 // Offset for text area
#define MENU_NAME_HEIGHT 8 // Height of menu name area

enum MenuModes {
    MODE_DEFAULT,
    MODE_TEXT,
    MODE_ICON,
    MODE_GRID,
    MODE_LINE
};

// ================================================================================================
// 8x16 pixel sprites
// ================================================================================================
#define SPRITE_WIDTH 16
#define SPRITE_HEIGHT 8
const unsigned char homeBMP[] PROGMEM = {
    0b00000000,0b10000000,
    0b00000001,0b11010000,
    0b00000011,0b11110000,
    0b00000111,0b11110000,
    0b00001111,0b11111000,
    0b00000110,0b11010000,
    0b00000110,0b11110000,
    0b00000110,0b11110000    
};

const unsigned char keysBMP[] PROGMEM = {
    0b11111111,0b11111111,
    0b11111111,0b11111111,
    0b10110110,0b11011011,
    0b10110110,0b11011011,
    0b10110110,0b11011011,
    0b10010010,0b01001001,
    0b10010010,0b01001001,
    0b11111111,0b11111111
};

const unsigned char envBMP[] PROGMEM = {
    0b01111111, 0b11111110,
    0b10010000, 0b00000001,
    0b10011000, 0b00000001,
    0b10100100, 0b00000001,
    0b10100011, 0b11111001,
    0b11000000, 0b00000101,
    0b11000000, 0b00000011,
    0b01111111, 0b11111110,
};
const unsigned char oscBMP[] PROGMEM = {
    0b01111111, 0b11111110,
    0b10000000, 0b00000001,
    0b11110000, 0b10001111,
    0b10010001, 0b10010001,
    0b10010010, 0b10010001,
    0b10011100, 0b11100001,
    0b10000000, 0b00000001,
    0b01111111, 0b11111110,
};
const unsigned char noiseSHBMP[] PROGMEM = {
    9,143,
    215,11,
    7,95,
    225,205,
    31,130,
    22,14,
    79,231,
    122,51
};
const unsigned char mixBMP[] PROGMEM = {
    0b01111111, 0b11111110,
    0b10010010, 0b01001001,
    0b10111010, 0b01011101,
    0b10010010, 0b01001001,
    0b10010010, 0b11101001,
    0b10010111, 0b01001001,
    0b10010010, 0b01001001,
    0b01111111, 0b11111110,
};
const unsigned char fltBMP[] PROGMEM = {
    0b01111111, 0b11111110,
    0b10000000, 0b00000001,
    0b11111111, 0b00000001,
    0b10000000, 0b11100001,
    0b10000000, 0b00011001,
    0b10000000, 0b00001001,
    0b10000000, 0b00000101,
    0b01111111, 0b11111110,
};
const unsigned char effectsBMP[] PROGMEM = {};

// ================================================================================================
// 24x16 pixel sprites
// ================================================================================================
#define MSPRITE_WIDTH 24
#define MSPRITE_HEIGHT 16

const unsigned char sawtoothBMP[] PROGMEM = {
    0b00000000,0b00000000,0b00000000,
    0b00000000,0b00000000,0b00000000,    
    0b00000100,0b00000000,0b01000000,
    0b00001100,0b00000000,0b11000000,
    0b00010100,0b00000001,0b01000000,
    0b00100100,0b00000010,0b01000000,    
    0b01000100,0b00000100,0b01000000,
    0b10000100,0b00001000,0b01000000,
    0b00000100,0b00010000,0b01000001,
    0b00000100,0b00100000,0b01000010,    
    0b00000100,0b01000000,0b01000100,
    0b00000100,0b10000000,0b01001000,
    0b00000101,0b00000000,0b01010000,
    0b00000110,0b00000000,0b01100000,    
    0b00000000,0b00000000,0b00000000,
    0b00000000,0b00000000,0b00000000
};

const unsigned char squareBMP[] PROGMEM = {
    0b00000000,0b00000000,0b00000000,
    0b00000000,0b00000000,0b00000000,    
    0b00111100,0b00011111,0b11000000,
    0b00000100,0b00010000,0b01000000,
    0b00000100,0b00010000,0b01000000,
    0b00000100,0b00010000,0b01000000,    
    0b00000100,0b00010000,0b01000000,
    0b00000100,0b00010000,0b01000000,
    0b00000100,0b00010000,0b01000000,
    0b00000100,0b00010000,0b01000000,    
    0b00000100,0b00010000,0b01000000,
    0b00000100,0b00010000,0b01000000,
    0b00000100,0b00010000,0b01000000,
    0b00000111,0b11110000,0b01111110,    
    0b00000000,0b00000000,0b00000000,
    0b00000000,0b00000000,0b00000000
};
const unsigned char sineBMP[] PROGMEM = {
    0b00000000,0b00000000,0b00000000,
    0b00000000,0b00000000,0b00000000,    
    0b00000100,0b00000000,0b01000000,
    0b00001010,0b00000000,0b10100000,
    0b00010001,0b00000001,0b00010000,
    0b00010001,0b00000001,0b00010000,    
    0b00100000,0b10000010,0b00001000,
    0b00100000,0b10000010,0b00001000,
    0b00100000,0b10000010,0b00001000,
    0b00100000,0b10000010,0b00001000,    
    0b01000000,0b01000100,0b00000100,
    0b01000000,0b01000100,0b00000100,
    0b00000000,0b00101000,0b00000100,
    0b00000000,0b00010000,0b00000010,    
    0b00000000,0b00000000,0b00000000,
    0b00000000,0b00000000,0b00000000
};

const unsigned char triangleBMP[] PROGMEM = {
    0b00000000,0b00000000,0b00000000,
    0b00000000,0b00000000,0b00000000,    
    0b00000010,0b00000000,0b10000000,
    0b00000010,0b00000000,0b10000000,
    0b00000101,0b00000001,0b01000000,
    0b00000101,0b00000001,0b01000000,    
    0b00001000,0b10000010,0b00100000,
    0b00001000,0b10000010,0b00100000,
    0b00010000,0b01000100,0b00010000,
    0b00010000,0b01000100,0b00010000,    
    0b10100000,0b00101000,0b00001000,
    0b10100000,0b00101000,0b00001000,
    0b01000000,0b00010000,0b00000100,
    0b01000000,0b00010000,0b00000100,    
    0b00000000,0b00000000,0b00000000,
    0b00000000,0b00000000,0b00000000
};

// End sprites ------------------------------------------------------------------------------------


// ================================================================================================
// Other BMPs
// ================================================================================================

#define QR_WIDTH 35
#define QR_HEIGHT 35
// 'Screenshot 2025-03-01 at 18-00-15 QR Code Generator Create Your Free QR Codes', 35x35px
const unsigned char qrCode [] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xe0, 
	0x27, 0xe8, 0x80, 0xe0, 0xef, 0xbf, 0x5b, 0xbe, 0xe0, 0xe8, 0xac, 0x1e, 0xa2, 0xe0, 0xe8, 0xa5, 
	0x8b, 0xa2, 0xe0, 0xe8, 0xa1, 0xb3, 0xa2, 0xe0, 0xef, 0xb6, 0x90, 0xbe, 0xe0, 0xe0, 0x2a, 0xaa, 
	0x80, 0xe0, 0xff, 0xf5, 0x05, 0xff, 0xe0, 0xe1, 0xaf, 0x7c, 0x62, 0xe0, 0xe6, 0xf7, 0xfc, 0x45, 
	0xe0, 0xf7, 0xbf, 0xa8, 0x65, 0xe0, 0xff, 0x5c, 0x57, 0xdd, 0xe0, 0xfa, 0x85, 0xde, 0x0f, 0xe0, 
	0xfe, 0xd9, 0x07, 0xc3, 0xe0, 0xf3, 0xb2, 0x83, 0x2b, 0xe0, 0xfa, 0xda, 0x68, 0x8c, 0xe0, 0xf0, 
	0x19, 0xe7, 0x69, 0xe0, 0xf1, 0x46, 0x7a, 0x1c, 0xe0, 0xed, 0x0c, 0xd1, 0xf3, 0xe0, 0xfa, 0xf2, 
	0xc9, 0xba, 0xe0, 0xf4, 0x14, 0x7c, 0x0a, 0xe0, 0xff, 0xee, 0x0c, 0xed, 0xe0, 0xe0, 0x38, 0x7c, 
	0xa5, 0xe0, 0xef, 0xbf, 0xd8, 0xe3, 0xe0, 0xe8, 0xb2, 0x92, 0x08, 0xe0, 0xe8, 0xac, 0xa3, 0xe9, 
	0xe0, 0xe8, 0xa5, 0x77, 0xc2, 0xe0, 0xef, 0xa1, 0x82, 0x72, 0xe0, 0xe0, 0x22, 0xa9, 0x95, 0xe0, 
	0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xe0
};

// 'teensynthlogo', 128x16px
const unsigned char teensynthlogo [] PROGMEM = {
	0x00, 0x00, 0x03, 0xfe, 0xff, 0x7f, 0xb0, 0x2f, 0xf6, 0x05, 0x02, 0xff, 0xa0, 0x40, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x63, 0x80, 0xc0, 0x70, 0x78, 0x06, 0x1b, 0x06, 0x18, 0x60, 0xc0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc3, 0x00, 0xc0, 0x70, 0x78, 0x06, 0x17, 0x86, 0x18, 0xc0, 0xc0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc3, 0x01, 0xc0, 0x70, 0xf0, 0x06, 0x37, 0x86, 0x30, 0xc0, 0xc0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc3, 0x01, 0x80, 0x78, 0xf0, 0x07, 0x67, 0x8c, 0x30, 0xc1, 0x80, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc7, 0x01, 0x80, 0xd8, 0xf0, 0x03, 0xc7, 0x8c, 0x30, 0xc1, 0x80, 0x00, 0x00, 
	0x00, 0x00, 0x01, 0x86, 0x01, 0x80, 0xd8, 0xf0, 0x03, 0x8f, 0xcc, 0x31, 0x81, 0x80, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x87, 0xf1, 0xf8, 0xd9, 0xef, 0xe3, 0x8c, 0xcc, 0x61, 0xff, 0x80, 0x00, 0x00, // Edit here
	0x00, 0x00, 0x01, 0x86, 0x03, 0x00, 0xdd, 0x80, 0xe3, 0x0c, 0xd8, 0x61, 0x83, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x01, 0x8c, 0x03, 0x01, 0x8d, 0x80, 0xc3, 0x0c, 0xd8, 0x61, 0x83, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x03, 0x0c, 0x03, 0x01, 0x8f, 0x80, 0xc3, 0x1c, 0xf8, 0x63, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x03, 0x0c, 0x06, 0x01, 0x8f, 0x00, 0xc7, 0x18, 0x78, 0xc3, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x03, 0x0c, 0x06, 0x01, 0x8f, 0x01, 0xc6, 0x18, 0x70, 0xc3, 0x06, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x03, 0x18, 0x06, 0x03, 0x07, 0x01, 0x86, 0x18, 0x70, 0xc3, 0x06, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x06, 0x18, 0x06, 0x03, 0x07, 0x01, 0x86, 0x38, 0x70, 0xc6, 0x06, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x06, 0x1f, 0xf7, 0xfa, 0x06, 0xff, 0x0c, 0x30, 0x21, 0x86, 0x04, 0x00, 0x00, 0x00
};

// End other BMPs ---------------------------------------------------------------------------------

// An Icon describes the appearance of a slot
// If an icon does not have a bitmap, the slot will be referenced for the value
struct Icon {
    const unsigned char* bmp;
    const char* label;
    const char* above;  
};

enum EffectorTypes {
    TYP_OSC,
    TYP_LFO,
    TYP_FLT,
    TYP_ENV,
    TYP_MIX
};

// Pointers to the value modified by each encoder        
// A Slot describes how to handle an encoder's input
// Storing this in MenusOLED because the controls depend on which menu is active
struct Slot {
    int8_t* effector=nullptr; // using float pointer to enable value sharing b/w slots
    int8_t minVal; // minimum value, NOT minimum onValueChange input
    int8_t maxVal; // maximum value, NOT maximum onValueChange input
    int8_t step=1;
    //float value=0;
    //float scale=1.0;
    const char* name=nullptr;
    //EffectorTypes effectorType;
    std::function<float(float)> scalingFunction;
    std::function<void(float)> onValueChange; // Callback function
};

class MenusOLED {
    protected:    
        int mode;
        String menuName;        
        const unsigned char* menuLogo;         
        MenusOLED* parent;
        bool isActive;
                    
        std::vector<MenusOLED*> children;
        
        //std::vector<Icon> icons;
        //int8_t iconIndex;

        // Numbering based off slot0 being the blue encoder
        // Create a fixed-size array of vectors using std::array
        //std::array<std::vector<Icon>, 4> slotIcons;
        std::vector<Icon> slotIcons[4]; // Array of vectors for icons in each slot
        int8_t slotIconIndex[4] = {0,0,0,0}; // Array tracking active icon index per slot
        Slot* slots[4]; // Array of slot pointers

    public:
        MenusOLED(int modee, String name, const unsigned char* logo)
        : mode(modee), menuName(name), menuLogo(logo), parent(nullptr), isActive(false){}
        
        // Tree Functions
        void addChild(MenusOLED* child);
        MenusOLED* getParent();
        MenusOLED* getChild(uint8_t index);
        size_t getChildCount();
        //MenusOLED* getLeft();
        //MenusOLED* getRight();
        MenusOLED* getSibling(int dir);

        // Graphic Functions
        void addIcon(int slot, Icon icon);
        Icon* getIcon(int slot, int8_t dir);
        size_t getIconCount(uint8_t slot);
        const unsigned char* getLogo();
        
        // Activation Functions
        bool activate();
        bool deactivate();        
        bool getActive();

        // Control Functions
        void setPtr(uint8_t index, float* ptr);
        void setSlot(
            uint8_t index, 
            int8_t min, 
            int8_t max, 
            int8_t step, 
            //float scale, 
            int8_t* ptr, 
            //float initialValue, 
            const char* name=nullptr,
            std::function<float(float)> scalingFunction = [](float val){return val;},
            std::function<void(float)> onValueChange = [](float val){}// Callback param
        );
        Slot* getSlot(uint8_t index);
        void writeSlot(uint8_t index, int8_t dir);

        // Utility Functions
        String getName();
        uint8_t getMode();
        
        //bool testText(uint8_t i);
        MenusOLED* findMenuByName(const String& name);

        ~MenusOLED();
};

// ================================================================================================
// Helper Functions 
// ================================================================================================
