#include <Arduino.h>
#include <nahs-Bricks-Feature-All.h>
#include <nahs-Bricks-Lib-FSmem.h>

/*
This example shows how the BrickSetup could interact with this Feature.
First some lists are printed, with data generated by this Feature.
Afterwards the BrickSetup is handed over to specific Features, referenced by their IDs
*/

void setup() {
  Serial.begin(115200);

  // initializing the feature
  FeatureAll.setOsVersion(77);
  FeatureAll.begin();
  FeatureAll.start();

  // printing the FeatureList from Feature-All's perspective
  // This should print nothing, as no other Features are registered
  Serial.println("\n\nFeatureList:");
  FeatureAll.printBrickSetupFeatureList();

  // printing the list of all versions, this should only contain OS and All
  Serial.println("\nVersionList:");
  FeatureAll.printBrickSetupVersionList();

  // showing off FSdata before the BrickSetup handovers happened
  // in which you are supposed to change some variables
  Serial.println("\nFSdata (before BrickSetup):");
  FeatureAll.printFSdata();

  // handing over the BrickSetup to Feature-All
  // feel free to change some variables
  Serial.println("BrickSetup of FeatureId 0:");
  FeatureAll.handoverBrickSetupToFeature(0);

  // handing over to an imaginary second Feature which, in this case, is not existing
  // nothing should happen, as no other Feature than All is registered
  Serial.println("\nBrickSetup of FeatureId 1:");
  FeatureAll.handoverBrickSetupToFeature(1);

  // finally show FSdata off again, which should now be modified by you
  Serial.println("\nFSdata (after BrickSetup):");
  FeatureAll.printFSdata();
}

void loop() {
}