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
        slotIcons[slot].push_back(icon);
        Serial.print("Added icon to slot: ");
        Serial.println(slot);
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
    if (slotIcons[slot].empty()) {
        //Serial.println("Error getIcon: no icons in slot");
        return nullptr;
    }
    //if (iconIndex+dir < 0) iconIndex = icons.size()-1;    

    // Handle wraparound
    if (slotIconIndex[slot]+dir < 0) slotIconIndex[slot] = slotIcons[slot].size()-1;
    else if (slotIconIndex[slot]+dir >= int(slotIcons[slot].size())) slotIconIndex[slot] = 0;
    // Update the icon index
    else slotIconIndex[slot] += dir;
    Serial.print("New Icon index: ");
    Serial.println(int(slotIconIndex[slot]));
    return &slotIcons[slot].at(slotIconIndex[slot]);
}

size_t MenusOLED::getIconCount(uint8_t slot){
    if (slot >= 4 || slot < 0) {
        Serial.println("Error getIconCount: slot out of bounds");
        return 0;
    }
    if (slotIcons[slot].empty()) {
        Serial.println("Error getIconCount: no icons in slot");
        return 0;
    }
    return slotIcons[slot].size();
};


// Control Functions ------------------------------------------------------------------------------
void MenusOLED::setSlot(
    uint8_t index, 
    int8_t min, 
    int8_t max, 
    int8_t step, 
    //float scale, 
    int8_t* ptr, 
    //float initialValue, 
    const char* name=nullptr,
    //EffectorTypes effectorType,
    std::function<float(float)> scalingFunction,
    std::function<void(float)> callback){
    if(index < 5) {
        if(slots[index] == nullptr) {
            slots[index] = new Slot();
        }        
        slots[index]->minVal = min;
        slots[index]->maxVal = max;
        slots[index]->step = step;
        //slots[index]->scale = scale;
        slots[index]->effector = ptr;
        //slots[index]->value = initialValue;
        slots[index]->name = name;
        slots[index]->scalingFunction = scalingFunction;
        slots[index]->onValueChange = callback; // Store the callback
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

/*
void MenusOLED::setPtr(uint8_t index, float* ptr){    
    if(index < 5) {
        slots[index]->effector = ptr;
    }
    else {
        Serial.println("Error setPtr: index out of bounds");
    }
}
*/

void MenusOLED::writeSlot(uint8_t index, int8_t dir){    
    if(index >= 4 || index < 0) {
        Serial.println("Error writeSlot: index out of bounds");
        return;
    }
    if (slots[index] == nullptr) {
        Serial.print("Error writeSlot: slot is null: ");
        Serial.println(index);
        return;
    }
    // If the effector exists, update it
    if (slots[index]->effector != nullptr) {

        int8_t val = int8_t(dir*slots[index]->step);
        Serial.print("val: ");
        Serial.println(val);
        
        // Ensure the value is within the min and max range        
        if (*slots[index]->effector + val < slots[index]->minVal) {
            //slots[index]->value = slots[index]->max;
            *slots[index]->effector = slots[index]->minVal;
        }
        else if (*slots[index]->effector + val > slots[index]->maxVal) {
            //slots[index]->effector = slots[index]->min;
            *slots[index]->effector = slots[index]->maxVal;            
        }
        else {
            *slots[index]->effector += val;
        }

        float scaledVal = slots[index]->scalingFunction(*slots[index]->effector);
        
        Serial.print("Effector val: ");
        Serial.println(*slots[index]->effector);        

        // Trigger the callback
        if (slots[index]->onValueChange != nullptr) {
            slots[index]->onValueChange(scaledVal);
            Serial.print("Scaled val: ");
            Serial.println(scaledVal);
        }    
    }
    
    else {
        Serial.print("No pointer set! ");
        Serial.println(index);
    }
    
    //return slots[index]->effectorType;
}


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
