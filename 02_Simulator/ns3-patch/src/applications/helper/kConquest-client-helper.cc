/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2008 INRIA
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
 * Author: Mohamed Amine Ismail <amine.ismail@sophia.inria.fr>
 * Adaptation: Leonard Fisser <leonard.fisser@tuhh.de>
 */

#include "ns3/kConquest-client-helper.h"
#include "ns3/kConquest-client.h"
#include "ns3/kConquest-leaf.h"
#include "ns3/kConquest-bb.h"
#include "ns3/udp-trace-client.h"
#include "ns3/uinteger.h"
#include "ns3/string.h"

namespace ns3 {

kConquestBBHelper::kConquestBBHelper ()
{
  m_factory.SetTypeId (kConquestBB::GetTypeId ());
}

kConquestBBHelper::kConquestBBHelper (uint16_t port)
{
  m_factory.SetTypeId (kConquestBB::GetTypeId ());
}

void
kConquestBBHelper::SetAttribute (std::string name, const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
kConquestBBHelper::Install (NodeContainer c)
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Ptr<Node> node = *i;

      m_server = m_factory.Create<kConquestBB> ();
      node->AddApplication (m_server);
      apps.Add (m_server);

    }
  return apps;
}

Ptr<kConquestBB>
kConquestBBHelper::GetServer (void)
{
  return m_server;
}

kConquestLeafHelper::kConquestLeafHelper ()
{
  m_factory.SetTypeId (kConquestLeaf::GetTypeId ());
}

kConquestLeafHelper::kConquestLeafHelper (Address address, uint16_t port)
{
  m_factory.SetTypeId (kConquestLeaf::GetTypeId ());
  SetAttribute ("RemoteAddress", AddressValue (address));
  SetAttribute ("RemotePort", UintegerValue (port));
}

kConquestLeafHelper::kConquestLeafHelper (Address address)
{
  m_factory.SetTypeId (kConquestLeaf::GetTypeId ());
  SetAttribute ("RemoteAddress", AddressValue (address));
}

void
kConquestLeafHelper::SetAttribute (std::string name, const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
kConquestLeafHelper::Install (NodeContainer c)
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Ptr<Node> node = *i;
      Ptr<kConquestLeaf> client = m_factory.Create<kConquestLeaf> ();
      node->AddApplication (client);
      apps.Add (client);
    }
  return apps;
}

kConquestTraceClientHelper::kConquestTraceClientHelper ()
{
  m_factory.SetTypeId (UdpTraceClient::GetTypeId ());
}

kConquestTraceClientHelper::kConquestTraceClientHelper (Address address, uint16_t port, std::string filename)
{
  m_factory.SetTypeId (UdpTraceClient::GetTypeId ());
  SetAttribute ("RemoteAddress", AddressValue (address));
  SetAttribute ("RemotePort", UintegerValue (port));
  SetAttribute ("TraceFilename", StringValue (filename));
}

kConquestTraceClientHelper::kConquestTraceClientHelper (Address address, std::string filename)
{
  m_factory.SetTypeId (UdpTraceClient::GetTypeId ());
  SetAttribute ("RemoteAddress", AddressValue (address));
  SetAttribute ("TraceFilename", StringValue (filename));
}

void
kConquestTraceClientHelper::SetAttribute (std::string name, const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
kConquestTraceClientHelper::Install (NodeContainer c)
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Ptr<Node> node = *i;
      Ptr<UdpTraceClient> client = m_factory.Create<UdpTraceClient> ();
      node->AddApplication (client);
      apps.Add (client);
    }
  return apps;
}

} // namespace ns3
