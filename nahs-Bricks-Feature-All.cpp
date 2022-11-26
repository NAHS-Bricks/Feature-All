#include <nahs-Bricks-Feature-All.h>
#include <nahs-Bricks-Lib-SerHelp.h>

NahsBricksFeatureAll::NahsBricksFeatureAll() {
}

/*
Returns name of feature
*/
String NahsBricksFeatureAll::getName() {
    return "all";
}

/*
Returns version of feature
*/
uint16_t NahsBricksFeatureAll::getVersion() {
    return version;
}

/*
Configures FSmem und RTCmem variables (prepares feature to be fully operational)
*/
void NahsBricksFeatureAll::begin() {
    if (!FSdata.containsKey("delay")) FSdata["delay"] = 60;
    if (!FSdata.containsKey("aod")) FSdata["aod"] = false;  // allwaysOverwriteDelay(WithDefault)
    if (!RTCmem.isValid()) {
        RTCdata->versionsRequested = false;
        RTCdata->brickTypeRequested = false;
        RTCdata->defaultDelayRequested = false;
        RTCdata->delay = FSdata["delay"].as<uint16_t>();
    }
    yield();

    // and now begin all other features
    for (uint8_t i = 0; i < _registeredFeaturesCount; ++i) {
        _registeredFeatures[i]->begin();
        yield();
    }
}

/*
Starts background processes like fetching data from other components
*/
void NahsBricksFeatureAll::start() {
    yield();
    // and now start all other features
    for (uint8_t i = 0; i < _registeredFeaturesCount; ++i) {
        _registeredFeatures[i]->start();
        yield();
    }
}

/*
Adds data to outgoing json, that is send to BrickServer
*/
void NahsBricksFeatureAll::deliver(JsonDocument* out_json) {
    // check if brick has just start up
    if (!RTCmem.isValid()) {
        if (!out_json->containsKey("y")) out_json->createNestedArray("y");
        JsonArray y_array = out_json->getMember("y").as<JsonArray>();
        y_array.add("i");
    }

    // if allwaysOverrideDelay is set; inform BrickServer about this
    if (FSdata["aod"].as<bool>()) {
        if (!out_json->containsKey("y")) out_json->createNestedArray("y");
        JsonArray y_array = out_json->getMember("y").as<JsonArray>();
        y_array.add("d");
    }

    // check if list of versions is requested
    if (RTCdata->versionsRequested) {
        RTCdata->versionsRequested = false;
        JsonArray v_array = out_json->createNestedArray("v");
        JsonArray e_array = v_array.createNestedArray();
        e_array.add("os");
        e_array.add(_osVersion);
        e_array = v_array.createNestedArray();
        e_array.add(getName());
        e_array.add(getVersion());
        for (uint8_t i = 0; i < _registeredFeaturesCount; ++i) {
            e_array = v_array.createNestedArray();
            e_array.add(_registeredFeatures[i]->getName());
            e_array.add(_registeredFeatures[i]->getVersion());
        }
    }

    // check if brickType is requested
    if (RTCdata->brickTypeRequested) {
        RTCdata->brickTypeRequested = false;
        out_json->getOrAddMember("x").set(_brickType);
    }

    // check if defaultDelay is requested
    if (RTCdata->defaultDelayRequested) {
        RTCdata->defaultDelayRequested = false;
        out_json->getOrAddMember("d").set(FSdata["delay"].as<uint16_t>());
    }

    yield();

    // and now do deliver for all other features
    for (uint8_t i = 0; i < _registeredFeaturesCount; ++i) {
        _registeredFeatures[i]->deliver(out_json);
        yield();
    }
}

/*
Processes feedback coming from BrickServer
*/
void NahsBricksFeatureAll::feedback(JsonDocument* in_json) {
    // s in feedback is ignored for now

    // evaluate requests
    if (in_json->containsKey("r")) {
        for (JsonVariant value : in_json->getMember("r").as<JsonArray>()) {
            switch(value.as<uint8_t>()) {
                case 1:
                    RTCdata->versionsRequested = true;
                    break;
                case 5:
                    RTCdata->brickTypeRequested = true;
                    break;
                case 8:
                    RTCdata->defaultDelayRequested = true;
            }
        }
    }

    // check if new delay value is delivered
    if (in_json->containsKey("d")) {
        RTCdata->delay = in_json->getMember("d").as<uint16_t>();
    }

    yield();

    // and now handle feedback for all other features
    for (uint8_t i = 0; i < _registeredFeaturesCount; ++i) {
        _registeredFeatures[i]->feedback(in_json);
        yield();
    }
}

/*
Finalizes feature (closes stuff)
*/
void NahsBricksFeatureAll::end() {
    // and now end all other features
    for (uint8_t i = 0; i < _registeredFeaturesCount; ++i) {
        _registeredFeatures[i]->end();
        yield();
    }
}

/*
Prints the features RTCdata in a formatted way to Serial (used for brickSetup)
*/
void NahsBricksFeatureAll::printRTCdata() {
    Serial.print(getName());
    Serial.println(":");
    Serial.print("  versionsRequested: ");
    SerHelp.printlnBool(RTCdata->versionsRequested);
    Serial.print("  brickTypeRequested: ");
    SerHelp.printlnBool(RTCdata->brickTypeRequested);
    Serial.print("  defaultDelayRequested: ");
    SerHelp.printlnBool(RTCdata->defaultDelayRequested);
    Serial.print("  delay: ");
    Serial.println(RTCdata->delay);
    Serial.println();

    // and now print all other features RTCdata
    for (uint8_t i = 0; i < _registeredFeaturesCount; ++i) {
        Serial.print(_registeredFeatures[i]->getName());
        Serial.println(":");
        _registeredFeatures[i]->printRTCdata();
        Serial.println();
        yield();
    }
}

/*
Prints the features FSdata in a formatted way to Serial (used for BrickSetup)
*/
void NahsBricksFeatureAll::printFSdata() {
    Serial.print(getName());
    Serial.println(":");
    Serial.print("  defaultDelay: ");
    Serial.println(FSdata["delay"].as<uint16_t>());
    Serial.print("  allwaysOverwriteDelay: ");
    SerHelp.printlnBool(FSdata["aod"].as<bool>());
    Serial.println();

    // and now print all other features FSdata
    for (uint8_t i = 0; i < _registeredFeaturesCount; ++i) {
        Serial.print(_registeredFeatures[i]->getName());
        Serial.println(":");
        _registeredFeatures[i]->printFSdata();
        Serial.println();
        yield();
    }
}

/*
BrickSetup hands over to this function, when features-submenu is selected
*/
void NahsBricksFeatureAll::brickSetupHandover() {
    _printMenu();
    while (true) {
        Serial.println();
        Serial.print("Select: ");
        uint8_t input = SerHelp.readLine().toInt();
        switch(input) {
            case 1:
                _setDefaultDelay();
                break;
            case 2:
                _toggleAOD();
                break;
            case 9:
                Serial.println("Returning to MainMenu!");
                return;
                break;
            default:
                Serial.println("Invalid input!");
                _printMenu();
                break;
        }
    }
}

/*
used to register features to be used on Brick
this function should normally be called on class-constructor of the feature
*/
void NahsBricksFeatureAll::registerFeature(NahsBricksFeatureBaseClass* feature) {
    if (_registeredFeaturesCount < MAX_FEATURES) {
        bool allreadyRegistered = false;

        for (uint8_t i = 0; i < _registeredFeaturesCount; ++i) {
            if (_registeredFeatures[i]->getName() == feature->getName()) {
                allreadyRegistered = true;
                break;
            }
        }

        if (!allreadyRegistered) {
            _registeredFeatures[_registeredFeaturesCount] = feature;
            _registeredFeaturesCount++;
        }
    }
}

/*
sets osVersion
*/
void NahsBricksFeatureAll::setOsVersion(uint16_t version) {
    _osVersion = version;
}

/*
sets brickType
*/
void NahsBricksFeatureAll::setBrickType(uint8_t version) {
    _brickType = version;
}

/*
return delay
*/
uint16_t NahsBricksFeatureAll::getDelay() {
    if (FSdata["aod"].as<bool>()) return FSdata["delay"].as<uint16_t>();
    else return RTCdata->delay;
}

/*
return brickType
*/
uint8_t NahsBricksFeatureAll::getBrickType() {
    return _brickType;
}

/*
helper fpr BrickSetup to print all FeatuerSubmenu entries of mainmenu
*/
void NahsBricksFeatureAll::printBrickSetupFeatureMenu(uint8_t offset) {
    Serial.print(offset);
    Serial.print(") ");
    Serial.println(getName());
    offset++;
    for (uint8_t i = 0; i < _registeredFeaturesCount; ++i) {
        Serial.print(offset + i);
        Serial.print(") ");
        Serial.println(_registeredFeatures[i]->getName());
    }
}

/*
helper for BrickServer to hand over the terminal/menu to a feature
Returns true if feature_id is valid, else returns false
*/
bool NahsBricksFeatureAll::handoverBrickSetupToFeature(uint8_t feature_id) {
    if (feature_id == 0) {
        brickSetupHandover();
        return true;
    }
    else if (feature_id > _registeredFeaturesCount) {
        return false;
    }
    else {
        _registeredFeatures[feature_id - 1]->brickSetupHandover();
        return true;
    }
}

/*
helper for BrickSetup to print all registered features as a list
*/
void NahsBricksFeatureAll::printBrickSetupFeatureList() {
    for (uint8_t i = 0; i < _registeredFeaturesCount; ++i) {
        Serial.print("  ");
        Serial.println(_registeredFeatures[i]->getName());
    }
}

/*
helper for BrickSetup to print all features with it's versions as a list
*/
void NahsBricksFeatureAll::printBrickSetupVersionList() {
    Serial.print("  os: ");
    Serial.println(_osVersion);
    Serial.print("  ");
    Serial.print(getName());
    Serial.print(": ");
    Serial.println(getVersion());

    for (uint8_t i = 0; i < _registeredFeaturesCount; ++i) {
        Serial.print("  ");
        Serial.print(_registeredFeatures[i]->getName());
        Serial.print(": ");
        Serial.println(_registeredFeatures[i]->getVersion());
    }
}

/*
Helper to print Feature submenu during BrickSetup
*/
void NahsBricksFeatureAll::_printMenu() {
    Serial.println("1) Set defaultDelay");
    Serial.println("2) Toggle allwaysOverwriteDelay");
    Serial.println("9) Return to MainMenu");
}

/*
BrickSetup function to set defaultDelay
*/
void NahsBricksFeatureAll::_setDefaultDelay() {
    Serial.print("Enter Seconds: ");
    uint16_t input = SerHelp.readLine().toInt();
    if (input < 1 || input > 1800) {
        Serial.println("Invalid input!");
        return;
    }
    FSdata["delay"] = input;
    Serial.print("defalutDelay set to: ");
    Serial.println(FSdata["delay"].as<uint16_t>());
}

void NahsBricksFeatureAll::_toggleAOD() {
    FSdata["aod"] = !FSdata["aod"].as<bool>();
    Serial.print("allwaysOverwriteDelay set to: ");
    SerHelp.printlnBool(FSdata["aod"].as<bool>());
}

//------------------------------------------
// globally predefined variable
#if !defined(NO_GLOBAL_INSTANCES)
NahsBricksFeatureAll FeatureAll;
#endif