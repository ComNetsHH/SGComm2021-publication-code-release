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
#include "kConquest-bb.h"
#include "seq-ts-header.h"
#include "kConquest-header.h"
#include "ns3/simulator.h"


namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("kConquestBB");

NS_OBJECT_ENSURE_REGISTERED (kConquestBB);

TypeId
kConquestBB::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::kConquestBB")
    .SetParent<kConquestClient> ()
    .SetGroupName("Applications")
    .AddConstructor<kConquestBB> ()
    .AddAttribute ("BackBoneHop",
                    "If node is BB, then this contains the next BB hop ID for each node in BackBoneHopDest.",
                    StringValue (""),
                    MakeStringAccessor (&kConquestBB::m_bbHop),
                    MakeStringChecker ())
    .AddAttribute ("BackBoneHopDest",
                   "An array containing the logical order of nodes in the topology.",
                   StringValue (""),
                   MakeStringAccessor (&kConquestBB::m_bbHopDest),
                   MakeStringChecker ())
    .AddAttribute ("BackBoneNeighbours",
                   "If node is BB, then this contains all adjacent BB nodes.",
                   StringValue (""),
                   MakeStringAccessor (&kConquestBB::m_bbNeighbours),
                   MakeStringChecker ())
    .AddAttribute ("ForwardDown",
                   "If node is BB, then this contains the associated children IDs.",
                   StringValue (""),
                   MakeStringAccessor (&kConquestBB::m_fDown),
                   MakeStringChecker ())
    .AddTraceSource ("RtxTimerTrace", "Traces changes in the RTX timer of this node.",
                    MakeTraceSourceAccessor (&kConquestBB::m_rtxTrace),
                    "ns3::kConquestClient::RtxTimerTrace")
  ;
  return tid;
}

kConquestBB::kConquestBB ()
{
  kConquestClient();
}

kConquestBB::~kConquestBB ()
{
}

void
kConquestBB::DoDispose (void)
{
  Application::DoDispose ();
}

void
kConquestBB::DeSerializeDataStrings (void)
{
  std::string splitString;
  std::stringstream m_bbHopDestStream(m_bbHopDest);
  std::stringstream m_bbHopStream(m_bbHop);
  std::stringstream m_fDownStream(m_fDown);
  std::stringstream m_bbNeighboursStream(m_bbNeighbours);

  while(std::getline(m_bbHopDestStream, splitString, ','))
  {
    m_bbHopDestIDs.push_back(std::stoi(splitString));
  }
  while(std::getline(m_bbHopStream, splitString, ','))
  {
    m_bbHopIDs.push_back(std::stoi(splitString));
  }
  while(std::getline(m_fDownStream, splitString, ','))
  {
    m_fDownIDs.push_back(std::stoi(splitString));
  }
  while(std::getline(m_bbNeighboursStream, splitString, ','))
  {
    m_bbNeighboursIDs.push_back(std::stoi(splitString));
  }
}

void
kConquestBB::StartApplication (void)
{
  DeSerializeDataStrings ();

  kConquestClient::StartApplication();
  m_socket->SetRecvCallback (MakeCallback (&kConquestBB::HandleRead, this));

  
  if(m_ID==m_firstInSeq)
  {
    GenerateNewData();
  }

  m_rtt = kConquestClient::m_interval.GetMilliSeconds();
  m_rtt_deviation = 0;
  
  Simulator::Schedule (m_interval, &kConquestClient::HandleCycle, this);
}

void
kConquestBB::updateRTXInterval (int64_t rttEstimate)
{
  double alpha = 0.125; //RFC 6298
  double beta = 0.25; // RFC 6298
  double rttM = double(rttEstimate);
  double diff = std::abs(m_rtt-rttM);
  // Update RTT Estimate
  m_rtt = (1-alpha)*m_rtt + alpha*rttM;
  m_rtt_deviation = (1-beta)*m_rtt_deviation + beta*diff; 
  kConquestClient::m_interval = MilliSeconds(m_rtt+4.0*m_rtt_deviation);
  m_rtxTrace(std::to_string(Now().GetMilliSeconds())+":"+std::to_string(m_ID)+":"+std::to_string(kConquestClient::m_interval.GetMilliSeconds())+":"+std::to_string(rttEstimate) );
}

void 
kConquestBB::HandlePacket (Ptr<Packet> packet)
{
  kConquestHeader kCHeader;
  packet->RemoveHeader(kCHeader);

  // Received data from BB Neighbour, we might have to send ACK back and forward data to fDown and BB Neighbours
  if(kCHeader.getPktType() == kConquestHeader::BB_DATA){
    HandleBBData(packet,kCHeader);
  }
  // Received BB data ACK from Neighbour, we might have to delete it from oustanding packet list
  if(kCHeader.getPktType() == kConquestHeader::BB_DATA_ACK){
    HandleBBAck(packet,kCHeader);
  }
  // Received new data from leaf node, we have to send ACK and forward to BB neighbours
  else if(kCHeader.getPktType() == kConquestHeader::DATA){
    HandleData(packet,kCHeader);
  }
  // Received data ACK from leaf, if we receive this for all leaf nodes, we delete packet from outstanding packet list
  else if(kCHeader.getPktType() == kConquestHeader::DATA_ACK){
    HandleDataAck(packet,kCHeader);
  }
  // Received c-data, either we are the dest or we have to forward furhter and send ACK back
  else if(kCHeader.getPktType() == kConquestHeader::BB_C_DATA){
    HandleBBCData(packet,kCHeader);
  }
  // Received c-data-ACK, we can delete c-data from out outstanding packet list
  else if(kCHeader.getPktType() == kConquestHeader::BB_C_DATA_ACK){
    HandleBBCDataAck(packet,kCHeader);
  }
}

void 
kConquestBB::HandleRead (Ptr<Socket> socket){
  Ptr<Packet> packet;
  Address from;
  Address localAddress;
  while ((packet = socket->RecvFrom (from)))
    {
      if (packet->GetSize () == 0)
      { //EOF
          break;
      }
      HandlePacket(packet);
    }
}

void kConquestBB::HandleBBCDataAck (Ptr<Packet> packet,kConquestHeader kCHeader){
  for(auto iter = m_outStandingPackets.begin(); iter != m_outStandingPackets.end();iter++){
    if( (iter->inDestIDs(kCHeader.getSrcID())) && (kCHeader.getDestID(0) == m_ID) && ((*iter).seqNum == kCHeader.getSeqNum()) && ((*iter).payloadID == kCHeader.getPayloadID()) && ((*iter).cID == kCHeader.getBlockedID()) ){
      m_eventTrace(std::to_string(Now().GetMilliSeconds())+" N: "+std::to_string(m_ID)+"\tRX\t"+kCHeader.PKTTYPENAMES[kCHeader.getPktType()]+"\t"+ "("+std::to_string(kCHeader.getPayloadID())+","+std::to_string(kCHeader.getSeqNum())+")\t S "+std::to_string(kCHeader.getSrcID()));
      // Remove from dest from retransmission list
      iter->removeDestID(kCHeader.getSrcID());
      // Remove from packet header dest field
      kConquestHeader packetHeader;
      (*iter).packet->RemoveHeader(packetHeader);
      packetHeader.removeDestID(kCHeader.getSrcID());
      (*iter).packet->AddHeader(packetHeader);
      // Remove entry from list if no dest left
      if(iter->destIDs.size()==0){
        iter = m_outStandingPackets.erase(iter);
      }
    }
  }
}

void kConquestBB::HandleBBCData (Ptr<Packet> packet,kConquestHeader kCHeader){
  // Check if we have a progress entry for this cData
  bool selfMsg = (kCHeader.getSrcID()==m_ID);

  cDataEntry testEntry;
  testEntry.payloadID = kCHeader.getPayloadID();
  testEntry.seqNum = kCHeader.getSeqNum();
  auto iter = std::find(m_cDataReceived.begin(),m_cDataReceived.end(),testEntry);

  if(kCHeader.inDestIDs(m_ID)){
    if(!selfMsg){
      m_eventTrace(std::to_string(Now().GetMilliSeconds())+" N: "+std::to_string(m_ID)+"\tRX\t"+kCHeader.PKTTYPENAMES[kCHeader.getPktType()]+"\t"+ "("+std::to_string(kCHeader.getPayloadID())+","+std::to_string(kCHeader.getSeqNum())+")\t S "+std::to_string(kCHeader.getSrcID()) + "\t" + std::to_string(kCHeader.getBlockedID()));
    }
    bool fullForward = false;

    if(iter==m_cDataReceived.end()){
      // Create new entry to keep track of current outstanding data
      cDataEntry newEntry;
      newEntry.payloadID = kCHeader.getPayloadID();
      newEntry.seqNum = kCHeader.getSeqNum();
      newEntry.srcIDs.push_back(kCHeader.getBlockedID());
      newEntry.numAcks = kCHeader.getCAckIDs();
      newEntry.alreadySent = false;
      m_cDataReceived.push_back(newEntry);
      iter = std::find(m_cDataReceived.begin(),m_cDataReceived.end(),testEntry);
      fullForward = true;
    }
    else{
      // check if we already recevied the node for this cData
      if((*iter).inSrcIDs(kCHeader.getBlockedID())){

      }
      else{
        (*iter).srcIDs.push_back(kCHeader.getBlockedID());
        (*iter).numAcks += kCHeader.getCAckIDs();
        fullForward = true;
      }
    }

    // Adjust Header and forward cData
    if(getNextHopCAck(kCHeader.getNextInSeq())!=m_ID && fullForward){
      kCHeader.setSrcID(m_ID);
      kCHeader.resetDestIDs();
      kCHeader.addDestID(getNextHopCAck(kCHeader.getNextInSeq()));
      scheduleData_C(packet,kCHeader,kCHeader.getBlockedID());
    }
    // Send a simple ACK if I am final destination
    else{
      if(!selfMsg){
        scheduleAck(packet,kCHeader,kConquestHeader::BB_C_DATA_ACK);
      }
    }

    // Check if we can generate new data at this node
    if((*iter).numAcks==m_maxID && !(*iter).alreadySent){
      (*iter).alreadySent = true;
      if(kCHeader.getNextInSeq()!=m_ID){
        scheduleData_ND(kCHeader.getNextInSeq(),kCHeader.getSeqNum());
      }
      else{
        GenerateNewData();
      }
    }

  }
  // If i am not in the destinations, it could still be an implicit ACK
  else{
    kCHeader.resetDestIDs();
    kCHeader.addDestID(m_ID);
    HandleBBCDataAck(packet,kCHeader);
  }
}

void kConquestBB::HandleDataAck (Ptr<Packet> packet,kConquestHeader kCHeader){

}

void kConquestBB::HandleBBData (Ptr<Packet> packet, kConquestHeader kCHeader)
{
  if(kCHeader.inDestIDs(m_ID)){
    m_eventTrace(std::to_string(Now().GetMilliSeconds())+" N: "+std::to_string(m_ID)+"\tRX\t"+kCHeader.PKTTYPENAMES[kCHeader.getPktType()]+"\t"+ "("+std::to_string(kCHeader.getPayloadID())+","+std::to_string(kCHeader.getSeqNum())+")\t S "+std::to_string(kCHeader.getSrcID()));
    scheduleAck(packet,kCHeader,kConquestHeader::BB_DATA_ACK);

    // Check if we know the received data, if not forward to fDown and bb neighbours
    dataEntry maybeNewData = {kCHeader.getPayloadID(),kCHeader.getSeqNum()};
    auto wasProcessed = (std::find(m_seenData.begin(),m_seenData.end(),maybeNewData)!=m_seenData.end());
    if(!wasProcessed){
      // Add data to our memory
      m_seenData.push_back(maybeNewData);
      scheduleData(packet,kCHeader);
      // If we are the custodian node for the next in sequence, create new Entry
    }
  }
}

void kConquestBB::HandleBBAck (Ptr<Packet> packet, kConquestHeader kCHeader)
{
  for(auto iter = m_outStandingPackets.begin(); iter != m_outStandingPackets.end();iter++){
    if( iter->inDestIDs(kCHeader.getSrcID()) && (iter->seqNum == kCHeader.getSeqNum()) && (iter->payloadID == kCHeader.getPayloadID()) && (iter->type!=kConquestHeader::BB_C_DATA)){
      m_eventTrace(std::to_string(Now().GetMilliSeconds())+" N: "+std::to_string(m_ID)+"\tRX\t"+kCHeader.PKTTYPENAMES[kCHeader.getPktType()]+"\t"+ "("+std::to_string(kCHeader.getPayloadID())+","+std::to_string(kCHeader.getSeqNum())+")\t S "+std::to_string(kCHeader.getSrcID()));
      // Use RTT estimator to adjust RTX timer
      // RTX BOOK
      if(kConquestClient::m_enableDynamicRTX){
        updateRTXInterval(Now().GetMilliSeconds() - iter->retransmissions[kCHeader.getRtxID()]);
      }
      // Remove from dest from retransmission list
      iter->removeDestID(kCHeader.getSrcID());
      // Remove from packet header dest field
      kConquestHeader packetHeader;
      (*iter).packet->RemoveHeader(packetHeader);
      packetHeader.removeDestID(kCHeader.getSrcID());
      (*iter).packet->AddHeader(packetHeader);

      // Remove entry from list if no dest left
      if(iter->destIDs.size()==0){
        auto AckType = iter->type;
        iter = m_outStandingPackets.erase(iter);

        if(AckType == kConquestHeader::BB_DATA){
          // We distributed data to all target nodes, send ACK to next hop towards next node in sequence
          kConquestHeader cAckCHeader;
          cAckCHeader.setSrcID (m_ID);
          cAckCHeader.setCAckIDs (m_fDownIDs.size());
          cAckCHeader.setPktType (kConquestHeader::BB_C_DATA);
          cAckCHeader.setBlockedID(m_ID); 
          cAckCHeader.resetDestIDs();
          // Find next hop
          cAckCHeader.addDestID (getNextHopCAck(packetHeader.getNextInSeq()));
          cAckCHeader.setNextInSeq (packetHeader.getNextInSeq());
          cAckCHeader.setSeqNum (packetHeader.getSeqNum());
          cAckCHeader.setPayloadID(packetHeader.getPayloadID());
          Ptr<Packet> ackPacket = Create<Packet> (cAckCHeader.GetSerializedSize());
          
          if(cAckCHeader.getDestID(0)!=m_ID){
            scheduleData_C(ackPacket,cAckCHeader,m_ID);
          }
          // Directly loopback into cDataAck Handling
          else{
            cAckCHeader.setSrcID (m_ID);
            cAckCHeader.setCAckIDs (m_fDownIDs.size());
            HandleBBCData(ackPacket,cAckCHeader);
          }
        }
      }
      break;
    }
  }
}

uint16_t kConquestBB::getNextHopCAck(uint16_t destID){
  std::vector<uint16_t>::iterator iter = std::find(m_bbHopDestIDs.begin(),m_bbHopDestIDs.end(),destID);
  if(iter!=m_bbHopDestIDs.end()){
    uint16_t offset = std::distance(m_bbHopDestIDs.begin(),iter);
    return m_bbHopIDs.at(offset);
  }
  return 0;
}


void
kConquestBB::scheduleAck(Ptr<Packet> packet, kConquestHeader kCHeader, kConquestHeader::pktType ackType){
  kConquestHeader kAckCHeader;
  kCHeader.resetDestIDs();
  kCHeader.addDestID (kCHeader.getSrcID());
  kCHeader.setSrcID (m_ID);
  kCHeader.setPktType (ackType);
  Ptr<Packet> p = Create<Packet> (kCHeader.GetSerializedSize());
  p->AddHeader (kCHeader);
  m_sendEvent = Simulator::Schedule (Seconds (0.0), &kConquestBB::ScheduleSend, this, p->Copy());
  m_eventTrace(std::to_string(Now().GetMilliSeconds())+" N: "+std::to_string(m_ID)+"\tTX\t"+ kCHeader.PKTTYPENAMES[kCHeader.getPktType()] +"\t"+ "("+std::to_string(kCHeader.getPayloadID())+","+std::to_string(kCHeader.getSeqNum())+")\t D " + kCHeader.printDestIDs());
}

void
kConquestBB::scheduleData(Ptr<Packet> packet, kConquestHeader kCHeader){

  kConquestClient::queueEntry newEntry;
  uint16_t srcID = kCHeader.getSrcID();
  kCHeader.setSrcID (m_ID);
  newEntry.srcID = m_ID;
  kCHeader.resetDestIDs();
  for (auto iter=m_fDownIDs.begin();iter!=m_fDownIDs.end();iter++){
    if(((*iter)!=m_ID) && (*iter!=srcID)){
      kCHeader.addDestID(*iter);
      newEntry.destIDs.push_back(*iter);
    }
  }
  for (auto iter=m_bbNeighboursIDs.begin();iter!=m_bbNeighboursIDs.end();iter++){
    if(((*iter)!=m_ID) && (*iter!=srcID)){
      kCHeader.addDestID(*iter);
      newEntry.destIDs.push_back(*iter);
    }
  }
  kCHeader.setPktType (kConquestHeader::BB_DATA);
  kCHeader.setBlockedID (m_ID);
  // Reset retransmission index
  kCHeader.setRtxID (0);
  packet->AddHeader(kCHeader);

  newEntry.type = kConquestHeader::BB_DATA;
  newEntry.seqNum = kCHeader.getSeqNum();
  newEntry.payloadID = kCHeader.getPayloadID();
  newEntry.packet = packet;
  newEntry.lastTransmission = Simulator::Now().GetMilliSeconds();
  newEntry.nextTransmission = Simulator::Now().GetMilliSeconds() + m_interval.GetMilliSeconds();
  newEntry.retransmissions[0] = newEntry.lastTransmission;
  m_outStandingPackets.push_back(newEntry);
  m_eventTrace(std::to_string(Now().GetMilliSeconds())+" N: "+std::to_string(m_ID)+"\tTX\t" + kCHeader.PKTTYPENAMES[kCHeader.getPktType()] +"\t"+ "("+std::to_string(kCHeader.getPayloadID())+","+std::to_string(kCHeader.getSeqNum())+")\t D "+kCHeader.printDestIDs());
  m_sendEvent = Simulator::Schedule (Seconds (0.0), &kConquestBB::ScheduleSend, this, packet->Copy());
};


void kConquestBB::HandleData (Ptr<Packet> packet, kConquestHeader kCHeader)
{
  // If we are custodian node for the packet source, send ACK
  bool selfMsg = (kCHeader.getSrcID()==m_ID);
  if(std::find(m_fDownIDs.begin(), m_fDownIDs.end(), kCHeader.getSrcID()) != m_fDownIDs.end())
  {
    if(!selfMsg){
    m_eventTrace(std::to_string(Now().GetMilliSeconds())+" N: "+std::to_string(m_ID)+"\tRX\t"+kCHeader.PKTTYPENAMES[kCHeader.getPktType()]+"\t"+ "("+std::to_string(kCHeader.getPayloadID())+","+std::to_string(kCHeader.getSeqNum())+")\t S "+std::to_string(kCHeader.getSrcID()));
    scheduleAck(packet,kCHeader,kConquestHeader::DATA_ACK);
    }

    // Check if we know the received data, if not forward to fDown and bb neighbours
    dataEntry maybeNewData = {kCHeader.getPayloadID(),kCHeader.getSeqNum()};
    auto wasProcessed = (std::find(m_seenData.begin(),m_seenData.end(),maybeNewData)!=m_seenData.end());
    if(!wasProcessed){
      // Add data to our memory
      m_seenData.push_back(maybeNewData);
      scheduleData(packet,kCHeader);
    }

    // Implicit ack for ND indicator
    for(auto iter = m_outStandingPackets.begin(); iter != m_outStandingPackets.end();iter++){
      if((*iter).type == kConquestHeader::BB_ND_IND){
        // check if dest ID 
        if((*iter).destIDs.at(0) == kCHeader.getSrcID())
        {
          iter = m_outStandingPackets.erase(iter);
        }
      }

    }
    
  }
}

void
kConquestBB::scheduleData_C(Ptr<Packet> packet, kConquestHeader kCHeader, uint16_t cID){

  // Complete Header
  packet->AddHeader(kCHeader);

  // Create Retransmission Entry
  kConquestClient::queueEntry newEntry;
  newEntry.srcID = m_ID;
  newEntry.destIDs.push_back(kCHeader.getDestID(0));

  newEntry.type = kConquestHeader::BB_C_DATA;
  newEntry.seqNum = kCHeader.getSeqNum();
  newEntry.payloadID = kCHeader.getPayloadID();
  newEntry.packet = packet;
  newEntry.cID = cID;
  newEntry.lastTransmission = Simulator::Now().GetMilliSeconds();
  newEntry.nextTransmission = Simulator::Now().GetMilliSeconds() + m_interval.GetMilliSeconds();
  newEntry.retransmissions[0] = newEntry.lastTransmission;
  m_outStandingPackets.push_back(newEntry);
  m_eventTrace(std::to_string(Now().GetMilliSeconds())+" N: "+std::to_string(m_ID)+"\tTX\t" + kCHeader.PKTTYPENAMES[kCHeader.getPktType()] +"\t"+ "("+std::to_string(kCHeader.getPayloadID())+","+std::to_string(kCHeader.getSeqNum())+")\t D "+kCHeader.printDestIDs());
  //m_eventTrace(std::to_string(Now().GetMilliSeconds())+" N: "+std::to_string(m_ID)+"\tTX2\t" + kCHeader.PKTTYPENAMES[newEntry.type] +"\t"+ "("+std::to_string(newEntry.payloadID)+","+std::to_string(newEntry.seqNum)+")\t D "+newEntry.printDestIDs());

  m_sendEvent = Simulator::Schedule (Seconds (0.0), &kConquestBB::ScheduleSend, this, packet->Copy());
};  

void kConquestBB::scheduleData_ND(uint16_t nodeID, uint16_t seqNum){
  kConquestHeader kCHeader;
  kCHeader.setSrcID (m_ID);
  kCHeader.resetDestIDs();
  kCHeader.addDestID (nodeID);
  kCHeader.setNextInSeq (0);
  kCHeader.setPktType (kConquestHeader::BB_ND_IND);
  kCHeader.setSeqNum (seqNum);
  kCHeader.setCAckIDs (0);
  kCHeader.setPayloadID(nodeID);
  Ptr<Packet> packet = Create<Packet> (kCHeader.GetSerializedSize());
  packet->AddHeader (kCHeader);

  // Create Retransmission Entry
  kConquestClient::queueEntry newEntry;
  newEntry.srcID = m_ID;
  newEntry.destIDs.push_back(kCHeader.getDestID(0));

  newEntry.type = kConquestHeader::BB_ND_IND;
  newEntry.seqNum = kCHeader.getSeqNum();
  newEntry.payloadID = kCHeader.getPayloadID();
  Ptr<Packet> newPacket = Create<Packet>(kCHeader.GetSerializedSize());
  newPacket->AddHeader(kCHeader);
  newEntry.packet = newPacket;
  newEntry.lastTransmission = Simulator::Now().GetMilliSeconds();
  newEntry.nextTransmission = Simulator::Now().GetMilliSeconds() + m_interval.GetMilliSeconds();
  newEntry.retransmissions[0] = newEntry.lastTransmission;
  m_outStandingPackets.push_back(newEntry);

  m_eventTrace(std::to_string(Now().GetMilliSeconds())+" N: "+std::to_string(m_ID)+"\tTX\t" + kCHeader.PKTTYPENAMES[kCHeader.getPktType()] +"\t"+ "("+std::to_string(kCHeader.getPayloadID())+","+std::to_string(kCHeader.getSeqNum())+")\t D "+kCHeader.printDestIDs());
  m_sendEvent = Simulator::Schedule (Seconds (0.0), &kConquestBB::ScheduleSend, this, packet->Copy());
}    


std::string 
kConquestBB::printNodeInfo ()
{
  std::stringstream returnString;
  returnString << "------------------------------------" << std::endl;
  returnString << "Node ID: \t\t" << std::to_string(m_ID) << std::endl;
  returnString << "Max ID: \t\t" << std::to_string(m_maxID) << std::endl;
  returnString << "Node Type: \t\t" << std::to_string(m_type) << std::endl;
  returnString << "First in Seq: \t" << std::to_string(m_firstInSeq) << std::endl;
  returnString << "Next in Seq: \t" << std::to_string(m_nextInSeq) << std::endl;
  returnString << "Forward Up: \t" << std::to_string(m_fUp) << std::endl;
  returnString << "Forward Down: \t" << m_fDown<< std::endl;
  returnString << "BB Neighbours: \t" << m_bbNeighbours << std::endl;
  returnString << "BB Hop Dest: \t" << m_bbHopDest << std::endl;
  returnString << "BB Hop     : \t" << m_bbHop << std::endl;
  returnString << "------------------------------------" << std::endl;
  return returnString.str();
}

void
kConquestBB::GenerateNewData (){
    m_currSeq++;

    kConquestHeader kCHeader;
    kCHeader.setSrcID (m_ID);
    kCHeader.resetDestIDs();
    kCHeader.addDestID (m_ID);
    kCHeader.setNextInSeq (m_nextInSeq);
    kCHeader.setPktType (kConquestHeader::pktType::DATA);
    kCHeader.setPayloadID(m_ID);
    kCHeader.setSeqNum (m_currSeq);
    kCHeader.setCAckIDs (0);
    Ptr<Packet> p = Create<Packet> (m_size-kCHeader.GetSerializedSize());
    p->AddHeader (kCHeader);

    // We can pass packet to handle packet, as if it was received from a leaf node
    m_eventTrace(std::to_string(Now().GetMilliSeconds())+" N: "+std::to_string(m_ID)+"\tTX\t"+kCHeader.PKTTYPENAMES[kCHeader.getPktType()]+"\t"+ "("+std::to_string(kCHeader.getPayloadID())+","+std::to_string(kCHeader.getSeqNum())+")\t D " + std::to_string(kCHeader.getSrcID()));
    HandlePacket(p);
}

} // Namespace ns3
