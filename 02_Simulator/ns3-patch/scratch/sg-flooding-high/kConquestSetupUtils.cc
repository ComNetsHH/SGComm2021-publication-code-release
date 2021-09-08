/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* 
 * Code to replicate results of the publication 
 * 'Minimizing Age of Information for Distributed Control in Smart Grids'
 * published at the IEEE International Conference on Communications, Control, and Computing Technologies for Smart Grids 2021, Aachen, Germany.
 * https://github.com/ComNetsHH/SGComm2021-publication-code-release
 * 
 * 
 * Copyright (C) 2021  Institute of Communication Networks, 
 *                     Hamburg University of Technology,
 *                     https://www.tuhh.de/comnets
 *           (C) 2021  Leonard Fisser, 
 *                     leonard.fisser@tuhh.de
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/lte-module.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "kConquestSetupUtils.h"
#include "tinyxml2.h"
#include <string>
#include <iostream>

using namespace ns3;
void setupNodeInfo (std::string path, NodeContainer* nodeC , ApplicationContainer* leafNodes, ApplicationContainer* bbNodes, kConquestLeafHelper* leafHelper, kConquestBBHelper* bbHelper)
{
  // Load node info files into c++ classes    
  tinyxml2::XMLDocument doc;
  uint8_t nodeID = 1;
  uint8_t numNodes = 0;
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

  // We can savely load the first file, as each topology has to have at least one node, afterwards continue until maxID from first node
  do{
    doc.LoadFile( std::string(path + "/Node-" + std::to_string(nodeID) + ".xml").c_str() );
    
    uint8_t nodeType = std::stoi(doc.FirstChildElement( "Node-Config" )->FirstChildElement( "type" )->GetText());
    uint16_t maxID = std::stoi(doc.FirstChildElement( "Node-Config" )->FirstChildElement( "maxID" )->GetText());
    nodeC->Create(1);
    Ptr<ListPositionAllocator> position = CreateObject<ListPositionAllocator> ();
    position->Add(Vector(std::stod(doc.FirstChildElement( "Node-Config" )->FirstChildElement( "xPos" )->GetText()), std::stod(doc.FirstChildElement( "Node-Config" )->FirstChildElement( "yPos" )->GetText()), 1.5));
    mobility.SetPositionAllocator(position);
    mobility.Install(nodeC->Get(nodeC->GetN()-1));


    if (nodeType == 1) // Leaf Node
    {
      Ptr<ListPositionAllocator> position = CreateObject<ListPositionAllocator> ();
      position->Add(Vector(std::stod(doc.FirstChildElement( "Node-Config" )->FirstChildElement( "xPos" )->GetText()), std::stod(doc.FirstChildElement( "Node-Config" )->FirstChildElement( "yPos" )->GetText()), 1.5));
      mobility.SetPositionAllocator(position);
      leafHelper->SetAttribute("ID",UintegerValue(std::stoi(doc.FirstChildElement( "Node-Config" )->FirstChildElement( "ID" )->GetText())));
      leafHelper->SetAttribute("MaxID", UintegerValue(maxID));
      leafHelper->SetAttribute("Type",EnumValue(kConquestClient::KCONQUEST_LEAF));
      leafHelper->SetAttribute("FirstInSeq",UintegerValue(std::stoi(doc.FirstChildElement( "Node-Config" )->FirstChildElement( "firstInSeq" )->GetText())));
      leafHelper->SetAttribute("NextSeqID",UintegerValue(std::stoi(doc.FirstChildElement( "Node-Config" )->FirstChildElement( "nextInSeq" )->GetText())));
      leafHelper->SetAttribute("ForwardUp",UintegerValue(std::stoi(doc.FirstChildElement( "Node-Config" )->FirstChildElement( "forwardUp" )->GetText())));
      leafNodes->Add(leafHelper->Install(nodeC->Get(nodeC->GetN()-1)));
    }
    else if (nodeType == 2) // BB Node
    {
      bbHelper->SetAttribute("ID",UintegerValue(std::stoi(doc.FirstChildElement( "Node-Config" )->FirstChildElement( "ID" )->GetText())));
      bbHelper->SetAttribute("MaxID", UintegerValue(maxID));
      bbHelper->SetAttribute("Type",EnumValue(kConquestClient::KCONQUEST_BB));
      bbHelper->SetAttribute("FirstInSeq",UintegerValue(std::stoi(doc.FirstChildElement( "Node-Config" )->FirstChildElement( "firstInSeq" )->GetText())));
      bbHelper->SetAttribute("NextSeqID",UintegerValue(std::stoi(doc.FirstChildElement( "Node-Config" )->FirstChildElement( "nextInSeq" )->GetText())));
      bbHelper->SetAttribute("ForwardUp",UintegerValue(std::stoi(doc.FirstChildElement( "Node-Config" )->FirstChildElement( "forwardUp" )->GetText())));
      bbHelper->SetAttribute("ForwardDown",StringValue(std::string(doc.FirstChildElement( "Node-Config" )->FirstChildElement( "forwardDown" )->GetText())));
      bbHelper->SetAttribute("BackBoneNeighbours",StringValue(std::string(doc.FirstChildElement( "Node-Config" )->FirstChildElement( "backBoneNeighbours" )->GetText())));
      bbHelper->SetAttribute("BackBoneHopDest",StringValue(std::string(doc.FirstChildElement( "Node-Config" )->FirstChildElement( "backBoneHopDest" )->GetText())));
      bbHelper->SetAttribute("BackBoneHop",StringValue(std::string(doc.FirstChildElement( "Node-Config" )->FirstChildElement( "backBoneHop" )->GetText())));
      bbNodes->Add(bbHelper->Install(nodeC->Get(nodeC->GetN()-1)));
    }
    else // Error
    {
      std::cerr << "ERROR: Unknow Node Type";
    }

    if(numNodes == 0){
      numNodes = maxID;
    }
    nodeID++;
  }while(nodeID<=numNodes);

}