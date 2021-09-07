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

#ifndef KCONQUEST_H
#define KCONQUEST_H

#include "ns3/application.h"
#include "ns3/ptr.h"
#include "ns3/core-module.h"
#include "ns3/ipv4-address.h"
#include "kConquest-header.h"
#include <map>

namespace ns3 {

class Socket;
class Packet;

/**
 * \ingroup kConquestClient
 *
 * \brief ToDo
 *
 */
class kConquestClient : public Application
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  void HandleCycle ();

  kConquestClient ();
  virtual ~kConquestClient ();

  enum nodeType
  {
    KCONQUEST_LEAF,
    KCONQUEST_BB,
  };

  struct queueEntry
  {
    uint16_t srcID;
    std::vector<uint16_t> destIDs;
    uint16_t seqNum;
    uint16_t payloadID;
    kConquestHeader::pktType type;
    Ptr<Packet> packet;
    int64_t lastTransmission;
    int64_t nextTransmission;
    std::map<uint8_t,int64_t> retransmissions;
    uint8_t rtxCounter=0;
    uint16_t cID=0;

    std::string printDestIDs(){
        std::stringstream ss;
        ss << "[ ";
        auto iter = destIDs.begin();
        while(iter !=destIDs.end()){
          ss << std::to_string(*iter) << " ";
          iter++;
        }
        ss << "]";
        return ss.str();
      }

  bool inDestIDs(uint16_t destID){
      auto start = destIDs.begin();
      while(start != destIDs.end()){
        if(*start == destID){
          return true;
        }
        start++;
      }
      return false;
    }

  void removeDestID(uint16_t ID){
      auto iter = destIDs.begin();
      while(iter !=destIDs.end()){
        if((*iter) == ID){
          destIDs.erase(iter);
          return;
        }
        iter++;
     }  
    }
  };

  struct dataEntry
  {
    uint16_t payloadID;
    uint16_t seqNum;

    public:
      bool operator==(const dataEntry& other)
      {
        return (this->payloadID == other.payloadID) && (this->seqNum == other.seqNum); 
      }

  };

  Time m_interval;
  bool m_enableDynamicRTX;

  TracedCallback<std::string> m_eventTrace;
  
protected:
  virtual void DoDispose (void);

  virtual void StartApplication (void);
  virtual void StopApplication (void);

  virtual void Send (Ptr<Packet>);
  void ScheduleSend (Ptr<Packet>);
  virtual void GenerateNewData ();

  uint32_t m_size; //!< Size of the sent packet (including the SeqTsHeader)
  uint32_t m_sent; //!< Counter for sent packets
  uint16_t m_currSeq; //!< Current sequence number

  // kConquest Parameter
  uint16_t m_ID;
  uint16_t m_maxID;
  uint16_t m_firstInSeq;
  uint16_t m_nextInSeq;
  enum nodeType m_type;
  uint16_t m_fUp;

  std::list<queueEntry> m_outStandingPackets;
  std::vector<dataEntry> m_seenData; //!< Stores received (ID,seqNum) pairs 
  
  Ptr<Socket> m_socket; //!< Socket
  Address m_peerAddress; //!< Remote peer address
  uint16_t m_peerPort; //!< Remote peer port
  EventId m_sendEvent; //!< Event to send the next packet
  Ptr<UniformRandomVariable> m_processingDelay; //!< Draw random processing delay

};

} // namespace ns3

#endif /* KCONQEUST_H */
