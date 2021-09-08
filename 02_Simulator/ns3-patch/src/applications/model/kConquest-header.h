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

#ifndef KCONQUESTHEADER_H
#define KCONQUESTHEADER_H

#include "ns3/header.h"
#include "ns3/nstime.h"

namespace ns3 {

/**
 * \ingroup applications
 * \class kConquestHeader
 * \brief Packet header for kConquest protocol
 *
 */
class kConquestHeader : public Header
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * \brief constructor
   */
  kConquestHeader ();
  
  // Inherited
  virtual TypeId GetInstanceTypeId (void) const override;
  virtual void Print (std::ostream &os) const override;
  virtual uint32_t GetSerializedSize (void) const override;
  virtual void Serialize (Buffer::Iterator start) const override;
  virtual uint32_t Deserialize (Buffer::Iterator start) override;

  enum pktType : uint8_t
  {
    DATA,
    DATA_ACK,
    BB_DATA,
    BB_DATA_ACK,
    BB_C_DATA, 
    BB_C_DATA_ACK,   
    BB_ND_IND
  };

  std::string PKTTYPENAMES[7] = {"D",
                                 "D-Ack",
                                 "BB-D",
                                 "BB-D-Ack", 
                                 "C-D",   
                                 "C-D-Ack",
                                 "ND-Ind"};

  uint16_t getSeqNum();
  void setSeqNum(uint16_t);

  uint16_t getNextInSeq();
  void setNextInSeq(uint16_t);

  pktType getPktType();
  void setPktType(pktType);

  uint16_t getDestID(uint16_t);
  void addDestID(uint16_t);
  void removeDestID(uint16_t);
  void resetDestIDs();
  std::string printDestIDs();


  uint16_t getSrcID();
  void setSrcID(uint16_t);

  uint16_t getCAckIDs();
  void setCAckIDs(uint16_t);

  uint16_t getPayloadID();
  void setPayloadID(uint16_t);

  uint16_t getBlockedID();
  void setBlockedID(uint16_t);

  uint8_t getRtxID();
  void setRtxID(uint8_t);

  bool inDestIDs(uint16_t);

private:

  // Identifier
  uint16_t m_payloadID;
  uint16_t m_seqNum;

  // Control Info
  uint16_t m_nextInSeq;
  pktType m_pktType;
  uint64_t m_cAckIDs;

  // Routing
  uint16_t m_srcID;
  std::vector<uint16_t> m_destIDs;
  uint16_t m_blockedID;

  uint8_t m_rtxID;

};

} // namespace ns3

#endif /* KCONQUESTHEADER_H */
