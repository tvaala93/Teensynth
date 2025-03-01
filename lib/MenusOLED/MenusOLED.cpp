#include <MenusOLED.h>

MenusOLED::MenusOLED(String namee, Adafruit_SSD1306& displayy) : display(displayy)
{
    menuName = namee;
    //parent = parentt;
    display = displayy;
}

void MenusOLED::show(){
    display.clearDisplay();
    display.fillRect(0,0,128,8,SSD1306_WHITE);
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(menuName, 128, 0, &x1, &y1, &w, &h); //calc width of new string
    Serial.println(w);    
    display.setCursor((128 - w) / 2, 0);
    display.setTextColor(SSD1306_BLACK,SSD1306_WHITE);
    display.print(menuName);
    display.display();
}

MenusOLED::~MenusOLED()
{
}
