#include "ns3/lte-module.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/config-store.h"
#include "kConquestSetupUtils.h"
#include "tinyxml2.h"
#include <cfloat>
#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <ns3/wifi-module.h>

using namespace ns3;

void
traceNodeEvents(std::string event, std::string test){
  std::ofstream os;
  os.open ("eventTrace.txt",std::fstream::app);
  os << test << std::endl;
  os.close();
}

void
traceRtxEvents(std::string event, std::string test){
  std::ofstream os;
  os.open ("rtxEventTrace.txt",std::fstream::app);
  os << test << std::endl;
  os.close();
}

int main (int argc, char *argv[])
{
//-------------------//
// Parameter Init
//-------------------//

  // Input Parameter Default Values
  // Global Simulation Parameter
  Time simTime = Seconds (6); //!< Simulation Durtation
  std::string topology = ""; //!< Topology
  double rtxTimeout = 60; //!< Retransmission timeout value in ms
  double useDynamicRtxTimeout = false;
  bool useLte = false; //!< Run LTE simulations
  bool useWifi = false; //!< Run Wifi simulations
  double dectPowerLevel = -80; //!< Radio activity detection level (dBm)
  double dectRange = 300; //!< Radio activity detection range (m)

  // Clear all previous results if exist, add all node info to output file at later point
  std::ofstream osTrace;
  osTrace.open ("eventTrace.txt");
  std::ofstream osRtxTrace;
  osRtxTrace.open ("rtxEventTrace.txt");
  osRtxTrace << "Time:Node:Timeout:Input_RTT" << std::endl;

  // LTE Parameter
  uint32_t ulEarfcn = 18100; //!< Uplink Frequency used in SL communication
  uint16_t ulBandwidth = 100; //!< Number of Ressource Blocks for SL communication

  // Wifi Parameter
  //std::string phyMode ("DsssRate2Mbps"); //!< Physial Layer Bitrate
  std::string phyMode ("DsssRate11Mbps"); //!< Physial Layer Bitrate

//-------------------//
// Parameter Setup
//-------------------//

  CommandLine cmd;
  cmd.AddValue ("simTime", "Total duration of the simulation", simTime);
  cmd.AddValue ("Topology", "Select the network topology from precomputed models.", topology);
  cmd.AddValue ("rtxTimeout", "Retransmission timeout value used for acknowledged transmissin.", rtxTimeout);
  cmd.AddValue ("dynamicRTX", "The retransmission timeout value is extracted from node config files.", useDynamicRtxTimeout);
  cmd.AddValue ("useLTE", "Link layer technology.", useLte);
  cmd.Parse (argc, argv);

  ConfigStore inputConfig;
  inputConfig.ConfigureDefaults ();
  // parse again so we can override input file default values via command line
  cmd.Parse (argc, argv);

  useWifi = !useLte;

//-------------------//
// Secondary Link Layer Setup
//-------------------//

  // These helpers have to be created for any case, as they have to be created before the node containers
  //Create the helpers
  Ptr<LteHelper> lteHelper;
  // Create and set the EPC helper
  Ptr<PointToPointEpcHelper> epcHelper;
  // Create Sidelink helper and set lteHelper
  Ptr<LteSidelinkHelper> proseHelper;
  // Sidelink bearers activation time
  Time slBearersActivationTime;

  // LL Parameter
  if(useLte){
    // Configure the UE for UE_SELECTED scenario
    Config::SetDefault ("ns3::LteUeMac::SlGrantMcs", UintegerValue (16));
    Config::SetDefault ("ns3::LteUeMac::SlGrantSize", UintegerValue (6)); //The number of RBs allocated per UE for Sidelink
    Config::SetDefault ("ns3::LteUeMac::Ktrp", UintegerValue (1));
    Config::SetDefault ("ns3::LteUeMac::UseSetTrp", BooleanValue (false)); //use default Trp index of 0
    Config::SetDefault ("ns3::LteUePhy::TxPower", DoubleValue (23.0));
    // Set error models
    Config::SetDefault ("ns3::LteSpectrumPhy::SlCtrlErrorModelEnabled", BooleanValue (true));
    Config::SetDefault ("ns3::LteSpectrumPhy::SlDataErrorModelEnabled", BooleanValue (true));
    Config::SetDefault ("ns3::LteSpectrumPhy::DropRbOnCollisionEnabled", BooleanValue (true));

    // These helpers have to be created for any case, as they have to be created before the node containers
    //Create the helpers
    lteHelper = CreateObject<LteHelper> ();
    // Create and set the EPC helper
    epcHelper = CreateObject<PointToPointEpcHelper> ();
    lteHelper->SetEpcHelper (epcHelper);
    // Create Sidelink helper and set lteHelper
    proseHelper = CreateObject<LteSidelinkHelper> ();
    proseHelper->SetLteHelper (lteHelper);
    // Sidelink bearers activation time
    slBearersActivationTime = Seconds (1.0);

    //Enable Sidelink
    lteHelper->SetAttribute ("UseSidelink", BooleanValue (true));
    // Set pathloss model
    lteHelper->SetAttribute ("PathlossModel", StringValue ("ns3::TwoDiskPropagationLossModel"));
    // channel model initialization
    lteHelper->Initialize ();
  }
  if(useWifi){
    Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue ("2200")); // disable fragmentation for frames below 2200 bytes
    Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("2200")); // turn off RTS/CTS for frames below 2200 bytes
    Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue (phyMode)); // Fix non-unicast data rate to be the same as that of unicast
  }  

  // Load communication range from first xml file 
  tinyxml2::XMLDocument doc;
  std::string path = topology;
  doc.LoadFile( std::string(path + "/Node-1" + ".xml").c_str() );
  double communicationRange = std::stod(doc.FirstChildElement( "Node-Config" )->FirstChildElement( "comRange" )->GetText());

//-------------------//
// Node Setup
//-------------------//
  NodeContainer nodeC;
  ApplicationContainer leafApps, bbApps;
  kConquestBBHelper bbHelper = kConquestBBHelper();
  kConquestLeafHelper leafHelper = kConquestLeafHelper();
  setupNodeInfo(path,&nodeC,&leafApps,&bbApps,&leafHelper,&bbHelper);
  // Print info debug
  for(uint16_t leafNodeID = 0; leafNodeID < leafApps.GetN(); leafNodeID++){
    osTrace << leafApps.Get(leafNodeID)->GetObject<kConquestLeaf>()->printNodeInfo();
  }
  for(uint16_t bbNodeID = 0; bbNodeID < bbApps.GetN(); bbNodeID++){
    osTrace << bbApps.Get(bbNodeID)->GetObject<kConquestBB>()->printNodeInfo();
  }
  osTrace << "Positions Start" << std::endl;
  osTrace << std::to_string(communicationRange) << std::endl;
  for(uint16_t bbNodeID = 0; bbNodeID < bbApps.GetN(); bbNodeID++){
    osTrace << bbApps.Get(bbNodeID)->GetNode()->GetId() << ",";
  }
  osTrace << std::endl;
  for(uint16_t nodeID = 0; nodeID < nodeC.GetN(); nodeID++){
    osTrace << "NodeID: " << std::to_string(nodeID+1) << "\tX Position:" << nodeC.Get(nodeID)->GetObject<ConstantPositionMobilityModel>()->GetPosition().x << "\tY Position:" << nodeC.Get(nodeID)->GetObject<ConstantPositionMobilityModel>()->GetPosition().y << std::endl;
  }
  osTrace << "Positions End" << std::endl;

  // Output Event Trace Headers
  osTrace << "Time/Node\tTX/RX\tType\tInfo\tDest/Src" << std::endl;


//-------------------//
// Main Link Layer Setup
//-------------------//

  Ipv4Address multiCastAddress; 

  if(useLte){
  //-------------------//
  // Sidelink Setup
  //-------------------//
    // Since we are not installing eNB, we need to set the frequency attribute of pathloss model here
    double ulFreq = LteSpectrumValueHelper::GetCarrierFrequency (ulEarfcn);
    Ptr<Object> uplinkPathlossModel = lteHelper->GetUplinkPathlossModel ();
    Ptr<PropagationLossModel> lossModel = uplinkPathlossModel->GetObject<TwoDiskPropagationLossModel> ();
    lossModel->SetAttributeFailSafe ("Frequency", DoubleValue (ulFreq));
    lossModel->SetAttribute ("MaxComRange", DoubleValue (communicationRange));
    lossModel->SetAttribute ("MaxDectRange", DoubleValue (dectRange));
    lossModel->SetAttribute ("DectPower", DoubleValue (-10000));

    // Install LTE UE devices to the nodes
    NetDeviceContainer ueDevs = lteHelper->InstallUeDevice (nodeC);

    // Sidelink pre-configuration for the UEs
    Ptr<LteSlUeRrc> ueSidelinkConfiguration = CreateObject<LteSlUeRrc> ();
    ueSidelinkConfiguration->SetSlEnabled (true);
    LteRrcSap::SlPreconfiguration preconfiguration;
    preconfiguration.preconfigGeneral.carrierFreq = ulEarfcn;
    preconfiguration.preconfigGeneral.slBandwidth = ulBandwidth;

    LteSlPreconfigPoolFactory pfactory;
    //Control
    pfactory.SetControlPeriod ("sf40");
    pfactory.SetControlBitmap (0x00000000FF); //8 subframes for PSCCH
    pfactory.SetControlOffset (0);
    pfactory.SetControlPrbNum (100);
    pfactory.SetControlPrbStart (0);
    pfactory.SetControlPrbEnd (99);

    //Data
    pfactory.SetDataBitmap (0xFFFFFFFFFF);
    pfactory.SetDataOffset (8); //After 8 subframes of PSCCH
    pfactory.SetDataPrbNum (100);
    pfactory.SetDataPrbStart (0);
    pfactory.SetDataPrbEnd (99);

    preconfiguration.preconfigComm.pools[0] = pfactory.CreatePool ();
    preconfiguration.preconfigComm.nbPools = 1;
    ueSidelinkConfiguration->SetSlPreconfiguration (preconfiguration);
    lteHelper->InstallSidelinkConfiguration (ueDevs, ueSidelinkConfiguration);

    //-------------------//
    // LTE Routing Setup
    //-------------------//

    InternetStackHelper internet;
    internet.Install (nodeC);
    uint32_t groupL2Address = 255;
    Ipv4Address groupAddress4 ("225.0.0.0");     //use multicast address as destination
    Ptr<LteSlTft> tft;

    Ipv4InterfaceContainer ueIpIface;
    ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueDevs));

    // set the default gateway for the UEs
    Ipv4StaticRoutingHelper ipv4RoutingHelper;

    for (uint32_t u = 0; u < nodeC.GetN (); ++u)
      {
        Ptr<Node> ueNode = nodeC.Get (u);
        // Set the default gateway for the UE
        Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
        ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
      }

    tft = Create<LteSlTft> (LteSlTft::BIDIRECTIONAL, groupAddress4, groupL2Address);
    proseHelper->ActivateSidelinkBearer (slBearersActivationTime, ueDevs, tft);
    multiCastAddress = groupAddress4;
    //lteHelper->EnableSidelinkTraces (); Enabling logging greatly increase simulation duration, as disk speed becomes a factor.
  }

  if(useWifi){
  //-------------------//
  // WiFi Setup
  //-------------------//
    // The below set of helpers will help us to put together the wifi NICs
    WifiHelper wifi;
    wifi.SetStandard (WIFI_PHY_STANDARD_80211b); // OFDM at 2.4 GHz
    YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
    
    YansWifiChannelHelper wifiChannel;
    //wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel","Speed",DoubleValue(50));
    wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
    wifiChannel.AddPropagationLoss ("ns3::TwoDiskPropagationLossModel","MaxComRange", DoubleValue (communicationRange),
                                                                     "MaxDectRange", DoubleValue (dectRange),
                                                                     "DectPower", DoubleValue (dectPowerLevel));

    wifiPhy.SetChannel (wifiChannel.Create ());

    // Disable rate control
    wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager","DataMode", StringValue (phyMode), "ControlMode", StringValue (phyMode));

    // Set WiFi Mac type to adhoc mode
    WifiMacHelper wifiMac;
    wifiMac.SetType ("ns3::AdhocWifiMac");

    // Add Wifi to nodes
    NetDeviceContainer devices = wifi.Install (wifiPhy, wifiMac, nodeC);

    //-------------------//
    // Routing Setup
    //-------------------//
    InternetStackHelper internet;
    internet.Install (nodeC);

    Ipv4AddressHelper ipv4;
    ipv4.SetBase ("10.1.1.0", "255.255.255.0");
    ipv4.Assign (devices);
    Ipv4Address dest = Ipv4Address ("255.255.255.255");
    multiCastAddress = dest;
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  }

//-------------------//
// Application Setup
//-------------------//

  for (uint32_t u = 0; u < nodeC.GetN (); ++u){
    Ptr<kConquestClient> application = nodeC.Get(u)->GetApplication(0)->GetObject<kConquestClient>();
    application->SetAttribute("StartTime",TimeValue(Seconds(2)));
    application->SetAttribute("RemoteAddress",AddressValue(multiCastAddress));
    application->SetAttribute("RemotePort",UintegerValue(8000));
    application->SetAttribute("PacketSize",UintegerValue(80));
    application->SetAttribute("TransmissionAttemptInterval",TimeValue(MilliSeconds(rtxTimeout)));

    if(useDynamicRtxTimeout){
      application->SetAttribute("enableDynamicRTXTimer",BooleanValue(true));
    }
  }

//-------------------//
// Statistics Setup
//-------------------//
  
  Config::Connect ("/NodeList/*/ApplicationList/*/$ns3::kConquestClient/EventTrace",MakeCallback (&traceNodeEvents));
  Config::Connect ("/NodeList/*/ApplicationList/*/$ns3::kConquestBB/RtxTimerTrace",MakeCallback (&traceRtxEvents));


//-------------------//
// Execute Simulation
//-------------------//

  Simulator::Stop (simTime);
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
