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

void MenusOLED::addIcon(int slot, Icon icon) {
    if(slot < 4){
        slotIcons[slot]->push_back(icon);
        return;
    }
    else{
        Serial.println("Error addIcon: slot out of bounds");
        return;
    }    
    //drawIcon(icon, display);
}

Icon* MenusOLED::getIcon(int slot, int8_t dir) {
    // Valdiate input
    if (slot >= 4 || slot < 0) {
        Serial.println("Error getIcon: slot out of bounds");
        return nullptr;
    }
    if (slotIcons[slot]->empty()) {
        Serial.println("Error getIcon: no icons in slot");
        return nullptr;
    }
    //if (iconIndex+dir < 0) iconIndex = icons.size()-1;    

    // Handle wraparound
    if (slotIconIndex[slot]+dir < 0) slotIconIndex[slot] = slotIcons[slot]->size()-1;
    else if (slotIconIndex[slot]+dir >= int(slotIcons[slot]->size())) slotIconIndex[slot] = 0;
    // Update the icon index
    else slotIconIndex[slot] += dir;
    Serial.print("New Icon index: ");
    Serial.println(int(slotIconIndex[slot]));
    return &slotIcons[slot]->at(slotIconIndex[slot]);
}

size_t MenusOLED::getIconCount(uint8_t slot){
    if (slot >= 4 || slot < 0) {
        Serial.println("Error getIconCount: slot out of bounds");
        return 0;
    }
    if (slotIcons[slot]->empty()) {
        Serial.println("Error getIconCount: no icons in slot");
        return 0;
    }
    return slotIcons[slot]->size();
};


// Control Functions ------------------------------------------------------------------------------
void MenusOLED::setSlot(uint8_t index, int8_t min, int8_t max, int8_t step, float scale, float* ptr){
    if(index < 5) {
        slots[index]->min = min;
        slots[index]->max = max;
        slots[index]->step = step;
        slots[index]->scale = scale;
        slots[index]->effector = ptr;
    }
    else {
        Serial.println("Error setSlot: index out of bounds");
    }
}

Slot * MenusOLED::getSlot(uint8_t index){
    if(index < 5) {
        return slots[index];
    }
    else {
        Serial.println("Error getSlot: index out of bounds");
        return nullptr;
    }
}

void MenusOLED::setPtr(uint8_t index, float* ptr){    
    if(index < 5) {
        slots[index]->effector = ptr;
    }
    else {
        Serial.println("Error setPtr: index out of bounds");
    }
}

void MenusOLED::writeSlot(uint8_t index, int8_t dir){
    if (slots[index]->effector) {
        int val = int(dir*slots[index]->step);
        // Ensure the value is within the min and max range
        // Wrapping the value around if it exceeds the limits
        if (slots[index]->value + val < slots[index]->min) {
            slots[index]->value = slots[index]->max;
        }
        else if (slots[index]->value + val > slots[index]->max) {
            slots[index]->value = slots[index]->min;
        }
        else {
            slots[index]->value += val * slots[index]->step;
        }
        // Update the value
        *slots[index]->effector = slots[index]->value;
        slots[index]->value += val;
        Serial.print("Blue Encoder: ");
        Serial.println(*slots[index]->effector);
    }
    else {
        Serial.print("No pointer set! ");
        Serial.println(index);
    }
};


// Utility Functions ------------------------------------------------------------------------------
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
