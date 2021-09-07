/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c)  2021 TUHH
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
 *
 * Author: Leonard Fisser <leonard.fisser@tuhh.de>
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