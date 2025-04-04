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

//std::vector<String> oscOptions = {"ADD","CONFIG","ROUTE","DELETE"};
//std::vector<String> envOptions = {"ADD","CONFIG","ROUTE","DELETE"};

//TODO test dynamic menu creation and deletion
//TODO test home menu modes: when in perform mode, have hotkeys for each menuscreen active

MenusOLED menuRoot(MODE_DEFAULT,"ROOT",display,NULL);

MenusOLED menuHome(MODE_TEXT,"Home", display,homeBMP);
    MenusOLED homeConfig(MODE_TEXT,"Configure Mode", display,homeBMP);
    MenusOLED homePerform(MODE_TEXT,"Perform Mode", display,homeBMP);
    MenusOLED homeRecord(MODE_TEXT,"Record Mode", display,homeBMP);
MenusOLED menuKeys(MODE_DEFAULT,"Keys", display,keysBMP);
MenusOLED menuOSC(MODE_TEXT,"Oscillators",display,oscBMP);
    MenusOLED oscAdd(MODE_TEXT,"Add",display,oscBMP);
        MenusOLED oscAdd0(MODE_DEFAULT,"Voice 0",display,oscBMP);
        MenusOLED oscAdd1(MODE_DEFAULT,"Voice 1",display,oscBMP);
        MenusOLED oscAdd2(MODE_DEFAULT,"Voice 2",display,oscBMP);
        MenusOLED oscAdd3(MODE_DEFAULT,"Voice 3",display,oscBMP);
        MenusOLED oscAddLFO(MODE_DEFAULT,"LFOs",display,oscBMP);
    MenusOLED oscConfig(MODE_DEFAULT,"Configure",display,oscBMP);
    MenusOLED oscRoute(MODE_DEFAULT,"Route",display,oscBMP);
    MenusOLED oscDelete(MODE_DEFAULT,"Delete",display,oscBMP);
MenusOLED menuNSH(MODE_DEFAULT,"Noise, S&H", display,noiseSHBMP);
MenusOLED menuMIX(MODE_TEXT,"Mixer", display,mixBMP);
    MenusOLED mixMain(MODE_DEFAULT,"Main", display,mixBMP);
    MenusOLED mix0(MODE_DEFAULT,"Voice 0", display,mixBMP);
    MenusOLED mix1(MODE_DEFAULT,"Voice 1", display,mixBMP);
    MenusOLED mix2(MODE_DEFAULT,"Voice 2", display,mixBMP);
    MenusOLED mix3(MODE_DEFAULT,"Voice 3", display,mixBMP);
MenusOLED menuFLT(MODE_DEFAULT,"Filter", display,fltBMP);
MenusOLED menuENV(MODE_TEXT,"Envelopes", display,envBMP);
    MenusOLED envConfig(MODE_DEFAULT,"Configure",display,envBMP);
    MenusOLED envAdd(MODE_DEFAULT,"Add",display,envBMP);
    MenusOLED envRoute(MODE_DEFAULT,"Route",display,envBMP);
    MenusOLED envDelete(MODE_DEFAULT,"Delete",display,envBMP);
MenusOLED menuEFF(MODE_TEXT,"Effects", display,keysBMP);
    MenusOLED effFade(MODE_DEFAULT,"Fade",display,keysBMP);
    MenusOLED effDelay(MODE_DEFAULT,"Delay",display,keysBMP);
    MenusOLED effReverb(MODE_DEFAULT,"Reverb",display,keysBMP);
    MenusOLED effChorus(MODE_DEFAULT,"Chrous",display,keysBMP);
    MenusOLED effFlanger(MODE_DEFAULT,"Flanger",display,keysBMP);
    MenusOLED effDist(MODE_DEFAULT,"Distortion",display,keysBMP);
    MenusOLED effBitCrush(MODE_DEFAULT,"Bit Crusher",display,keysBMP);
    MenusOLED effChop(MODE_DEFAULT,"Chop",display,keysBMP);
    MenusOLED effRingMod(MODE_DEFAULT,"Ring Mod",display,keysBMP);
    MenusOLED effWah(MODE_DEFAULT,"Wah",display,keysBMP);
    MenusOLED effPhaser(MODE_DEFAULT,"Phaser",display,keysBMP);
//MenusOLED modMatrix("MOD MATRIX",display,gridBMP);

std::vector<Icon> waveIcons = {
    {sawtoothBMP, "WAVE", 16, 32},
    {squareBMP, "OCTV", 48, 32},
    {triangleBMP, "TUNE", 80, 32},
    {sineBMP, "GAIN", 112, 32}
};

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
// The idea is to have a parent menu and a child menu, with the ability to navigate
// between them. The parent menu is the one that is currently displayed, and the child
// menu is the one that is currently selected. The child menu can be a sub-menu of the
// parent menu, or it can be a completely different menu. The child menu can also have
// its own child menus, and so on. The idea is to have a tree structure of menus, with
// the ability to navigate between them.
void screenSetup(){
    // Setup Root children
    menuRoot.addChild(&menuHome);
    menuRoot.addChild(&menuKeys);
    menuRoot.addChild(&menuOSC);
    menuRoot.addChild(&menuNSH);
    menuRoot.addChild(&menuMIX);
    menuRoot.addChild(&menuFLT);
    menuRoot.addChild(&menuENV);
    menuRoot.addChild(&menuEFF);

    // Setup menuHome children
    menuHome.addChild(&homeConfig);
    menuHome.addChild(&homePerform);
    menuHome.addChild(&homeRecord);

    // Setup menuOSC children
    menuOSC.addChild(&oscAdd);
        oscAdd.addChild(&oscAdd0);
        oscAdd.addChild(&oscAdd1);
        oscAdd.addChild(&oscAdd2);
    menuOSC.addChild(&oscConfig);
    menuOSC.addChild(&oscRoute);
    menuOSC.addChild(&oscDelete);

    // Setup menuMIX children
    menuMIX.addChild(&mixMain);
    menuMIX.addChild(&mix0);
    menuMIX.addChild(&mix1);
    menuMIX.addChild(&mix2);
    menuMIX.addChild(&mix3);

    // Setup menuENV children
    menuENV.addChild(&envAdd);
    menuENV.addChild(&envConfig);
    menuENV.addChild(&envRoute);
    menuENV.addChild(&envDelete);

    // Setup menuEFF children
    menuEFF.addChild(&effFade);
    menuEFF.addChild(&effDelay);
    menuEFF.addChild(&effReverb);
    menuEFF.addChild(&effChorus);
    menuEFF.addChild(&effFlanger);
    menuEFF.addChild(&effDist);
    menuEFF.addChild(&effBitCrush);
    menuEFF.addChild(&effChop);
    menuEFF.addChild(&effRingMod);
    menuEFF.addChild(&effWah);
    menuEFF.addChild(&effPhaser);
}