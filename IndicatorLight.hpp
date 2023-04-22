
class IndicatorLight
{
    public:

    static const int OFF_STATE = 0;
    static const int LONG_STATE = 1;
    static const int SHORT_STATE = 2;

    IndicatorLight(const int ioPin)
    {
        pin = ioPin;
    }

    void initialize()
    {
        pinMode(pin, OUTPUT);
        setLightOn(false);
    }

    void tick()
    {
        if (state != OFF_STATE) {
            unsigned long timeSinceChange = millis() - lastLightChange;
            if (isLightOn && timeSinceChange > 10) {
                setLightOn(false);
            } else {
                int lightOffTime = 1000;
                if (state == SHORT_STATE)
                    lightOffTime = 300;

                if (timeSinceChange > lightOffTime)
                    setLightOn(true);
            }
        }
        if (state == 1 && (millis() - lastLightChange) > 1000) {
            lastLightChange = millis();
            setLightOn(!isLightOn);
        }
    }

    void setState(int newState)
    {
        if (state != newState) {
            state = newState;
            setLightOn(state != OFF_STATE);
        }
    }

    void deactivate()
    {
        state = 0;
        setLightOn(false);
    }

    private:
    int pin;
    int state {0};
    unsigned long lastLightChange{0};
    bool isLightOn{false};

    void setLightOn(bool on)
    {
        int valToWrite = HIGH;
        if (on)
            valToWrite = LOW;

        digitalWrite(pin, valToWrite);
        isLightOn = on;

        lastLightChange = millis();
    }
};
