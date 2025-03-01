#include <NotesVolts.h>
using namespace std;


/**
 * @brief Function to convert a voltage to a frequency based on 1V/Oct scaling
 * based off the set ROOT_FREQ value
 * 
 * @param volt the value representing the effective voltage of a signal
 * @return the corresponding frequency
 */
float volt_to_freq(float volt){       
    return ROOT_FREQ * pow(2,volt);
}
