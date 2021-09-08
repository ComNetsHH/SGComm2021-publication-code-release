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

#ifndef KCONQEUSTBB_H
#define KCONQEUSTBB_H

#include "ns3/application.h"
#include "ns3/ptr.h"
#include "ns3/ipv4-address.h"
#include "ns3/kConquest-client.h"
#include "kConquest-header.h"

namespace ns3 {

class Socket;
class Packet;

/**
 * \ingroup kConquestBB
 */
class kConquestBB : public kConquestClient
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  kConquestBB ();
  virtual ~kConquestBB ();

  virtual std::string printNodeInfo ();

  struct cDataEntry
  {
    uint16_t payloadID;
    uint16_t seqNum;
    std::vector<uint16_t> srcIDs;
    uint16_t numAcks;
    bool alreadySent;

    public:
    bool operator==(const cDataEntry& other)
    {
      return (this->payloadID == other.payloadID) && (this->seqNum == other.seqNum); 
    }

    bool inSrcIDs(uint16_t srcID){
      auto start = srcIDs.begin();
      while(start != srcIDs.end()){
        if(*start == srcID){
          return true;
        }
        start++;
      }
      return false;
    }
  };

  TracedCallback<std::string> m_rtxTrace;

protected:
  virtual void DoDispose (void);

private:
  void DeSerializeDataStrings (void);
  virtual void StartApplication (void);
  virtual void HandleRead (Ptr<Socket>);
  void HandlePacket (Ptr<Packet> packet);

  void updateRTXInterval (int64_t);



  void HandleData (Ptr<Packet>,kConquestHeader);
  void HandleDataAck (Ptr<Packet>,kConquestHeader);

  void HandleBBData (Ptr<Packet>,kConquestHeader);
  void HandleBBAck (Ptr<Packet>,kConquestHeader);

  void HandleBBCData(Ptr<Packet>,kConquestHeader);
  void HandleBBCDataAck(Ptr<Packet>,kConquestHeader);

  void scheduleData(Ptr<Packet>,kConquestHeader);
  void scheduleData_C(Ptr<Packet>,kConquestHeader,uint16_t);
  void scheduleData_ND(uint16_t,uint16_t);    
  void forwardData_C(Ptr<Packet>,uint16_t,kConquestHeader::pktType);
  void scheduleAck(Ptr<Packet>,kConquestHeader,kConquestHeader::pktType);

  uint16_t getNextHopCAck(uint16_t);

  virtual void GenerateNewData ();
  
  std::string m_bbHopDest;
  std::vector<uint16_t> m_bbHopDestIDs;

  std::string m_bbHop;
  std::vector<uint16_t> m_bbHopIDs;

  std::string m_fDown;
  std::vector<uint16_t> m_fDownIDs;

  std::string m_bbNeighbours;
  std::vector<uint16_t> m_bbNeighboursIDs;

  std::vector<cDataEntry> m_cDataReceived;

  bool m_enableDynamicRTX;
  double m_rtt;
  double m_rtt_deviation;
};

} // namespace ns3

#endif /* KCONQEUSTBB_H */
