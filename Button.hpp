#include "Timer.hpp"

class Button
{
    public:

    Button(const int ioPin, std::function<void()> func)
    {
        pin = ioPin;
        callback = func;
    }

    void initialize()
    {
        pinMode(pin, INPUT);
    }

    void tick()
    {
        int currentState = digitalRead(pin);

        if (currentState != previousState)
            debounceTimer.setGoalTime(millis() + debounceDelay);

        if (debounceTimer.isGoalTimeMet()) {
            if (stablePinState != currentState) {
                stablePinState = currentState;

                if (stablePinState == HIGH)
                    callback();
            }
        }

        previousState = currentState;
    }

    bool isPressed()
    {
        return stablePinState == HIGH;
    }

    private:
    int pin;
    int previousState = LOW;
    Timer debounceTimer;
    const unsigned long debounceDelay = 50;
    bool stablePinState;
    std::function<void()> callback;
};
