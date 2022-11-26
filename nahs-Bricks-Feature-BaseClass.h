#ifndef NAHS_BRICKS_FEATURE_BASECLASS_H
#define NAHS_BRICKS_FEATURE_BASECLASS_H

#include <Arduino.h>
#include <ArduinoJson.h>

class NahsBricksFeatureBaseClass {
    protected:
        NahsBricksFeatureBaseClass() {};
    public:
        /*
        Returns name of feature
        */
        virtual String getName() = 0;

        /*
        Returns version of feature
        */
        virtual uint16_t getVersion() = 0;

        /*
        Configures FSmem und RTCmem variables (prepares feature to be fully operational)
        */
        virtual void begin() = 0;

        /*
        Starts background processes like fetching data from other components
        */
        virtual void start() = 0;

        /*
        Adds data to outgoing json, that is send to BrickServer
        */
        virtual void deliver(JsonDocument* out_json) = 0;

        /*
        Processes feedback coming from BrickServer
        */
        virtual void feedback(JsonDocument* in_json) = 0;

        /*
        Finalizes feature (closes stuff)
        */
        virtual void end() = 0;

        /*
        Prints the features RTCdata in a formatted way to Serial (used for brickSetup)
        */
        virtual void printRTCdata() = 0;

        /*
        Prints the features FSdata in a formatted way to Serial (used for brickSetup)
        */
        virtual void printFSdata() = 0;

        /*
        BrickSetup hands over to this function, when features-submenu is selected
        */
        virtual void brickSetupHandover() = 0;
};

#endif // NAHS_BRICKS_FEATURE_BASECLASS_H
