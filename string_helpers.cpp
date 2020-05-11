#include <string> // for string class

using namespace std;

// Function converts digital 0 or 1 to "OFF" and "ON"
string convertDigitalReadToString(int status) {
    if (status == 0) {
        return "OFF";
    } else {
        return "ON";
    }
}