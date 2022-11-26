#ifndef NAHS_BRICKS_FEATURE_ALL_H
#define NAHS_BRICKS_FEATURE_ALL_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <nahs-Bricks-Feature-BaseClass.h>
#include <nahs-Bricks-Lib-RTCmem.h>
#include <nahs-Bricks-Lib-FSmem.h>

class NahsBricksFeatureAll : public NahsBricksFeatureBaseClass {
    private:  // Variables
        static const uint16_t version = 3;
        static const uint8_t MAX_FEATURES = 8;
        typedef struct {
            bool brickTypeRequested;
            bool versionsRequested;
            bool defaultDelayRequested;
            uint16_t delay;
        } _RTCdata;
        _RTCdata* RTCdata = RTCmem.registerData<_RTCdata>();
        JsonObject FSdata = FSmem.registerData("a");
        NahsBricksFeatureBaseClass* _registeredFeatures[MAX_FEATURES];
        uint8_t _registeredFeaturesCount = 0;
        uint16_t _osVersion;
        uint8_t _brickType;
    public:  // BaseClass implementations
        NahsBricksFeatureAll();
        String getName();
        uint16_t getVersion();
        void begin();
        void start();
        void deliver(JsonDocument* out_json);
        void feedback(JsonDocument* in_json);
        void end();
        void printRTCdata();
        void printFSdata();
        void brickSetupHandover();
    public:  // Brick-Specific setter
        void registerFeature(NahsBricksFeatureBaseClass* feature);
        void setOsVersion(uint16_t version);
        void setBrickType(uint8_t version);
    public:  // Feature getter
        uint16_t getDelay();
        uint8_t getBrickType();
    public:  // BrickSetup Helpers
        void printBrickSetupFeatureMenu(uint8_t offset);
        bool handoverBrickSetupToFeature(uint8_t feature_id);
        void printBrickSetupFeatureList();
        void printBrickSetupVersionList();
    private: // BrickSetup Helpers
        void _printMenu();
        void _setDefaultDelay();
        void _toggleAOD();
};

#if !defined(NO_GLOBAL_INSTANCES)
extern NahsBricksFeatureAll FeatureAll;
#endif

#endif // NAHS_BRICKS_FEATURE_ALL_H
