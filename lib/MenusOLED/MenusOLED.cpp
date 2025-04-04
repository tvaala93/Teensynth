#include <MenusOLED.h>

// Display Functions --------------------------------------------------------------------------

/**
 * @brief Shows the menu name and logo in the top bar. If menu is text-based those options show
 * 
 * @param color indicates the menu name text color. 0= black text on white background 
 */
void MenusOLED::show(bool color){
    //reset menu area of display
    display.clearDisplay();
    display.fillRect(0,0,SCREEN_WIDTH,9,color);
    
    // Print menu name
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(menuName, 128, 0, &x1, &y1, &w, &h); //calc width of new string    
    display.setCursor((128 - w) / 2, 1);
    display.setTextColor(!color,color);
    display.print(menuName);

    // Print Arrows Left / Right        
    if(!isActive){
        if(getLeft() != nullptr){
            display.setCursor(1,1);
            display.write(0x11);
        }
        if(getRight() != nullptr){
            display.setCursor(122,1);
            display.write(0x10);
        }
    }
    
    // Print logo
    if(menuLogo != nullptr){
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
    // Display visible options
    for (size_t i = textOffset; i < children.size(); ++i) {
        int yOffset = 8 * (i - textOffset) + 17;
        printText(yOffset, String(i) + ": " + children[i]->getName(), false);
        /*
        if (yOffset >= SCREEN_HEIGHT) {
            drawArrows(true, true); // Show both arrows if there are items above and below
            break;
        }
        */        
    }

    // Show arrows if there are more items above or below
    drawArrows(textOffset > 0, textOffset + 6 < int(children.size()));

    display.display(); // Update the display
}

void MenusOLED::showGraphic(){}

/**
 * @brief Highlights a particular text option on screen
 * 
 * @param index index from optionList to highlight
 */
void MenusOLED::highlight(int8_t index){        
    if (mode != MODE_TEXT || index < 0 || index >= int(children.size())) {
        return; // Invalid index or mode
    }

    // Adjust textOffset if the highlighted option is out of view
    if (index < textOffset) {
        textOffset = index;
    } else if (8 * (index - textOffset) + 17 >= SCREEN_HEIGHT) {
        textOffset = index - 5; // Ensure the highlighted option is visible
    }

    showText(); // Redraw the text area

    // Highlight the selected option
    int yOffset = 8 * (index - textOffset) + 17;
    printText(yOffset, String(index) + ": " + children[index]->getName(), true);
    
    // Show arrows if there are more items above or below
    drawArrows(textOffset > 0, textOffset + 6 < int(children.size()));

    display.display(); // Update the display
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

/*
bool MenusOLED::testText(uint8_t i){
    if(optionList[i]=="") return false;
    return true;
}
*/

uint8_t MenusOLED::getMode(){
    return mode;
}


// Tree Traversal Functions -------------------------------------------------------------------
/*
void MenusOLED::setParent(MenusOLED* par){
    parent = {par};
}
*/

MenusOLED* MenusOLED::getParent(){
    return parent;
}

/*
void MenusOLED::setNeighbors(MenusOLED* left, MenusOLED* right, MenusOLED* par){
    leftSibling = left;
    rightSibling = right;
    parent = par;
}
*/

MenusOLED* MenusOLED::getLeft(){        
    if (!parent) return nullptr; // No parent means no siblings
    auto& siblings = parent->children;
    for (size_t i = 0; i < siblings.size(); ++i) {
        if (siblings[i] == this) {
            if (i == 0) return siblings[siblings.size()-1]; // Wrap around to the last sibling
            // If not the first sibling, return the previous one
            // Note: This assumes that the vector is not empty and has at least one sibling
            return siblings[i - 1];
        }
    }
    return nullptr; // No left sibling
}

MenusOLED* MenusOLED::getRight(){
    if (!parent) return nullptr; // No parent means no siblings
    auto& siblings = parent->children;
    for (size_t i = 0; i < siblings.size(); ++i) {
        if (siblings[i] == this) {
            if(i == siblings.size()-1) return siblings[0]; // Wrap around to the first sibling
            // If not the last sibling, return the next one
            // Note: This assumes that the vector is not empty and has at least one sibling
            return siblings[i + 1];
        }
    }
    return nullptr; // No right sibling
}

MenusOLED* MenusOLED::getChild(uint8_t index){        
    if (index < children.size()) {
        return children[index];
    }
    return nullptr; // Return nullptr if the index is out of bounds
}

void MenusOLED::addChild(MenusOLED* child) {
    child->parent = this; // Set the parent of the child
    children.push_back(child); // Add the child to the vector
}

size_t MenusOLED::getChildCount() {
    return children.size();
}


// Utility Functions --------------------------------------------------------------------------
String MenusOLED::getName(){
    return menuName;
}

MenusOLED* MenusOLED::findMenuByName(const String& name) {
    if (menuName == name) return this;
    for (auto* child : children) {
        MenusOLED* result = child->findMenuByName(name);
        if (result) return result;
    }
    return nullptr;
}

void MenusOLED::drawArrows(bool showUp, bool showDown) {
    display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    if (showUp) {
        display.setCursor(121, 17); // Position for the up arrow
        display.write(0x1e);       // Up arrow character
    }
    if (showDown) {
        display.setCursor(121, 59); // Position for the down arrow
        display.write(0x1f);        // Down arrow character
    }
}

void MenusOLED::printText(int yOffset, const String& text, bool highlight) {
    if (highlight) {
        display.fillRect(0, yOffset, SCREEN_WIDTH, 8, SSD1306_WHITE);
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    } else {
        display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    }
    display.setCursor(0, yOffset);
    display.print(text);
}

MenusOLED::~MenusOLED(){}

// ===================================================================================
// Helper Functions
// ===================================================================================

void drawIcon(Icon icon, Adafruit_SSD1306& display){
    // Draw the icon on the display
    display.drawBitmap(icon.x-MSPRITE_WIDTH/2, icon.y, icon.bmp, MSPRITE_WIDTH, MSPRITE_HEIGHT, SSD1306_WHITE);
    display.drawCircle(icon.x, icon.y + MSPRITE_HEIGHT/2, MSPRITE_WIDTH/2, SSD1306_WHITE);

    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(icon.label, 128, 0, &x1, &y1, &w, &h);
    display.setCursor(icon.x - w / 2, icon.y + MSPRITE_WIDTH);
    display.print(icon.label);    

    display.display();    
}
