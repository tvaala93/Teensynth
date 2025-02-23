#include <NotesVolts.h>
#include <map>
using namespace std;

float volt_to_freq(float volt){       
    return ROOT_FREQ * pow(2,volt);
}
