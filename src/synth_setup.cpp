#include <synth_setup.h>

// ================================================================================================
// Standard hardware IO. Do not change without good reason
// ================================================================================================
PCA9555 pca0(ADDR_PCA0);
PCA9555 pca1(ADDR_PCA1);
PCA9555 pca2(ADDR_PCA2);

Encoder blkEnc(pca0);
Encoder bluEnc(pca0);
Encoder grnEnc(pca1);
Encoder ylwEnc(pca1);
Encoder ongEnc(pca2);

int8_t blkDir = 0;
int8_t bluDir = 0;
int8_t grnDir = 0;
int8_t ylwDir = 0;
int8_t ongDir = 0;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// End standard hardware IO -----------------------------------------------------------------------

// ================================================================================================
// Setup Menu screens
// ================================================================================================
String oscOptions[] = {"ADD","CONFIG","ROUTE","DELETE","","","","","","","","","","","",""};
String envOptions[] = {"ADD","CONFIG","ROUTE","DELETE","","","","","","","","","","","",""};

MenusOLED oscAdd("OSC ADD",display,oscBMP);
MenusOLED oscConfig("OSC CONFIG",display,oscBMP);
MenusOLED oscRoute("OSC ROUTE",display,oscBMP);
MenusOLED oscDelete("OSC DELETE",display,oscBMP);
//MenusOLED oscFour("FOUR",display,NULL);
//MenusOLED oscFive("FIVE",display,NULL);
//MenusOLED oscSix("SIX",display,NULL);
//MenusOLED oscSeven("SEVEN",display,NULL);
MenusOLED oscArr[] = {
    oscAdd,
    oscConfig,
    oscRoute,
    oscDelete
    //oscFour,
    //oscFive,
    //oscSix,
    //oscSeven
};


MenusOLED envConfig("ENV CONFIG",display,envBMP);
MenusOLED envAdd("ENV ADD",display,envBMP);
MenusOLED envDelete("ENV DELETE",display,envBMP);
MenusOLED envRoute("ENV ROUTE",display,envBMP);
MenusOLED envFour("FOUR",display,NULL);
MenusOLED envArr[] = {
    envAdd,
    envConfig,
    envRoute,
    envDelete
    //envFour
};

MenusOLED menuHome("HOME", display,homeBMP);
MenusOLED menuKeys("KEYS", display,keysBMP);
MenusOLED menuOSC(MODE_TEXT,"OSCILLATORS",display,oscBMP,oscOptions,oscArr);
MenusOLED menuNSH("NOISE, S&H", display,noiseSHBMP);
MenusOLED menuMIX("MIXER", display,mixBMP);
MenusOLED menuFLT("FILTER", display,fltBMP);
MenusOLED menuENV(MODE_TEXT,"ENVELOPE", display,envBMP, envOptions,envArr);
MenusOLED menuEFF("EFFECTS", display,keysBMP);
//MenusOLED modMatrix("MOD MATRIX",display,gridBMP);

// End setup menu screens -------------------------------------------------------------------------

// Show the Teensynth logo and QR code for 3 seconds on startup
void startupScreen(){
    display.clearDisplay();
    display.cp437(true);         // Use full 256 char 'Code Page 437' font
    display.setTextSize(1);  
    display.setTextColor(SSD1306_WHITE,SSD1306_BLACK);
    display.setCursor(40, 20);
    display.println(APP_NAME);
    display.setCursor(40, 28);
    display.print("fw v");
    display.print(APP_VERSION);
    display.setCursor(40,36);
    display.println(APP_DATE);
    display.setCursor(40, 44);
    display.print("by ");
    display.print(APP_AUTHOR);
    display.drawBitmap(0,18,qrCode,35,35,SSD1306_WHITE);
    display.drawBitmap(0,0,teensynthlogo,128,16,SSD1306_WHITE);
    display.display();
    
    delay(3000);
}

// Configures the PCA9555s and the encoders, sets up the display
void ioSetup(){
    //setup PCAs    
    pca0.config(PCA0_IO0,PCA0_IO1);
    pca1.config(PCA1_IO0,PCA1_IO1);
    pca2.config(PCA2_IO0,PCA2_IO1);

    //setup Encoders
    blkEnc.config(6,5,4,1);
    bluEnc.config(1,2,3,1);
    grnEnc.config(6,5,4,1);
    ylwEnc.config(1,2,3,1);
    ongEnc.config(7,6,5,1);

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
    }
}

// Set up the menu structure
// This is a bit of a mess, but it works.
// The idea is to have a parent menu and a child menu, with the ability to navigate
// between them. The parent menu is the one that is currently displayed, and the child
// menu is the one that is currently selected. The child menu can be a sub-menu of the
// parent menu, or it can be a completely different menu. The child menu can also have
// its own child menus, and so on. The idea is to have a tree structure of menus, with
// the ability to navigate between them.
void screenSetup(){
    //menuOSC.setChildren(oscArr);  
    oscConfig.setNeighbors(NULL,NULL,&menuOSC);  
    oscAdd.setNeighbors(NULL,NULL,&menuOSC);
    oscConfig.setNeighbors(NULL,NULL,&menuOSC);
    oscRoute.setNeighbors(NULL,NULL,&menuOSC);  
    
    envConfig.setParent(&menuENV);
    envAdd.setParent(&menuENV);
    envConfig.setParent(&menuENV);
    envRoute.setParent(&menuENV);  
    
    menuHome.setNeighbors(&menuEFF,&menuKeys,&menuHome);
    menuKeys.setNeighbors(&menuHome,&menuOSC,&menuHome);
    menuOSC.setNeighbors(&menuKeys,&menuNSH,&menuHome);//&menuNSH);
    menuNSH.setNeighbors(&menuOSC,&menuMIX,&menuHome);
    menuMIX.setNeighbors(&menuNSH,&menuFLT,&menuHome);
    menuFLT.setNeighbors(&menuMIX,&menuENV,&menuHome);
    menuENV.setNeighbors(&menuFLT,&menuEFF,&menuHome);
    menuEFF.setNeighbors(&menuENV,&menuHome,&menuHome);
}