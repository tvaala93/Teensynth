#include "DisplayManager.h"

// ================================================================================
// DisplayManager Functions
// ================================================================================

void DisplayManager::drawMenuArea(bool color){
    //reset menu area of display
    display.clearDisplay();
    display.fillRect(0,0,SCREEN_WIDTH,9,color);

    // Print menu name
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(currentMenu->getName(), 128, 0, &x1, &y1, &w, &h); //calc width of new string    
    display.setCursor((128 - w) / 2, 1);
    display.setTextColor(!color,color);
    display.print(currentMenu->getName());
}

void DisplayManager::drawLogo(bool color){
    // Print logo
    if(currentMenu->getLogo() != nullptr){
        display.drawBitmap(SCREEN_WIDTH-TEXT_AREA_OFFSET-SPRITE_HEIGHT,0, currentMenu->getLogo(),SPRITE_WIDTH,SPRITE_HEIGHT,!color);
    }
}
    
void DisplayManager::drawLRArrows(bool color){
    // Print Arrows Left / Right        
    if(!currentMenu->getActive()){
        if(currentMenu->getSibling(-1) != nullptr){
            display.setCursor(1,1);
            display.write(0x11);
        }
        if(currentMenu->getSibling(1) != nullptr){
            display.setCursor(122,1);
            display.write(0x10);
        }
    }
}

/**
 * @brief Shows the menu name and logo in the top bar. If menu is text-based those options show
 * 
 * @param color indicates the menu name text color. 0= black text on white background 
 */
void DisplayManager::show(bool color) {
    drawMenuArea(color);
    drawLogo(color);
    drawLRArrows(color);    

    // Update the display based on the mode
    switch (currentMenu->getMode())
    {
        case MODE_DEFAULT:
            break;
        case MODE_TEXT:
            showText();
            break;
        case MODE_ICON:
            showIcons();
            break;
        case MODE_LINE:
            showADSR();
            break;
        default:
            Serial.println("Invalid mode");
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
void DisplayManager::showText() {
    //TODO implement the old MenusOLED showText function here
    // clear the text area
    display.fillRect(0,9,SCREEN_WIDTH,SCREEN_HEIGHT-8,SSD1306_BLACK);
    display.setTextColor(SSD1306_WHITE,SSD1306_BLACK);

    // Display visible options
    for (size_t i = textOffset; i < currentMenu->getChildCount(); ++i) {
        int yOffset = MENU_NAME_HEIGHT * (i - textOffset) + TEXT_AREA_OFFSET;
        if(yOffset >= SCREEN_HEIGHT) break; // Stop if we exceed the screen height
        printText(yOffset, String(i) + ": " + currentMenu->getChild(i)->getName(), false);     
    }

    // Show arrows if there are more items above or below (6 items visible at once)
    drawArrows(textOffset > 0, textOffset + 6 < int(currentMenu->getChildCount()));

    // Update the display
    display.display(); 
}

/**
 * @brief Shows the defined icons on screen
 * 
 */
void DisplayManager::showIcons(){
    // Clear the display work area?
    Serial.println("show Icons");    
    //display.fillRect(0, 9, SCREEN_WIDTH, SCREEN_HEIGHT - 8, SSD1306_BLACK);
    
    // Draw each icon    
    for (int i = 0; i < 4; ++i) {
        drawIcon(i, 0);
    }

    // Show the display
    display.display();
}

/**
 * @brief Shows drawn lines on screen
 * 
 * This is leveraged for the ADSR menus
 */
void DisplayManager::showADSR(){
    // clear the text area
    display.fillRect(0,9,SCREEN_WIDTH,SCREEN_HEIGHT-8,SSD1306_BLACK);
        
    uint8_t x0=0;
    uint8_t y0=SCREEN_HEIGHT;
    
    uint8_t y1 = 16; // Start and end coordinates
    
    int8_t times[4];
    
    // Get the envelope params
    for (int i = 0; i < 4; ++i) {
        times[i] = *currentMenu->getSlot(i)->effector; //times[2] will be the sustain level, not a time        
    }

    // Draw Attack Line
    int8_t width = map(times[0],0,100,0,SCREEN_WIDTH/2);
    display.drawLine(x0,y0,x0+width,y1,SSD1306_WHITE);
    x0+=width;
    y0=y1;
    // Draw Decay Line
    width = map(times[1],0,100,0,SCREEN_WIDTH/2);
    y1 = map(times[2],0,100,0,SCREEN_HEIGHT-16);
    y1 = SCREEN_HEIGHT - y1;
    display.drawLine(x0,y0,x0+width,y1,SSD1306_WHITE);
    x0+=width;
    y0=y1;
    // Draw Sustain Line
    width = SCREEN_WIDTH-x0-map(times[3],0,100,0,SCREEN_WIDTH/2);
    display.drawLine(x0,y0,x0+width,y1,SSD1306_WHITE);
    x0+=width;
    // Draw Release Line
    width = map(times[3],0,100,0,SCREEN_WIDTH/2);
    y1 = SCREEN_HEIGHT;
    display.drawLine(x0,y0,x0+width,y1,SSD1306_WHITE);
    // Update Text (Use the yellow space)
    display.setTextColor(SSD1306_WHITE,SSD1306_BLACK);
    String ADSR = "ADSR";
    for(int i=0;i<4;i++){
        display.setCursor(i*32,9);
        display.print(ADSR[i]);
        display.print(":");
        display.setCursor(i*32+12,9);
        display.print(times[i]);
    }
    display.display();
}

/**
 * @brief Highlights a particular text option on screen
 * 
 * @param index index from optionList to highlight
 */
void DisplayManager::highlight(int8_t index){        
    if (currentMenu->getMode() != MODE_TEXT || index < 0 || index >= int(currentMenu->getChildCount())) {
        Serial.printf("Invalid index or mode");
        return; // Invalid index or mode
    }    
    // Adjust optionIndex if the highlighted option is out of view
    if (index < textOffset) {
        textOffset = index; // Ensure the highlighted option is visible
    } else if (index >= textOffset + 5) {
    //} else if (8 * (index - textOffset) + TEXT_AREA_OFFSET >= SCREEN_HEIGHT) {
        textOffset = index - 5; // Ensure the highlighted option is visible
    }

    showText(); // Redraw the text area

    // Highlight the selected option
    int yOffset = 8 * (index - textOffset) + TEXT_AREA_OFFSET;
    printText(yOffset, String(index) + ": " + currentMenu->getChild(index)->getName(), true);
    
    // Show arrows if there are more items above or below
    drawArrows(textOffset > 0, textOffset + 6 < int(currentMenu->getChildCount()));

    display.display(); // Update the display
}


// ===================================================================================
// Helper Functions
// ===================================================================================

std::array<int8_t,2> DisplayManager::drawADSR(int slotNum){
    return {-1,-1};
}


void DisplayManager::drawIcon(int slotNum, int dir){
    Icon* icon = currentMenu->getIcon(slotNum, dir);    
    Slot* tmp = currentMenu->getSlot(slotNum);    
    int16_t x1, y1;
    uint16_t w, h, iconX;
    iconX = 32*slotNum + 16;
    
    display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    display.fillCircle(iconX, BASE_ICON_Y + MSPRITE_HEIGHT/2, MSPRITE_WIDTH/2+2, SSD1306_BLACK);
    
    // If icon is null, check if the slot is empty
    if(icon == nullptr || icon->bmp == nullptr){
        if (tmp!= nullptr){ //} && tmp->value != NULL) {
            Serial.print("tmp->value: ");
            Serial.println(*tmp->effector);
            String printVal = String(*tmp->effector);
            display.getTextBounds(printVal, 128, 0, &x1, &y1, &w, &h);
            display.fillRect(iconX - w / 2, BASE_ICON_Y+4, w, h, SSD1306_BLACK);
            display.setCursor(iconX - w / 2, BASE_ICON_Y+4);
            display.print(printVal);

            int nSegs = int(*tmp->effector * 120 / tmp->maxVal);
            fillArc2(iconX, BASE_ICON_Y + MSPRITE_HEIGHT/2, 180, nSegs, MSPRITE_WIDTH/2+1, 2, SSD1306_WHITE);
        }
        if (tmp->name != nullptr){
            display.getTextBounds(tmp->name, 128, 0, &x1, &y1, &w, &h);
            display.fillRect(iconX - 16, BASE_ICON_Y + MSPRITE_WIDTH, 32, h, SSD1306_BLACK);
            display.setCursor(iconX - w / 2, BASE_ICON_Y + MSPRITE_WIDTH);
            display.print(tmp->name);
        } 
    }

    // Draw the icon on the display if available
    else{
        if (icon->bmp != nullptr) {
            display.drawBitmap(iconX-MSPRITE_WIDTH/2, BASE_ICON_Y, icon->bmp, MSPRITE_WIDTH, MSPRITE_HEIGHT, SSD1306_WHITE);
        }

        // Calculate the width and height of the label
        if(icon->label != nullptr){
            display.getTextBounds(icon->label, 128, 0, &x1, &y1, &w, &h);
            display.fillRect(iconX - 16, BASE_ICON_Y + MSPRITE_WIDTH, 32, h, SSD1306_BLACK);
            display.setCursor(iconX - w / 2, BASE_ICON_Y + MSPRITE_WIDTH);
            display.print(icon->label); 
        }
        // Calculate the width and height of the above text
        if(icon->above != nullptr){
            display.getTextBounds(icon->above, 128, 0, &x1, &y1, &w, &h);
            display.fillRect(iconX - 16, BASE_ICON_Y - MSPRITE_WIDTH/2, 32, h, SSD1306_BLACK);
            display.setCursor(iconX - w / 2, BASE_ICON_Y - MSPRITE_WIDTH/2);
            display.print(icon->above); 
        }  
    }
    // Draw the circle around the icon    
    display.drawCircle(iconX, BASE_ICON_Y + MSPRITE_HEIGHT/2, MSPRITE_WIDTH/2, SSD1306_WHITE);
    
    //display.display();    
}

void DisplayManager::drawArrows(bool showUp, bool showDown) {
    display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    if (showUp) {
        display.setCursor(121, TEXT_AREA_OFFSET); // Position for the up arrow
        display.write(0x1e);       // Up arrow character
    }
    if (showDown) {
        display.setCursor(121, 59); // Position for the down arrow
        display.write(0x1f);        // Down arrow character
    }
}

void DisplayManager::printText(int yOffset, const String& text, bool highlight) {
    if (highlight) {
        display.fillRect(0, yOffset, SCREEN_WIDTH, 8, SSD1306_WHITE);
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    } else {
        display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    }
    display.setCursor(0, yOffset);
    display.print(text);
}

MenusOLED* DisplayManager::getCurrentMenu(){
    return currentMenu;
}

// ==================================================================================
// Navigation Functions
// ==================================================================================

// Need to tell navEnc what to do based on current navigationMode
void DisplayManager::handleNavigation(int encDir){  
    if(!currentMenu){
        Serial.println("Error: currentMenu is null");
        return;
    }
    if(encDir!= 0){
      // If menu is not active, navigate left/right
      if(!currentMenu->getActive() || currentMenu->getMode() == MODE_DEFAULT){      
        if(currentMenu->getSibling(encDir) != nullptr){        
          currentMenu = currentMenu->getSibling(encDir);          
          textOffset = 0; // Reset text offset when switching menus
          show(currentMenu->deactivate());
        }
        else{
          Serial.println("No sibling found");
        }
      }
      // If menu is active, navigate based on screen type
      else{
        // If menu is active, navigate based on screen type        
        switch(currentMenu->getMode()){
            case MODE_DEFAULT:
                Serial.println("Getting here shouldn't be possible...");
                break;      
            case MODE_TEXT:
                handleTextNavigation(encDir);
                break;
            case MODE_ICON:
                handleIconNavigation(encDir);
                break;
            default:
                break;
        }
      }

      Serial.print("currentMenu: ");
      Serial.println(currentMenu->getName());
      Serial.println();
    }
  }

void DisplayManager::handleTextNavigation(int encDir){
    // Handle navigation when in text mode
    if(optionIndex==0 && encDir==-1){return;} // Don't go below 0
    else if(optionIndex==int(currentMenu->getChildCount())-1 && encDir==1){return;} // Don't go above NUM_OPTIONS
    optionIndex+=encDir;
    highlight(optionIndex);
    Serial.print("optionIndex: ");
    Serial.println(optionIndex);
    Serial.print("textOffset: ");
    Serial.println(textOffset);
}

void DisplayManager::handleIconNavigation(int encDir){
    // Handle navigation when in icon mode
    Serial.println("handleIconNavigation");  
}

void DisplayManager::navigateForward(){
    Serial.println("navigateForward");
    // If current menu is not active, activate it
    if(!currentMenu->getActive()){
        show(currentMenu->activate());                   
        optionIndex = 0;
        highlight(optionIndex);
    }
    // If in text mode, switch to the highlighted menu
    else if(currentMenu->getMode()==MODE_TEXT){
        if(currentMenu->getChild(optionIndex) != nullptr){            
            currentMenu = currentMenu->getChild(optionIndex);            
            textOffset = 0; // Reset text offset when switching menus
            //if(currentMenu->getChildCount()>0) show(currentMenu->deactivate()); // Added logic to automatically activate childless menus when going forward
            //else show(currentMenu->activate());
            show(currentMenu->deactivate());
        }
    }
    else{
        Serial.println("UNHANDLED LOGIC: navButton press");
    }
}

void DisplayManager::navigateBackward(){
    // If current menu is not active, 
    if(!currentMenu->getActive() || currentMenu->getChildCount() < 1){ // Added OR statement to handle childless menus - 5/1/25
        //if(currentMenu->getParent() != nullptr){            
        currentMenu = currentMenu->getParent();
        if (currentMenu->getName() == "ROOT"){
            currentMenu = baseMenu;
            textOffset = 0; // Reset text offset when switching menus
            show(currentMenu->deactivate());
            return;
        }
        else{
            show(SSD1306_WHITE);
        }    
        show(currentMenu->activate());
    
    if(currentMenu->getMode() == MODE_TEXT){
        highlight(optionIndex);
    }          
    }        
    else{
        textOffset = 0; // Reset text offset when switching menus
        show(currentMenu->deactivate());
    }
}

// ==================================================================================
// Other Functions
// ==================================================================================

// #########################################################################
// Draw a circular or elliptical arc with a defined thickness
// #########################################################################

// x,y == coords of centre of arc
// start_angle = 0 - 359
// seg_count = number of 3 degree segments to draw (120 => 360 degree arc)
// rx = x axis radius
// yx = y axis radius
// w  = width (thickness) of arc in pixels
// colour = 16 bit colour value
// Note if rx and ry are the same then an arc of a circle is drawn


#define DEG2RAD 0.0174532925
void DisplayManager::fillArc2(int x, int y, int start_angle, int seg_count, int r, int w, bool colour)
{

    byte seg = 3; // Segments are 3 degrees wide = 120 segments for 360 degrees
    byte inc = 3; // Draw segments every 3 degrees, increase to 6 for segmented ring

    // Handle negative angles and segments
    if (seg_count < 0) {
        seg_count = -seg_count;
        start_angle -= (seg * seg_count);
    }

    // Calculate first pair of coordinates for segment start
    float sx = cos((start_angle - 90) * DEG2RAD);
    float sy = sin((start_angle - 90) * DEG2RAD);
    uint16_t x0 = sx * (r - w) + x;
    uint16_t y0 = sy * (r - w) + y;
    uint16_t x1 = sx * r + x;
    uint16_t y1 = sy * r + y;


    // Draw colour blocks every inc degrees
    for (int i = start_angle; i < start_angle + seg * seg_count; i += inc) {

        // Calculate pair of coordinates for segment end
        float sx2 = cos((i + seg - 90) * DEG2RAD);
        float sy2 = sin((i + seg - 90) * DEG2RAD);
        int x2 = sx2 * (r - w) + x;
        int y2 = sy2 * (r - w) + y;
        int x3 = sx2 * r + x;
        int y3 = sy2 * r + y;

        display.fillTriangle(x0, y0, x1, y1, x2, y2, colour);
        display.fillTriangle(x1, y1, x2, y2, x3, y3, colour);

        // Copy segment end to sgement start for next segment
        x0 = x2;
        y0 = y2;
        x1 = x3;
        y1 = y3;
    }  
}