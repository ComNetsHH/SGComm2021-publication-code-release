#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "tinyxml2.h"
#include <string>
#include <iostream>


#ifndef KCONQUESTSETUPUITLS
#define KCONQUESTSETUPUITLS

using namespace ns3;
void setupNodeInfo (std::string topology, NodeContainer* nodeC , ApplicationContainer* leafNodes, ApplicationContainer* bbNodes, kConquestLeafHelper* leafHelper, kConquestBBHelper* bbHelper);


#endif // KCONQUESTSETUPUITLS