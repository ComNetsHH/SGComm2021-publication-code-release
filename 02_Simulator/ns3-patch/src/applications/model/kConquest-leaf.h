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

#ifndef KCONQEUSTLEAF_H
#define KCONQEUSTLEAF_H

#include "ns3/application.h"
#include "ns3/ptr.h"
#include "ns3/ipv4-address.h"
#include "ns3/kConquest-client.h"
#include "kConquest-header.h"

namespace ns3 {

class Socket;
class Packet;

/**
 * \ingroup kConquestClient
 */
class kConquestLeaf : public kConquestClient
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  kConquestLeaf ();
  virtual ~kConquestLeaf ();

  std::string printNodeInfo ();

protected:
  virtual void DoDispose (void);

private:

  virtual void StartApplication (void);
  virtual void HandleRead (Ptr<Socket>);

  void HandleData (Ptr<Packet>,kConquestHeader);
  void HandleDataAck (Ptr<Packet>,kConquestHeader);
  void HandleBBAck (Ptr<Packet>,kConquestHeader);
};

} // namespace ns3

#endif /* KCONQEUSTLEAF_H */
