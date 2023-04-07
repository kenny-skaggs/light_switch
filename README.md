# light_switch
## Purpose
My partner and I have a few Kasa smart bulbs (https://www.kasasmart.com/us/products/smart-lighting/kasa-smart-light-bulb-multicolor-kl125) and we wanted a way to turn them on and off outside of using the smartphone app.

I also have an ESP8266 that doesn't have a life purpose yet. So this is my endeavor to make physical button for the lights by starting to really learn: C++, the ESP8266/Arduino SDK, TP-Link's API, and a bit of wiring.

## State of the Project
I'm just now getting the TP-Link API figured out. Next step is to clean up the code and make some re-usable components before I do much more with the API.

## System Details
### Talking to the bulbs (TP-Link API)
This took some reverse engineering of the python-kasa code (https://github.com/python-kasa/python-kasa). Communication with the bulb is encrypted, but it's a simple XOR cipher so it's easy enough to set up (after figuring it out from the python code). And the python code also gives the command text to be able to get the state of the bulbs, as well as turn them on/off.
