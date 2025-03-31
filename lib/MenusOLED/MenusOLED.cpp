#include <MenusOLED.h>

// Default Constructor
MenusOLED::MenusOLED(    
    String name, 
    Adafruit_SSD1306& displayy, 
    const unsigned char* logo):
        display(displayy)
{
    mode = MODE_DEFAULT;
    menuName = name;    
    display = displayy;
    menuLogo = logo;
    parent = NULL;
    isActive = false;
    textOffset = 0;
    leftSibling = NULL;
    rightSibling = NULL;
}

// Text Menu Constructor
MenusOLED::MenusOLED(
    int modee,
    String name, 
    Adafruit_SSD1306& displayy, 
    const unsigned char* logo,
    String* options,    
    MenusOLED* kiddos):
        display(displayy)
{
    mode = modee;
    menuName = name;    
    display = displayy;
    menuLogo = logo;
    parent = NULL;
    isActive = false;
    optionList = options;
    children = kiddos;
    textOffset = 0;
    leftSibling = NULL;
    rightSibling = NULL;
}
    // Display Functions --------------------------------------------------------------------------

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
        if(!isActive){
            if(leftSibling != NULL){
                display.setCursor(1,1);
                display.write(0x11);
            }
            if(rightSibling != NULL){
                display.setCursor(122,1);
                display.write(0x10);
            }
        }
        
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


    /**
     * @brief Shows the text options on screen
     * 
     * Arrows up and down are displayed if there are add'l options that won't fit
     * on screen all at once
     */
    void MenusOLED::showText(){
        // clear the text area
        display.fillRect(0,9,SCREEN_WIDTH,SCREEN_HEIGHT-8,SSD1306_BLACK);
        //display.display();
        
        display.setTextColor(SSD1306_WHITE,SSD1306_BLACK);
        for(int i=textOffset;i<NUM_OPTIONS;i++){
            if(optionList[i] == ""){ 
                return; //continue;
            }

            // Show upper arrow
            /*
            if(textOffset > 0){
                display.setCursor(121,17);
                display.write(0x1e);
            }
            */

            // Show lower arrow
            if((8*(i-textOffset)+17) >= (SCREEN_HEIGHT)){                
                display.setCursor(121,59);
                display.write(0x1f);
                return;
            }            
            display.setCursor(0,(8*(i-textOffset)+17));
            String tmp = String(i) + ": " + optionList[i];
            display.print(tmp);
        }
        display.display();        
    }


    void MenusOLED::showGraphic(){}

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
        if(i<textOffset){
            textOffset--;
            showText();
        }
        int yOffset = (8*(i-textOffset)+17);
        if(yOffset >= SCREEN_HEIGHT){
            textOffset++; // Update textOffset
            showText(); // Redraw text
            yOffset = (8*(i-textOffset)+17); // Update yOffset
        }        

        display.fillRect(0,yOffset,128,8,SSD1306_WHITE);
        display.setTextColor(SSD1306_BLACK,SSD1306_WHITE);
        display.setCursor(0,yOffset);
        String tmp = String(i) + ": " + optionList[i];
        display.print(tmp);

        // Clean up previous item
        if(yOffset-8 > 9){
            display.fillRect(0,yOffset-8,128,8,SSD1306_BLACK);
            display.setTextColor(SSD1306_WHITE,SSD1306_BLACK);
            display.setCursor(0,yOffset-8);
            tmp = String(i-1) + ": " + optionList[i-1];
            display.print(tmp);
        }

        // Clean up next item
        if(i+1 < NUM_OPTIONS){
            if(optionList[i+1] !=""){
                display.fillRect(0,yOffset+8,128,8,SSD1306_BLACK);
                display.setTextColor(SSD1306_WHITE,SSD1306_BLACK);
                display.setCursor(0,yOffset+8);
                tmp = String(i+1) + ": " + optionList[i+1];
                display.print(tmp);
            }
        }

        // Show upper arrow
        if(textOffset > 0){
            display.setCursor(121,17);
            display.write(0x1e);
        }
        // Show lower arrow
        if(optionList[textOffset+6]!=""){                
            display.setCursor(121,59);
            display.write(0x1f);            
        }

        display.display();
    }


    // Activation Functions -----------------------------------------------------------------------
    
    bool MenusOLED::getActive(){
        return isActive;
    }
    
    void MenusOLED::activate(){
        if(mode==MODE_DEFAULT){
            show(1);
            return;
        }
        isActive = true;
        show(0);

        // If textMenu, circle/highlight 
    }

    void MenusOLED::deactivate(){
        isActive = false;
        textOffset = 0;
        show(1);
    }

    
    bool MenusOLED::testText(uint8_t i){
        if(optionList[i]=="") return false;
        return true;
    }

    uint8_t MenusOLED::getMode(){
        return mode;
    }


    // Parent Functions ---------------------------------------------------------------------------
    void MenusOLED::setParent(MenusOLED* par){
        parent = {par};
    }

    MenusOLED* MenusOLED::getParent(){
        return parent;
    }

    // Sibling Functions --------------------------------------------------------------------------
    void MenusOLED::setNeighbors(MenusOLED* left, MenusOLED* right, MenusOLED* par){
        leftSibling = left;
        rightSibling = right;
        parent = par;
    }

    MenusOLED* MenusOLED::getLeft(){        
        return leftSibling;
    }

    MenusOLED* MenusOLED::getRight(){
        return rightSibling;
    }

    // Child Functions ----------------------------------------------------------------------------
    MenusOLED* MenusOLED::getChild(uint8_t index){        
        return &children[index];
    }

    

    String MenusOLED::getName(){
        return menuName;
    }

    MenusOLED::~MenusOLED(){}
