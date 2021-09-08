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