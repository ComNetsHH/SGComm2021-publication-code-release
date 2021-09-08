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
#include "ns3/kConquest-client.h"
#include "seq-ts-header.h"
#include "kConquest-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("kConquestClient");

NS_OBJECT_ENSURE_REGISTERED (kConquestClient);

TypeId
kConquestClient::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::kConquestClient")
    .SetParent<Application> ()
    .SetGroupName("Applications")
    .AddConstructor<kConquestClient> ()
    .AddAttribute ("RemoteAddress",
                   "The destination Address of the outbound packets",
                   AddressValue (),
                   MakeAddressAccessor (&kConquestClient::m_peerAddress),
                   MakeAddressChecker ())
    .AddAttribute ("RemotePort", "The destination port of the outbound packets",
                   UintegerValue (100),
                   MakeUintegerAccessor (&kConquestClient::m_peerPort),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("PacketSize",
                   "Size of packets generated. The minimum packet size is 12 bytes which is the size of the header carrying the sequence number and the time stamp.",
                   UintegerValue (1024),
                   MakeUintegerAccessor (&kConquestClient::m_size),
                   MakeUintegerChecker<uint32_t> (12,65507))
    .AddAttribute ("ID",
                   "The unique ID of the node in the whole network.",
                   UintegerValue (1),
                   MakeUintegerAccessor (&kConquestClient::m_ID),
                   MakeUintegerChecker<uint16_t> (1,1000))
    .AddAttribute ("MaxID",
                   "The highest ID of any node in the whole network, indicating the number of expecte Acknowledgements.",
                   UintegerValue (1),
                   MakeUintegerAccessor (&kConquestClient::m_maxID),
                   MakeUintegerChecker<uint16_t> (1,1000))
    .AddAttribute ("Type",
                   "The type of the node.",
                   EnumValue (kConquestClient::KCONQUEST_LEAF),
                   MakeEnumAccessor (&kConquestClient::m_type),
                   MakeEnumChecker (kConquestClient::KCONQUEST_LEAF, "Leaf",
                                    kConquestClient::KCONQUEST_BB, "BB-Node"))
    .AddAttribute ("FirstInSeq",
                   "The node ID of the first node in the flooding sequence.",
                   UintegerValue (1),
                   MakeUintegerAccessor (&kConquestClient::m_firstInSeq),
                   MakeUintegerChecker<uint16_t> (1,1000))
    .AddAttribute ("NextSeqID",
                   "The node ID of the subsequent node in the flooding sequence.",
                   UintegerValue (1),
                   MakeUintegerAccessor (&kConquestClient::m_nextInSeq),
                   MakeUintegerChecker<uint16_t> (1,1000))
    .AddAttribute ("ForwardUp",
                   "The nodes associated parent node ID.",
                   UintegerValue (1),
                   MakeUintegerAccessor (&kConquestClient::m_fUp),
                   MakeUintegerChecker<uint16_t> (1,1000))
    .AddAttribute ("TransmissionAttemptInterval",
                   "Interval in which a new transmission can be scheduled.",
                   TimeValue (MilliSeconds(40)),
                   MakeTimeAccessor (&kConquestClient::m_interval),
                   MakeTimeChecker ())
    .AddAttribute ("enableDynamicRTXTimer",
                   "Estimates RTT and sets RTX timer like RFC6298.",
                   BooleanValue (false),
                   MakeBooleanAccessor (&kConquestClient::m_enableDynamicRTX),
                   MakeBooleanChecker ())
    .AddTraceSource ("EventTrace", "Traces RX/TX/RTX events happening in the node.",
                     MakeTraceSourceAccessor (&kConquestClient::m_eventTrace),
                     "ns3::kConquestClient::EventTrace")
  ;
  return tid;
}

kConquestClient::kConquestClient ()
{
  m_sent = 0;
  m_currSeq = 0;
  m_sendEvent = EventId ();
  m_processingDelay = CreateObject<UniformRandomVariable>();
  m_processingDelay->SetAttribute ("Min", DoubleValue (5000));
  m_processingDelay->SetAttribute ("Max", DoubleValue (10000));
}

kConquestClient::~kConquestClient ()
{
}

void
kConquestClient::DoDispose (void)
{
  Application::DoDispose ();
}

void
kConquestClient::StartApplication (void)
{
  if (m_socket == 0)
  {
    TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
    m_socket = Socket::CreateSocket (GetNode (), tid);
    if (Ipv4Address::IsMatchingType(m_peerAddress) == true)
    {
      if (m_socket->Bind (InetSocketAddress (Ipv4Address::GetAny (), m_peerPort)) == -1)
      {
        NS_FATAL_ERROR ("Failed to bind socket");
      }
    }
    else
    {
      NS_ASSERT_MSG (false, "Incompatible address type: " << m_peerAddress);
    }
  }
  m_socket->SetAllowBroadcast (true);  
}

void
kConquestClient::StopApplication (void)
{
  Simulator::Cancel (m_sendEvent);
}

void
kConquestClient::Send (Ptr<Packet> p)
{
  Ptr<Packet> p_copy = p->Copy();
  std::stringstream peerAddressStringStream;
  peerAddressStringStream << Ipv4Address::ConvertFrom (m_peerAddress);
  if ((m_socket->SendTo (p_copy,0,InetSocketAddress (Ipv4Address::ConvertFrom (m_peerAddress), m_peerPort))) >= 0)
  { 
    m_sent++;
  } 
  else
  {
  }
}

void
kConquestClient::HandleCycle ()
{
  // Check current outstanding packets and retransmit
  for(auto iter = m_outStandingPackets.begin(); iter != m_outStandingPackets.end(); iter++)
  {
    kConquestHeader temp;
    if(Simulator::Now().GetMilliSeconds() >= (*iter).nextTransmission){
      (*iter).packet->RemoveHeader(temp);
      m_eventTrace(std::to_string(Now().GetMilliSeconds())+" N: "+std::to_string(m_ID)+"\tRTX\t" + temp.PKTTYPENAMES[temp.getPktType()] +"\t"+ "("+std::to_string(temp.getPayloadID())+","+std::to_string(temp.getSeqNum())+")\t D "+temp.printDestIDs());
      (*iter).lastTransmission = Simulator::Now().GetMilliSeconds();
      (*iter).nextTransmission = Simulator::Now().GetMilliSeconds() + m_interval.GetMilliSeconds();
      (*iter).rtxCounter = (*iter).rtxCounter+1;
      (*iter).retransmissions[(*iter).rtxCounter] = (*iter).lastTransmission;
      temp.setRtxID(temp.getRtxID()+1);
      (*iter).packet->AddHeader(temp);
      Simulator::Schedule (Seconds (0.0), &kConquestClient::ScheduleSend, this, (*iter).packet->Copy());
      break;
    }
  }
}

void
kConquestClient::ScheduleSend (Ptr<Packet> p)
{
  double proc_delay = m_processingDelay->GetValue();
  Simulator::Schedule (MicroSeconds (proc_delay), &kConquestClient::Send, this, p);
  Simulator::Schedule (MicroSeconds (proc_delay)+m_interval, &kConquestClient::HandleCycle, this);
}

void
kConquestClient::GenerateNewData (){

    m_currSeq++;
    kConquestHeader kCHeader;
    kCHeader.setSrcID (m_ID);
    kCHeader.addDestID (m_fUp);
    kCHeader.setPayloadID (m_ID);
    kCHeader.setNextInSeq (m_nextInSeq);
    kCHeader.setPktType (kConquestHeader::pktType::DATA);
    kCHeader.setSeqNum (m_currSeq);
    kCHeader.setCAckIDs (0);
    Ptr<Packet> p = Create<Packet> (m_size-kCHeader.GetSerializedSize());
    p->AddHeader (kCHeader);
    m_sendEvent = Simulator::Schedule (Seconds (0.0), &kConquestClient::ScheduleSend, this, p->Copy());
    m_eventTrace(std::to_string(Now().GetMilliSeconds())+" N: "+std::to_string(m_ID)+"\tTX\t"+kCHeader.PKTTYPENAMES[kCHeader.getPktType()]+"\t"+ "("+std::to_string(m_ID)+","+std::to_string(m_currSeq)+")\t D " + std::to_string(kCHeader.getDestID(0)));

    // Add SeqNum to outstanding packets
    kConquestClient::queueEntry newEntry;
    newEntry.seqNum = m_currSeq;
    newEntry.destIDs.push_back(m_fUp);
    newEntry.packet = p;
    newEntry.payloadID = m_ID;
    newEntry.type = kConquestHeader::DATA;
    newEntry.lastTransmission = Simulator::Now().GetMilliSeconds();
    newEntry.nextTransmission = Simulator::Now().GetMilliSeconds() + m_interval.GetMilliSeconds();

    newEntry.retransmissions[0] = newEntry.lastTransmission;
    m_outStandingPackets.push_back(newEntry);
}

} // Namespace ns3
