#include <synth_setup_mini.h>

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

// End standard hardware IO -----------------------------------------------------------------------


// ================================================================================================
// Setup Audio
// ================================================================================================

AudioOutputI2S           i2s1;
AudioSynthWaveform       waveform0;
AudioSynthWaveform       waveform1;
AudioSynthWaveform       waveform2;
AudioSynthWaveform       waveform3;
AudioSynthWaveform       lowFreqOsc0;
AudioSynthWaveform       lowFreqOsc1;
AudioEffectEnvelope      envelopeAmp;
AudioEffectEnvelope      envelopeFilter;
AudioMixer4              mixer1;
AudioFilterLadder        ladder1;         
AudioConnection          patchCord1(waveform2, 0, mixer1, 1);
AudioConnection          patchCord2(waveform3, 0, mixer1, 2);
AudioConnection          patchCord3(waveform1, 0, mixer1, 0);
AudioConnection          patchCord4(mixer1, 0, ladder1, 0);
AudioConnection          patchCord5(ladder1, 0, i2s1, 0);
AudioConnection          patchCord6(ladder1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     

/*
int asdf = WAVEFORM_SINE;
asdf = WAVEFORM_SAWTOOTH;
asdf = WAVEFORM_SQUARE;
asdf = WAVEFORM_TRIANGLE;
asdf = WAVEFORM_ARBITRARY;
asdf = WAVEFORM_PULSE;
asdf = WAVEFORM_SAWTOOTH_REVERSE;
asdf = WAVEFORM_SAMPLE_HOLD;
asdf = WAVEFORM_TRIANGLE_VARIABLE;
*/

// Oscillator params
Oscillator osc0Params = {440.0, WAVEFORM_SINE, 3, 0.0, 0.5, 0.5};
Oscillator osc1Params = {440.0, WAVEFORM_SINE, 3, 0.0, 0.5, 0.5};
Oscillator osc2Params = {440.0, WAVEFORM_SINE, 3, 0.0, 0.5, 0.5};
Oscillator osc3Params = {440.0, WAVEFORM_SINE, 3, 0.0, 0.5, 0.5};
// LFO params
LFO lfo0Params = {1.0, WAVEFORM_SINE, 1.0, 0.5, 0.5};
LFO lfo1Params = {1.0, WAVEFORM_SINE, 1.0, 0.5, 0.5};
// Filter params
Filter filterParams = {1000.0, 0.5};
// Envelope params
Envelope envAmpParams = {0.1, 0.1, 0.5, 0.1};
Envelope envFltParams = {0.1, 0.1, 0.5, 0.1};

// End setup audio --------------------------------------------------------------------------------


// ================================================================================================
// Setup Menu screens
// ================================================================================================

//std::vector<String> oscOptions = {"ADD","CONFIG","ROUTE","DELETE"};
//std::vector<String> envOptions = {"ADD","CONFIG","ROUTE","DELETE"};

//TODO test dynamic menu creation and deletion
//TODO test home menu modes: when in perform mode, have hotkeys for each menuscreen active

std::vector<Icon> testIcons = {
    {sawtoothBMP, "Wave",nullptr},
    {squareBMP, "Octv",nullptr},
    {triangleBMP, "Tune",nullptr},
    {sineBMP, "Gain",nullptr}
};

std::vector<Icon> waveIcons = {
    {sawtoothBMP, "Wave",nullptr},
    {squareBMP, "Octv",nullptr},
    {triangleBMP, "Tune",nullptr},
    {sineBMP, "Gain",nullptr}
};

MenusOLED menuRoot(MODE_DEFAULT,"ROOT",NULL);

MenusOLED menuHome(MODE_TEXT,"Home", homeBMP);
    MenusOLED homeConfig(MODE_TEXT,"Configure Mode", homeBMP);
    MenusOLED homePerform(MODE_TEXT,"Perform Mode", homeBMP);
    MenusOLED homeRecord(MODE_TEXT,"Record Mode", homeBMP);
MenusOLED menuKeys(MODE_DEFAULT,"Keys", keysBMP);
MenusOLED menuOSC(MODE_TEXT,"Oscillators",oscBMP);
    MenusOLED osc0(MODE_ICON,"OSC 0",oscBMP);
    MenusOLED osc1(MODE_ICON,"OSC 1",oscBMP);
    MenusOLED osc2(MODE_ICON,"OSC 2",oscBMP);
    MenusOLED osc3(MODE_ICON,"OSC 3",oscBMP);
    MenusOLED lfo0(MODE_ICON,"LFO",oscBMP);
    /*
    MenusOLED oscAdd(MODE_TEXT,"Add",oscBMP);
        MenusOLED oscAdd0(MODE_DEFAULT,"Voice 0",oscBMP);
        MenusOLED oscAdd1(MODE_DEFAULT,"Voice 1",oscBMP);
        MenusOLED oscAdd2(MODE_DEFAULT,"Voice 2",oscBMP);
        MenusOLED oscAdd3(MODE_DEFAULT,"Voice 3",oscBMP);
        MenusOLED oscAddLFO(MODE_DEFAULT,"LFOs",oscBMP);
    MenusOLED oscConfig(MODE_ICON,"Configure",oscBMP);          
    MenusOLED oscRoute(MODE_DEFAULT,"Route",oscBMP);
    MenusOLED oscDelete(MODE_DEFAULT,"Delete",oscBMP);
    */
MenusOLED menuNSH(MODE_DEFAULT,"Noise, S&H", noiseSHBMP);
MenusOLED menuMIX(MODE_ICON,"Mixer", mixBMP);    
MenusOLED menuFLT(MODE_DEFAULT,"Filter", fltBMP);
MenusOLED menuENV(MODE_TEXT,"Envelopes", envBMP);
    MenusOLED envFilter(MODE_ICON,"ENV for FLT",envBMP);
    MenusOLED envAmp(MODE_ICON,"ENV for VCA",envBMP);
    MenusOLED envLFO(MODE_ICON,"ENV for LFO",envBMP);    
MenusOLED menuEFF(MODE_TEXT,"Effects", keysBMP);
    MenusOLED effFade(MODE_DEFAULT,"Fade",keysBMP);
    MenusOLED effDelay(MODE_DEFAULT,"Delay",keysBMP);
    MenusOLED effReverb(MODE_DEFAULT,"Reverb",keysBMP);
    MenusOLED effChorus(MODE_DEFAULT,"Chrous",keysBMP);
    MenusOLED effFlanger(MODE_DEFAULT,"Flanger",keysBMP);
    MenusOLED effDist(MODE_DEFAULT,"Distortion",keysBMP);
    MenusOLED effBitCrush(MODE_DEFAULT,"Bit Crusher",keysBMP);
    MenusOLED effChop(MODE_DEFAULT,"Chop",keysBMP);
    MenusOLED effRingMod(MODE_DEFAULT,"Ring Mod",keysBMP);
    MenusOLED effWah(MODE_DEFAULT,"Wah",keysBMP);
    MenusOLED effPhaser(MODE_DEFAULT,"Phaser",keysBMP);
//MenusOLED modMatrix("MOD MATRIX",display,gridBMP);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DisplayManager dispMgr(display, &menuHome);

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
    
    delay(2000);
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

void setupRootMenu(){
    menuRoot.addChild(&menuHome);
    menuRoot.addChild(&menuKeys);
    menuRoot.addChild(&menuOSC);
    menuRoot.addChild(&menuNSH);
    menuRoot.addChild(&menuMIX);
    menuRoot.addChild(&menuFLT);
    menuRoot.addChild(&menuENV);
    menuRoot.addChild(&menuEFF);
}

void setupHomeMenu(){
    menuHome.addChild(&homeConfig);
    menuHome.addChild(&homePerform);
    menuHome.addChild(&homeRecord);
}

void setupOscillatorMenu(){
    menuOSC.addChild(&osc0);
    menuOSC.addChild(&osc1);
        osc1.addIcon(0,waveIcons[0]);
        osc1.addIcon(0,waveIcons[1]);
        osc1.addIcon(0,waveIcons[2]);
        osc1.addIcon(0,waveIcons[3]);  
    menuOSC.addChild(&osc2);
    menuOSC.addChild(&osc3);
    menuOSC.addChild(&lfo0);    
}

void setupMixerMenu(){
    return;
    /*
    menuMIX.addChild(&mixMain);
    menuMIX.addChild(&mix0);
    menuMIX.addChild(&mix1);
    menuMIX.addChild(&mix2);
    menuMIX.addChild(&mix3);
    */
}

void setupEnvelopeMenu(){
    menuENV.addChild(&envFilter);
    menuENV.addChild(&envAmp);
    menuENV.addChild(&envLFO);    
}

void setupEffectsMenu(){
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

void screenSetup(){
    setupRootMenu();
    setupHomeMenu();
    setupOscillatorMenu();
    setupMixerMenu();
    setupEnvelopeMenu();
    setupEffectsMenu();
}