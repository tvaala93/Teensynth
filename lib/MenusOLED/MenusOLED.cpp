#include <MenusOLED.h>

MenusOLED::MenusOLED(String name, Adafruit_SSD1306& displayy, const unsigned char* logo) : display(displayy)
{
    menuName = name;
    //parent = parentt;
    display = displayy;
    if(logo!=NULL){
        menuLogo = logo;
    }else{
        menuLogo = NULL;
    }
    parent = NULL;
}

void MenusOLED::show(){
    //reset menu area of display
    //display.clearDisplay();
    display.fillRect(0,0,128,9,SSD1306_WHITE);
    
    // Print menu name
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(menuName, 128, 0, &x1, &y1, &w, &h); //calc width of new string    
    display.setCursor((128 - w) / 2, 1);
    display.setTextColor(SSD1306_BLACK,SSD1306_WHITE);
    display.print(menuName);

    // Print Arrows
    // TODO add logic around arrow displays
    display.setCursor(1,1);
    display.write(0x11);
    display.setCursor(122,1);
    display.write(0x10);

    display.setTextColor(SSD1306_WHITE,SSD1306_BLACK);
    display.setCursor(1,9);
    display.write(0x1e);
    display.setCursor(1,59);
    display.write(0x1f);

    // Print logo
    if(menuLogo != NULL){
        display.drawBitmap(128-17-8,0, menuLogo,SPRITE_WIDTH,SPRITE_HEIGHT,SSD1306_BLACK);
    }
    display.display();    
}

 void MenusOLED::setParent(MenusOLED* par){
    parent = par;
 }

 void MenusOLED::setChild(MenusOLED* chile){
    child = chile;
 }

MenusOLED::~MenusOLED()
{
}
