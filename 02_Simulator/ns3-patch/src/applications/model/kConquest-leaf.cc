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

#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/udp-socket.h"
#include "ns3/address-utils.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/core-module.h"
#include "kConquest-leaf.h"
#include "seq-ts-header.h"
#include "kConquest-header.h"


namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("kConquestLeaf");

NS_OBJECT_ENSURE_REGISTERED (kConquestLeaf);

TypeId
kConquestLeaf::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::kConquestLeaf")
    .SetParent<kConquestClient> ()
    .SetGroupName("Applications")
    .AddConstructor<kConquestLeaf> ()
  ;
  return tid;
}

kConquestLeaf::kConquestLeaf ()
{
  kConquestClient();
}

kConquestLeaf::~kConquestLeaf ()
{
}

void
kConquestLeaf::DoDispose (void)
{
  Application::DoDispose ();
}

void
kConquestLeaf::StartApplication (void)
{

  kConquestClient::StartApplication();
  m_socket->SetRecvCallback (MakeCallback (&kConquestLeaf::HandleRead, this));
  Simulator::Schedule (m_interval, &kConquestClient::HandleCycle, this);

  // If we are the first node in our sequence, start data generation
  if(m_ID==m_firstInSeq)
  {
    GenerateNewData();
  }
}

void 
kConquestLeaf::HandleRead (Ptr<Socket> socket)
{
  Ptr<Packet> packet;
  Address from;
  Address localAddress;
  while ((packet = socket->RecvFrom (from)))
    {
      if (packet->GetSize () == 0)
        { //EOF
          break;
        }
      kConquestHeader kCHeader;
      packet->RemoveHeader(kCHeader);
      // Check if I am addressed
      bool inDestField = kCHeader.inDestIDs(m_ID);
      // Received data ACK, we have to update our outstanding packet list
      if(kCHeader.getPktType() == kConquestHeader::DATA_ACK && inDestField){
        HandleDataAck(packet,kCHeader);
      }
      // Received data, we have to answer with an ACK
      else if(kCHeader.getPktType() == kConquestHeader::BB_DATA && inDestField){
        HandleData(packet,kCHeader);
      }
      // Received NewData Indicator, we have to generate and send new data to fUp
      else if(kCHeader.getPktType() == kConquestHeader::BB_ND_IND && inDestField){
        HandleBBAck(packet,kCHeader);
      }
    }
}

void kConquestLeaf::HandleDataAck (Ptr<Packet> packet, kConquestHeader kCHeader)
{
  // Search if ACKED Seq is in outstanding list
  for (auto iter=m_outStandingPackets.begin(); iter != m_outStandingPackets.end();)
  {
    if((*iter).seqNum == kCHeader.getSeqNum() && (*iter).inDestIDs(kCHeader.getSrcID()))
    {
      m_eventTrace(std::to_string(Now().GetMilliSeconds())+" N: "+std::to_string(m_ID)+"\tRX\t"+ kCHeader.PKTTYPENAMES[kCHeader.getPktType()] +"\t"+ "("+std::to_string((*iter).payloadID)+","+std::to_string((*iter).seqNum)+")\t S "+ std::to_string(kCHeader.getSrcID()));
      iter = m_outStandingPackets.erase(iter); 
    }
    else
    {
      iter++;
    }
  }
}

void kConquestLeaf::HandleBBAck (Ptr<Packet> packet, kConquestHeader kCHeader)
{
  // Only generate data if we didn't already, implicitly acks bb-c-Ack
  uint16_t seqNum = kCHeader.getSeqNum();
  // Check if I am allowed to increase seqnum
  if(m_ID!=m_firstInSeq)
  {
    seqNum--;
  }

  if(seqNum==m_currSeq){
    m_eventTrace(std::to_string(Now().GetMilliSeconds())+" N: "+std::to_string(m_ID)+"\tRX\t"+"C-ACK\t"+ "("+std::to_string(kCHeader.getPayloadID())+","+std::to_string(m_currSeq+1)+")\t C "+std::to_string(kCHeader.getCAckIDs()));
    GenerateNewData();
  }
}

void kConquestLeaf::HandleData (Ptr<Packet> packet, kConquestHeader kCHeader)
{
  m_eventTrace(std::to_string(Now().GetMilliSeconds())+" N: "+std::to_string(m_ID)+"\tRX\t"+"ND\t"+ "("+std::to_string(kCHeader.getPayloadID())+","+std::to_string(kCHeader.getSeqNum())+")\t S "+std::to_string(kCHeader.getSrcID()));
 
  kCHeader.setSrcID (m_ID);
  kCHeader.resetDestIDs();
  kCHeader.addDestID (m_fUp);
  kCHeader.setPktType (kConquestHeader::BB_DATA_ACK);
  kCHeader.setCAckIDs (0);
  Ptr<Packet> p = Create<Packet> (kCHeader.GetSerializedSize()); // 8+4 : the size of the seqTs header
  p->AddHeader (kCHeader);
  
  m_sendEvent = Simulator::Schedule (Seconds (0.0), &kConquestLeaf::ScheduleSend, this, p->Copy());
  m_eventTrace(std::to_string(Now().GetMilliSeconds())+" N: "+std::to_string(m_ID)+"\tTX\t"+kCHeader.PKTTYPENAMES[kCHeader.getPktType()]+"\t"+ "("+std::to_string(kCHeader.getPayloadID())+","+std::to_string(kCHeader.getSeqNum())+")\t D " + kCHeader.printDestIDs());
}

std::string 
kConquestLeaf::printNodeInfo ()
{
  std::stringstream returnString;
  returnString << "------------------------------------" << std::endl;
  returnString << "Node ID: \t\t" << std::to_string(m_ID) << std::endl;
  returnString << "Max ID: \t\t" << std::to_string(m_maxID) << std::endl;
  returnString << "Node Type: \t\t" << std::to_string(m_type) << std::endl;
  returnString << "First in Seq: \t" << std::to_string(m_firstInSeq) << std::endl;
  returnString << "Next in Seq: \t" << std::to_string(m_nextInSeq) << std::endl;
  returnString << "Forward Up: \t" << std::to_string(m_fUp) << std::endl;
  returnString << "------------------------------------" << std::endl;
  return returnString.str();
}

} // Namespace ns3
