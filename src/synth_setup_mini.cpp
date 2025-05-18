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
AudioEffectEnvelope      envelopeLFO;
AudioMixer4              mixer1;
AudioFilterLadder        ladder1;         
AudioConnection          patchCord0(waveform0, 0, mixer1, 0);
AudioConnection          patchCord1(waveform1, 0, mixer1, 1);
AudioConnection          patchCord2(waveform2, 0, mixer1, 2);
AudioConnection          patchCord3(waveform3, 0, mixer1, 3);
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
Oscillator vco0 = {&waveform0,WAVEFORM_SINE,3,0,0.50,100};
Oscillator vco1 = {&waveform1,WAVEFORM_SINE,3,0,0.50,100};
Oscillator vco2 = {&waveform2,WAVEFORM_SINE,3,0,0.50,100};
Oscillator vco3 = {&waveform3,WAVEFORM_SINE,3,0,0.50,100};
std::vector<Oscillator*> oscillatorParams = {&vco0,&vco1,&vco2,&vco3};
// LFO params
LFO lfo0Params = {&lowFreqOsc0,WAVEFORM_SINE,0,50,25};
LFO lfo1Params = {&lowFreqOsc1,WAVEFORM_SINE,0,50,25};
std::vector<LFO*> lfoParams = {&lfo0Params,&lfo1Params};

// Filter params
Filter filterParams = {&ladder1,1000.0, 0.5};
//std::vector<Filter*> fil
// Envelope params
Envelope envVCAParams = {&envelopeAmp,1, 1, 5, 1};
Envelope envVCFParams = {&envelopeFilter,1, 1, 5, 1};
Envelope envLFOParams = {&envelopeLFO,1, 1, 5, 1};
std::vector<Envelope*> envelopeParams = {&envVCAParams, &envVCFParams, &envLFOParams};
// Mixer params
Mixer mixer0Params = {&mixer1,0.25,0.25,0.25,0.25};
std::vector<Mixer*> mixerParams = {&mixer0Params};

// End setup audio --------------------------------------------------------------------------------


// ================================================================================================
// Setup Menu screens
// ================================================================================================

//TODO test dynamic menu creation and deletion
//TODO test home menu modes: when in perform mode, have hotkeys for each menuscreen active

struct MenuConfig {
    int mode;
    const char* name;    
    const unsigned char* bmp;
    std::vector<Icon> icons;
};

Icon sine = {sineBMP, "SINE",nullptr};
Icon sawtooth = {sawtoothBMP, "SAW",nullptr};
Icon square = {squareBMP, "SQR",nullptr};
Icon triangle = {triangleBMP, "TRI",nullptr};

Icon octv = {nullptr, "OCTV",nullptr};
Icon tune = {nullptr, "TUNE",nullptr};
Icon gain = {nullptr, "GAIN",nullptr};
Icon pulseWidth = {nullptr, "PW",nullptr};

MenusOLED menuRoot(MODE_DEFAULT,"ROOT",NULL);

MenusOLED menuHome(MODE_TEXT,"Home", homeBMP);
    /*
    MenusOLED homeConfig(MODE_TEXT,"Configure Mode", homeBMP);
    MenusOLED homePerform(MODE_TEXT,"Perform Mode", homeBMP);
    MenusOLED homeRecord(MODE_TEXT,"Record Mode", homeBMP);
    */
MenusOLED menuKeys(MODE_DEFAULT,"Keys", keysBMP);
MenusOLED menuOSC(MODE_TEXT,"Oscillators",oscBMP);
    MenusOLED osc0(MODE_ICON, "OSC 0", oscBMP);
    MenusOLED osc1(MODE_ICON, "OSC 1", oscBMP);
    MenusOLED osc2(MODE_ICON, "OSC 2", oscBMP);
    MenusOLED osc3(MODE_ICON, "OSC 3", oscBMP);
    MenusOLED lfo0(MODE_ICON, "LFO 0", oscBMP);
    MenusOLED lfo1(MODE_ICON, "LFO 1", oscBMP);

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
    MenusOLED envFLT(MODE_ICON,"ENV for FLT",envBMP);
    MenusOLED envVCA(MODE_ICON,"ENV for VCA",envBMP);
    MenusOLED envLFO(MODE_ICON,"ENV for LFO",envBMP);
  
MenusOLED menuEFF(MODE_TEXT,"Effects", keysBMP);
    MenusOLED effFade(MODE_DEFAULT,"Fade",keysBMP);
    MenusOLED effDelay(MODE_DEFAULT,"Delay",keysBMP);
    MenusOLED effReverb(MODE_DEFAULT,"Reverb",keysBMP);
    MenusOLED effChorus(MODE_DEFAULT,"Chorus",keysBMP);
    MenusOLED effFlanger(MODE_DEFAULT,"Flanger",keysBMP);
    MenusOLED effDist(MODE_DEFAULT,"Distortion",keysBMP);
    MenusOLED effCrush(MODE_DEFAULT,"Bit Crusher",keysBMP);
    MenusOLED effChop(MODE_DEFAULT,"Chop",keysBMP);
    MenusOLED effRing(MODE_DEFAULT,"Ring Mod",keysBMP);
    MenusOLED effWah(MODE_DEFAULT,"Wah",keysBMP);
    MenusOLED effPhase(MODE_DEFAULT,"Phaser",keysBMP);

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
    //menuHome.addChild(&homeConfig);
    //menuHome.addChild(&homePerform);
    //menuHome.addChild(&homeRecord);
}

void setupOscillatorMenu(){
    // Add children to main oscillator menu
    menuOSC.addChild(&osc0); menuOSC.addChild(&osc1); menuOSC.addChild(&osc2); menuOSC.addChild(&osc3);
    menuOSC.addChild(&lfo0); menuOSC.addChild(&lfo1);
    // Add icons as appropriate
    osc0.addIcon(0,sine); osc0.addIcon(0,sawtooth); osc0.addIcon(0,square); osc0.addIcon(0,triangle);
    osc1.addIcon(0,sine); osc1.addIcon(0,sawtooth); osc1.addIcon(0,square); osc1.addIcon(0,triangle);
    osc2.addIcon(0,sine); osc2.addIcon(0,sawtooth); osc2.addIcon(0,square); osc2.addIcon(0,triangle);
    osc3.addIcon(0,sine); osc3.addIcon(0,sawtooth); osc3.addIcon(0,square); osc3.addIcon(0,triangle);
    lfo0.addIcon(0,sine); lfo0.addIcon(0,sawtooth); lfo0.addIcon(0,square); lfo0.addIcon(0,triangle);
    lfo1.addIcon(0,sine); lfo1.addIcon(0,sawtooth); lfo1.addIcon(0,square); lfo1.addIcon(0,triangle);
    // Set slots
    /*vvoid setSlot(
            uint8_t index, 
            int8_t min, 
            int8_t max, 
            int8_t step, 
            //float scale, 
            int8_t* ptr, 
            //float initialValue, 
            const char* name=nullptr,
            std::function<float(float)> scalingFunction = [](float val){return val;},
            std::function<void(float)> onValueChange = [](float val){return val;}// Callback param
        );*/
    osc0.setSlot(0, 0, 3,1, &vco0.wave, "WAVE", 
        [](int8_t value) { return float(value); },
        [](float wave){waveform0.begin(int(wave));});        
    osc0.setSlot(1, 1, 5, 1, &vco0.octv, "OCTV", 
        [](float value) { return volt_to_freq(value + vco0.tune/120.0);},
        [](float scaledValue){waveform0.frequency(scaledValue);});        
    osc0.setSlot(2, -120, 120, 1, &vco0.tune, "TUNE", 
        [](float value) { return volt_to_freq(value/120.0 + vco0.octv);},
        [](float scaledValue){waveform0.frequency(scaledValue);});        
    osc0.setSlot(3, 0, 100, 5, &vco0.pulseWidth, "PWM", 
        [](float value) { return value/100; },
        [](float pulseWidth){waveform0.pulseWidth(pulseWidth);});

    osc1.setSlot(0, 0, 3,1, &vco1.wave, "WAVE", 
        [](int8_t value) { return float(value); },
        [](float wave){waveform1.begin(int(wave));});        
    osc1.setSlot(1, 1, 5, 1, &vco1.octv, "OCTV", 
        [](float value) { return volt_to_freq(value + vco1.tune/121.0);},
        [](float scaledValue){waveform1.frequency(scaledValue);});        
    osc1.setSlot(2, -120, 120, 1, &vco1.tune, "TUNE", 
        [](float value) { return volt_to_freq(value/121.0 + vco1.octv);},
        [](float scaledValue){waveform1.frequency(scaledValue);});        
    osc1.setSlot(3, 0, 100, 5, &vco1.pulseWidth, "PWM", 
        [](float value) { return value/100; },
        [](float pulseWidth){waveform1.pulseWidth(pulseWidth);});

    osc2.setSlot(0, 0, 3,1, &vco2.wave, "WAVE", 
        [](int8_t value) { return float(value); },
        [](float wave){waveform2.begin(int(wave));});        
    osc2.setSlot(1, 1, 5, 1, &vco2.octv, "OCTV", 
        [](float value) { return volt_to_freq(value + vco2.tune/122.0);},
        [](float scaledValue){waveform2.frequency(scaledValue);});        
    osc2.setSlot(2, -120, 120, 1, &vco2.tune, "TUNE", 
        [](float value) { return volt_to_freq(value/122.0 + vco2.octv);},
        [](float scaledValue){waveform2.frequency(scaledValue);});        
    osc2.setSlot(3, 0, 100, 5, &vco2.pulseWidth, "PWM", 
        [](float value) { return value/100; },
        [](float pulseWidth){waveform2.pulseWidth(pulseWidth);});

    osc3.setSlot(0, 0, 3,1, &vco3.wave, "WAVE", 
        [](int8_t value) { return float(value); },
        [](float wave){waveform3.begin(int(wave));});        
    osc3.setSlot(1, 1, 5, 1, &vco3.octv, "OCTV", 
        [](float value) { return volt_to_freq(value + vco3.tune/123.0);},
        [](float scaledValue){waveform3.frequency(scaledValue);});        
    osc3.setSlot(2, -120, 120, 1, &vco3.tune, "TUNE", 
        [](float value) { return volt_to_freq(value/123.0 + vco3.octv);},
        [](float scaledValue){waveform3.frequency(scaledValue);});        
    osc3.setSlot(3, 0, 100, 5, &vco3.pulseWidth, "PWM", 
        [](float value) { return value/100; },
        [](float pulseWidth){waveform3.pulseWidth(pulseWidth);});

    /*
    lfo0.setSlot(0, 0, 3, 1, &lfo0Params.wave,"WAVE", [](float value) { return value; },[](float wave){lowFreqOsc0.begin(int(wave));});
    lfo0.setSlot(1, -100, 100, 1, &lfo0Params.tune, "TUNE", &volt_to_freq,[](float scaledValue){lowFreqOsc0.frequency(scaledValue);}); 
    lfo0.setSlot(2, 0, 1, 0.05, &lfo0Params.pulseWidth, "PWM", [](float value) { return value; },[](float pulseWidth){lowFreqOsc0.pulseWidth(pulseWidth);});
    lfo0.setSlot(3, 0, 1, 0.01, &lfo0Params.gain, "GAIN");
    
    lfo1.setSlot(0, 0, 3,1, 1, &lfo1Params.wave,0 ,"WAVE");
    lfo1.setSlot(1, -100, 100, 1, 1, &lfo1Params.tune,0 , "TUNE");
    lfo1.setSlot(2, 0, 1, 5, 1, &lfo1Params.pulseWidth, 50, "PWM");
    lfo1.setSlot(3, 0, 1, 1, 1, &lfo1Params.gain, 25, "GAIN");
    */
}

void setupMixerMenu(){
    menuMIX.setSlot(0,0,100,5,&mixer0Params.chan0,"OSC0",
        [](int8_t value) { return float(value/100.0); },
        [](float gain){mixer1.gain(0,gain);});
    menuMIX.setSlot(1,0,100,5,&mixer0Params.chan1,"OSC1",
        [](int8_t value) { return float(value/100.0); },
        [](float gain){mixer1.gain(1,gain);});
    menuMIX.setSlot(2,0,100,5,&mixer0Params.chan2,"OSC2",
        [](int8_t value) { return float(value/100.0); },
        [](float gain){mixer1.gain(2,gain);});
    menuMIX.setSlot(3,0,100,5,&mixer0Params.chan3,"OSC3",
        [](int8_t value) { return float(value/100.0); },
        [](float gain){mixer1.gain(3,gain);});
}

void setupEnvelopeMenu(){
    menuENV.addChild(&envFLT);
    menuENV.addChild(&envVCA);
    menuENV.addChild(&envLFO);
    
    /*
    envFLT.setSlot(0, 0, 100, 1, 1, &envVCFParams.attack,0 ,"ATK"); 
    envFLT.setSlot(1, 0, 100, 1, 1, &envVCFParams.decay, 1, "DLY");
    envFLT.setSlot(2, 0, 100, 1, 1, &envVCFParams.sustain,0 , "SUS");
    envFLT.setSlot(3, 0, 100, 1, 1, &envVCFParams.release, 50, "REL");

    envVCA.setSlot(0, 0, 100, 1, 1, &envVCAParams.attack,0 ,"ATK"); 
    envVCA.setSlot(1, 0, 100, 1, 1, &envVCAParams.decay, 1, "DLY");
    envVCA.setSlot(2, 0, 100, 1, 1, &envVCAParams.sustain,0 , "SUS");
    envVCA.setSlot(3, 0, 100, 1, 1, &envVCAParams.release, 50, "REL");
    
    envLFO.setSlot(0, 0, 100, 1, 1, &envLFOParams.attack,0 ,"ATK"); 
    envLFO.setSlot(1, 0, 100, 1, 1, &envLFOParams.decay, 1, "DLY");
    envLFO.setSlot(2, 0, 100, 1, 1, &envLFOParams.sustain,0 , "SUS");
    envLFO.setSlot(3, 0, 100, 1, 1, &envLFOParams.release, 50, "REL");
    */
}

void setupEffectsMenu(){
    menuEFF.addChild(&effFade); 
    menuEFF.addChild(&effDelay);
    menuEFF.addChild(&effReverb);
    menuEFF.addChild(&effChorus);
    menuEFF.addChild(&effFlanger);
    menuEFF.addChild(&effDist);
    menuEFF.addChild(&effCrush);
    menuEFF.addChild(&effChop);
    menuEFF.addChild(&effRing);
    menuEFF.addChild(&effWah);
    menuEFF.addChild(&effPhase);
}

void screenSetup(){
    setupRootMenu();
    Serial.println("RootMenu complete!");
    setupHomeMenu();
    Serial.println("HomeMenu complete!");
    setupOscillatorMenu();
    Serial.println("OscMenu complete!");
    setupMixerMenu();
    Serial.println("MixMenu complete!");
    setupEnvelopeMenu();
    Serial.println("EnvMenu complete!");
    setupEffectsMenu();
    Serial.println("EffectMenu complete!");
}

void setupAudio(){
    AudioMemory(32);
    sgtl5000_1.enable();
    sgtl5000_1.volume(0.32);
    waveform0.begin(WAVEFORM_SINE); waveform0.amplitude(0.25); waveform0.frequency(220);
    waveform1.begin(WAVEFORM_SINE); waveform1.amplitude(0.25); waveform1.frequency(220);
    waveform2.begin(WAVEFORM_SINE); waveform2.amplitude(0.25); waveform2.frequency(220);
    waveform3.begin(WAVEFORM_SINE); waveform3.amplitude(0.25); waveform3.frequency(220);

    mixer1.gain(0,1);
    mixer1.gain(1,1);
    mixer1.gain(2,1);
    mixer1.gain(3,1);
}