#include <MenusOLED.h>

// Default Constructor
MenusOLED::MenusOLED(    
    String name, 
    Adafruit_SSD1306& displayy, 
    const unsigned char* logo):
        display(displayy)
{
    mode = MODE_OTHER;
    menuName = name;    
    display = displayy;
    menuLogo = logo;
    parent = NULL;
    isActive = false;
    textOffset = 0;
}

// Text Menu Constructor
MenusOLED::MenusOLED(
    uint8_t modee,
    String name, 
    Adafruit_SSD1306& displayy, 
    const unsigned char* logo,
    String* options):
        display(displayy)
{
    mode = modee;
    menuName = name;    
    display = displayy;
    menuLogo = logo;
    parent = NULL;
    isActive = false;
    optionList = options;
    textOffset = 0;
}

    /**
     * @brief Shows the menu name and logo in the top bar. If menu is text-based those options show
     * 
     * @param color indicates the menu name text color. 0= black text on white background 
     */
    void MenusOLED::show(bool color){
        //reset menu area of display
        display.clearDisplay();
        display.fillRect(0,0,128,9,color);
        
        // Print menu name
        int16_t x1, y1;
        uint16_t w, h;
        display.getTextBounds(menuName, 128, 0, &x1, &y1, &w, &h); //calc width of new string    
        display.setCursor((128 - w) / 2, 1);
        display.setTextColor(!color,color);
        display.print(menuName);

        // Print Arrows L/R
        // TODO add logic around arrow displays
        display.setCursor(1,1);
        display.write(0x11);
        display.setCursor(122,1);
        display.write(0x10);

        // Print logo
        if(menuLogo != NULL){
            display.drawBitmap(128-17-8,0, menuLogo,SPRITE_WIDTH,SPRITE_HEIGHT,!color);
        }

        switch (mode)
        {
            case MODE_TEXT:
                showText();
                break;
            case MODE_GRAPHIC:
                showGraphic();
                break;
            default:
                break;
        }        
        display.display();        
    }

    void MenusOLED::setParent(MenusOLED* par){
        parent = par;
    }

    void MenusOLED::setChild(MenusOLED* chile){
        child = chile;
    }

    /**
     * @brief Shows the text options on screen
     * 
     * Arrows up and down are displayed if there are add'l options that won't fit
     * on screen all at once
     */
    void MenusOLED::showText(){
        //TODO incorporate offset value       
        display.setTextColor(SSD1306_WHITE,SSD1306_BLACK);
        for(int i=0;i<NUM_OPTIONS;i++){
            if(optionList[i] == ""){ continue;}
            if((8*i+17) >= (SCREEN_HEIGHT)){
                // Show lower arrow
                display.setCursor(121,59);
                display.write(0x1f);
                break;
            }
            display.setCursor(0,(8*i+17));
            String tmp = String(i+textOffset) + ": " + optionList[i+textOffset];
            display.print(tmp);
        }
    }


    void MenusOLED::showGraphic(){}

    void MenusOLED::activate(){
        isActive = true;
        show(0);

        // If textMenu, circle/highlight 
    }

    void MenusOLED::deactivate(){
        isActive = false;
        show(1);
    }

    /**
     * @brief Highlights a particular text option on screen
     * 
     * @param i index from optionList to highlight
     */
    void MenusOLED::highlight(int8_t i){        
        if(mode != MODE_TEXT){return;}
        if(i<0 || i>=NUM_OPTIONS){return;}
        if(optionList[i] == ""){ return;}

        // TODO handle menu scroll / instances where there are more menu items than space on screen

        display.fillRect(0,(8*i+17),128,8,SSD1306_WHITE);
        display.setTextColor(SSD1306_BLACK,SSD1306_WHITE);
        display.setCursor(0,(8*i+17));
        String tmp = String(i) + ": " + optionList[i];
        display.print(tmp);

        // Clean up previous item
        if(i-1 >= 0){
            display.fillRect(0,(8*(i-1)+17),128,8,SSD1306_BLACK);
            display.setTextColor(SSD1306_WHITE,SSD1306_BLACK);
            display.setCursor(0,(8*(i-1)+17));
            tmp = String(i-1) + ": " + optionList[i-1];
            display.print(tmp);
        }

        // Clean up next item
        if(i+1 < NUM_OPTIONS){
            if(optionList[i+1] !=""){
                display.fillRect(0,(8*(i+1)+17),128,8,SSD1306_BLACK);
                display.setTextColor(SSD1306_WHITE,SSD1306_BLACK);
                display.setCursor(0,(8*(i+1)+17));
                tmp = String(i+1) + ": " + optionList[i+1];
                display.print(tmp);
            }
        }        
        display.display();
    }

    MenusOLED::~MenusOLED(){}

/*
TextMenu::TextMenu(    
    String name, 
    Adafruit_SSD1306& displayy, 
    const unsigned char* logo,
    String* options): 
        MenusOLED(name,displayy,logo)
{
    optionList = options;
}


    void TextMenu::showText(){
        for(int i=0;i<NUM_OPTIONS;i++){
            if(optionList[i] == ""){
                continue;
            }
            if((8*i+16) > (SCREEN_HEIGHT - 8)){
                break;
            }
            display.setCursor(0,(8*i+16));
            String tmp = String(i) + ": " + optionList[i];
            display.print(tmp);
        }
        display.display();
    }
*/