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

void DisplayManager::showGraphic(){
    // Clear the display work area?
    Serial.println("show Graphic");
    //display.fillRect(0, 9, SCREEN_WIDTH, SCREEN_HEIGHT - 8, SSD1306_BLACK);
    
    // Draw each icon
    for (int i = 0; i < int(currentMenu->getIconCount()); ++i) {
        drawIcon(currentMenu->getIcon(i));
    }    
    // Show the display
    display.display();
}

/**
 * @brief Highlights a particular text option on screen
 * 
 * @param index index from optionList to highlight
 */
void DisplayManager::highlight(int8_t index){        
    if (currentMenu->getMode() != MODE_TEXT || index < 0 || index >= int(currentMenu->getChildCount())) {
        Serial.println("Invalid index or mode");
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

void DisplayManager::drawIcon(Icon icon){
    // Draw the icon on the display
    display.fillCircle(icon.x, icon.y + MSPRITE_HEIGHT/2, MSPRITE_WIDTH/2, SSD1306_BLACK);
    display.drawBitmap(icon.x-MSPRITE_WIDTH/2, icon.y, icon.bmp, MSPRITE_WIDTH, MSPRITE_HEIGHT, SSD1306_WHITE);
    display.drawCircle(icon.x, icon.y + MSPRITE_HEIGHT/2, MSPRITE_WIDTH/2, SSD1306_WHITE);

    int16_t x1, y1;
    uint16_t w, h;
    display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    // Calculate the width and height of the label
    display.getTextBounds(icon.label, 128, 0, &x1, &y1, &w, &h);
    display.fillRect(icon.x - w / 2, icon.y + MSPRITE_WIDTH, w, h, SSD1306_BLACK);
    display.setCursor(icon.x - w / 2, icon.y + MSPRITE_WIDTH);
    display.print(icon.label);    

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
                Serial.println("Default mode");
                break;      
            case MODE_TEXT:
                handleTextNavigation(encDir);
                break;
            case MODE_GRAPHIC:
                handleGraphicNavigation(encDir);
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

void DisplayManager::handleGraphicNavigation(int encDir){
// Handle navigation when in graphic mode
Serial.println("handleGraphicNavigation");  
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
        //lastMenu = currentMenu;            
        currentMenu = currentMenu->getChild(optionIndex);            
        textOffset = 0; // Reset text offset when switching menus
        show(currentMenu->deactivate());        
        }
    }
    else{
        Serial.println("UNHANDLED LOGIC: navButton press");
    }
}

void DisplayManager::navigateBackward(){
    // If current menu is not active, 
    if(!currentMenu->getActive()){          
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