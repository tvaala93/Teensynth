#include <MenusOLED.h>

// =================================================================================
// MenusOLED Functions
// =================================================================================

bool MenusOLED::getActive(){
    return isActive;
}

bool MenusOLED::activate(){
    isActive = true;
    if(mode==MODE_DEFAULT){
        //show(1);
        return 1;
    }    
    //show(0);
    return 0;

    // If textMenu, circle/highlight 
}

bool MenusOLED::deactivate(){
    isActive = false;
    //textOffset = 0;
    //show(1);
    return 1;
}

uint8_t MenusOLED::getMode(){
    return mode;
}


// Tree Traversal Functions -------------------------------------------------------------------

MenusOLED* MenusOLED::getParent(){
    return parent;
}

MenusOLED* MenusOLED::getSibling(int dir){
    if (!parent) return nullptr; // No parent means no siblings
    auto& siblings = parent->children;
    for (size_t i = 0; i < siblings.size(); ++i) {
        if (siblings[i] == this) {
            int j = i + dir;
            if (j < 0) j = siblings.size() - 1; // Wrap around to the last sibling
            else if (j >= int(siblings.size())) j = 0; // Wrap around to the first sibling
            return siblings[j]; // Return the sibling in the specified direction
        }
    }    
    return nullptr; // Invalid direction
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


// Graphic Functions -------------------------------------------------------------------------

const unsigned char* MenusOLED::getLogo(){
    return menuLogo;
}

void MenusOLED::addIcon(Icon icon) {
    icons.push_back(icon);
    //drawIcon(icon, display);
}

Icon MenusOLED::getIcon(int8_t dir) {    
    if (iconIndex+dir < 0) iconIndex = icons.size()-1;
    else if (iconIndex+dir >= int(icons.size())) iconIndex = 0;
    else iconIndex += dir;
    Serial.print("New Icon index: ");
    Serial.println(iconIndex);
    return icons[iconIndex];
}

size_t MenusOLED::getIconCount(){
    return icons.size();
};

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

MenusOLED::~MenusOLED(){}

// ===================================================================================
// Helper Functions
// ===================================================================================
