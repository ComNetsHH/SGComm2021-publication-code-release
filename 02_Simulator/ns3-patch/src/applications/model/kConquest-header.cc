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


#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/header.h"
#include "ns3/simulator.h"
#include "kConquest-header.h"
#include <vector>
#include <sstream>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("kConquestHeader");

NS_OBJECT_ENSURE_REGISTERED (kConquestHeader);

kConquestHeader::kConquestHeader ()
{
  NS_LOG_FUNCTION (this);


  // Identifier
  m_payloadID = 0;
  m_seqNum = 0;

  // Control Info
  m_nextInSeq = 0;
  m_pktType = kConquestHeader::DATA;
  m_cAckIDs = 0;

  // Routing
  m_srcID = 0;
  m_blockedID = 0;

  m_rtxID = 0;
}


TypeId
kConquestHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::kConquestHeader")
    .SetParent<Header> ()
    .SetGroupName ("Applications")
    .AddConstructor<kConquestHeader> ()
  ;
  return tid;
}

TypeId
kConquestHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
kConquestHeader::Print (std::ostream &os) const
{
  NS_LOG_FUNCTION (this << &os);
  os << "......." << std::endl;
  os << "SeqNum: " << std::to_string(m_seqNum) << std::endl;
  os << "Type: " << std::to_string(m_pktType) << std::endl;
  os << "Src ID: " << std::to_string(m_srcID) << std::endl;
  os << "cAck: " << std::to_string(m_cAckIDs) << std::endl;
  os << "Next in S: " << std::to_string(m_nextInSeq) << std::endl;
  os << "......." << std::endl;
}

uint32_t
kConquestHeader::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION (this);
  return sizeof(m_rtxID) + sizeof(m_seqNum) + sizeof(m_blockedID) + sizeof(m_payloadID) + sizeof(m_nextInSeq) + sizeof(uint16_t)*(m_destIDs.size()+1) + sizeof(m_srcID) + sizeof(m_pktType) + sizeof(m_cAckIDs);
}

uint16_t
kConquestHeader::getSeqNum()
{
  return m_seqNum;
};

void
kConquestHeader::setSeqNum(uint16_t num)
{
  m_seqNum = num;
};

uint16_t
kConquestHeader::getNextInSeq()
{
  return m_nextInSeq;
};

uint8_t
kConquestHeader::getRtxID(){
  return m_rtxID;
}

void
kConquestHeader::setRtxID(uint8_t rtxID){
  m_rtxID = rtxID;
};

void 
kConquestHeader::setNextInSeq(uint16_t ID)
{
  m_nextInSeq = ID;
};

kConquestHeader::pktType 
kConquestHeader::getPktType(){
  return m_pktType;
};

void 
kConquestHeader::setPktType(kConquestHeader::pktType type)
{
  m_pktType = type;
};

uint16_t 
kConquestHeader::getDestID(uint16_t pos)
{
  return m_destIDs.at(pos);
};

void 
kConquestHeader::addDestID(uint16_t ID)
{
  m_destIDs.push_back(ID);
};

void
kConquestHeader::removeDestID(uint16_t ID){
  auto iter = m_destIDs.begin();
  while(iter !=m_destIDs.end()){
    if((*iter) == ID){
      m_destIDs.erase(iter);
      return;
    }
    iter++;
  }
}


std::string
kConquestHeader::printDestIDs(){
  std::stringstream ss;
  ss << "[ ";
  auto iter = m_destIDs.begin();
  while(iter !=m_destIDs.end()){
    ss << std::to_string(*iter) << " ";
    iter++;
  }
  ss << "]";
  return ss.str();
}

void
kConquestHeader::resetDestIDs(){
  m_destIDs.clear();
}


uint16_t 
kConquestHeader::getSrcID()
{
  return m_srcID;
};

void 
kConquestHeader::setSrcID(uint16_t ID)
{
  m_srcID = ID;
};

uint16_t 
kConquestHeader::getCAckIDs()
{
  return m_cAckIDs;
};

void 
kConquestHeader::setCAckIDs(uint16_t cAcks)
{
  m_cAckIDs = cAcks;
};

uint16_t 
kConquestHeader::getPayloadID()
{
  return m_payloadID;
};

void 
kConquestHeader::setPayloadID(uint16_t payloadID)
{
  m_payloadID = payloadID;
};

uint16_t 
kConquestHeader::getBlockedID()
{
  return m_blockedID;
};

void 
kConquestHeader::setBlockedID(uint16_t blockID)
{
  m_blockedID = blockID;
};

bool 
kConquestHeader::inDestIDs(uint16_t destID){

  auto start = m_destIDs.begin();
  while(start != m_destIDs.end()){
    if(*start == destID){
      return true;
    }
    start++;
  }
  return false;
}

void
kConquestHeader::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  i.WriteU16 (m_destIDs.size());
  i.WriteU16 (m_seqNum);
  i.WriteU16 (m_nextInSeq);
  i.WriteU16 (m_srcID);
  i.WriteU64 (m_cAckIDs);
  i.WriteU8 (m_pktType);
  i.WriteU16 (m_payloadID);
  i.WriteU16 (m_blockedID);
  i.WriteU8 (m_rtxID);
  for (auto iter = m_destIDs.begin(); iter!=m_destIDs.end(); iter++){
    i.WriteU16(*iter);  
  }
}

uint32_t
kConquestHeader::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  uint16_t num_dests = i.ReadU16();
  m_seqNum = i.ReadU16 ();
  m_nextInSeq = i.ReadU16 ();
  m_srcID = i.ReadU16 ();
  m_cAckIDs = i.ReadU64();
  m_pktType = pktType(i.ReadU8 ());
  m_payloadID = i.ReadU16 ();
  m_blockedID = i.ReadU16 ();
  m_rtxID = i.ReadU8 ();
  m_destIDs.clear();
  while(num_dests>0){
    m_destIDs.push_back(i.ReadU16());
    num_dests--;
  }
  return GetSerializedSize ();
}

} // namespace ns3
