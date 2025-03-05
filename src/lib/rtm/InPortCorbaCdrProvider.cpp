﻿// -*- C++ -*-
/*!
 * @file  InPortCorbaCdrProvider.cpp
 * @brief InPortCorbaCdrProvider class
 * @date  $Date: 2008-01-14 07:49:59 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2009-2010
 *     Noriaki Ando
 *     Task-intelligence Research Group,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id: InPortCorbaCdrProvider.cpp 1244 2009-03-13 07:25:42Z n-ando $
 *
 */

#include <rtm/InPortCorbaCdrProvider.h>

namespace RTC
{
  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  InPortCorbaCdrProvider::InPortCorbaCdrProvider()
  {
    // PortProfile setting
    setInterfaceType("corba_cdr");

    // ConnectorProfile setting

#ifdef ORB_IS_OMNIORB
    PortableServer::ObjectId_var oid = ::RTC::Manager::instance().theShortCutPOA()->activate_object(this);
#endif
    m_objref = this->_this();

    // set InPort's reference
    CORBA::ORB_var orb = ::RTC::Manager::instance().getORB();
    CORBA::String_var ior = orb->object_to_string(m_objref.in());
    CORBA_SeqUtil::
      push_back(m_properties,
                NVUtil::newNV("dataport.corba_cdr.inport_ior", ior.in()));
    CORBA_SeqUtil::
      push_back(m_properties,
                NVUtil::newNV("dataport.corba_cdr.inport_ref", m_objref));
  }

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  InPortCorbaCdrProvider::~InPortCorbaCdrProvider()
  {
    try
      {
        PortableServer::ObjectId_var oid;
#ifdef ORB_IS_OMNIORB
        oid = ::RTC::Manager::instance().theShortCutPOA()->servant_to_id(this);
        ::RTC::Manager::instance().theShortCutPOA()->deactivate_object(oid);
#else
        oid = _default_POA()->servant_to_id(this);
        _default_POA()->deactivate_object(oid);
#endif
      }
    catch (PortableServer::POA::ServantNotActive &e)
      {
#ifdef ORB_IS_ORBEXPRESS
        oe_out << e << oe_endl << oe_flush;
#else
        RTC_ERROR(("%s", e._name()));
#endif
      }
    catch (PortableServer::POA::WrongPolicy &e)
      {
#ifdef ORB_IS_ORBEXPRESS
        oe_out << e << oe_endl << oe_flush;
#else
        RTC_ERROR(("%s", e._name()));
#endif
      }
    catch (...)
      {
        // never throws exception
        RTC_ERROR(("Unknown exception caught."));
      }
  }

  void InPortCorbaCdrProvider::init(coil::Properties& /*prop*/)
  {
  }

  /*!
   * @if jp
   * @brief バッファをセットする
   * @else
   * @brief Setting outside buffer's pointer
   * @endif
   */
  void InPortCorbaCdrProvider::
  setBuffer(BufferBase<ByteData>* buffer)
  {
    m_buffer = buffer;
  }

  /*!
   * @if jp
   * @brief リスナを設定する
   * @else
   * @brief Set the listener
   * @endif
   */
  void InPortCorbaCdrProvider::setListener(ConnectorInfo& info,
                                           ConnectorListenersBase* listeners)
  {
    m_profile = info;
    m_listeners = listeners;
  }

  /*!
   * @if jp
   * @brief Connectorを設定する。
   * @else
   * @brief set Connector
   * @endif
   */
  void InPortCorbaCdrProvider::setConnector(InPortConnector* connector)
  {
    m_connector = connector;
  }

  /*!
   * @if jp
   * @brief バッファにデータを書き込む
   * @else
   * @brief Write data into the buffer
   * @endif
   */
  ::OpenRTM::PortStatus
  InPortCorbaCdrProvider::put(const ::OpenRTM::CdrData& data)
  {
    RTC_PARANOID(("InPortCorbaCdrProvider::put()"));

    if (m_connector == nullptr)
      {
#ifndef ORB_IS_RTORB
        m_cdr.writeData(const_cast<unsigned char*>(data.get_buffer()), static_cast<CORBA::ULong>(data.length()));
#else
        m_cdr.writeData(reinterpret_cast<unsigned char*>(&data[0]), static_cast<CORBA::ULong>(data.length()));
#endif

        onReceiverError(m_cdr);
        return ::OpenRTM::PORT_ERROR;
      }

    RTC_PARANOID(("received data size: %d", data.length()));
    
    // set endian type
    bool endian_type = m_connector->isLittleEndian();
    RTC_TRACE(("connector endian: %s", endian_type ? "little":"big"));

    m_cdr.isLittleEndian(endian_type);
#ifndef ORB_IS_RTORB
    m_cdr.writeData(const_cast<unsigned char*>(data.get_buffer()), static_cast<CORBA::ULong>(data.length()));
#else
    m_cdr.writeData(reinterpret_cast<unsigned char*>(&data[0]), static_cast<CORBA::ULong>(data.length()));
#endif
    RTC_PARANOID(("converted CDR data size: %d", m_cdr.getDataLength()));

    onReceived(m_cdr);
    BufferStatus ret = m_connector->write(m_cdr);

    return convertReturn(ret, m_cdr);
  }

  /*!
   * @if jp
   * @brief リターンコード変換
   * @else
   * @brief Return codes conversion
   * @endif
   */
  ::OpenRTM::PortStatus
  InPortCorbaCdrProvider::convertReturn(BufferStatus status,
                                        ByteData& data)
  {
    switch (status)
      {
      case BufferStatus::OK:
        onBufferWrite(data);
        return ::OpenRTM::PORT_OK;

      case BufferStatus::BUFFER_ERROR:
        onReceiverError(data);
        return ::OpenRTM::PORT_ERROR;

      case BufferStatus::FULL:
        onBufferFull(data);
        onReceiverFull(data);
        return ::OpenRTM::BUFFER_FULL;

      case BufferStatus::EMPTY:
        // never come here
        return ::OpenRTM::BUFFER_EMPTY;

      case BufferStatus::PRECONDITION_NOT_MET:
        onReceiverError(data);
        return ::OpenRTM::PORT_ERROR;

      case BufferStatus::TIMEOUT:
        onBufferWriteTimeout(data);
        onReceiverTimeout(data);
        return ::OpenRTM::BUFFER_TIMEOUT;

      case BufferStatus::NOT_SUPPORTED: /* FALLTHROUGH */
      default:
        return ::OpenRTM::UNKNOWN_ERROR;
      }

  }

} // namespace RTC


extern "C"
{
  /*!
   * @if jp
   * @brief モジュール初期化関数
   * @else
   * @brief Module initialization
   * @endif
   */
  void InPortCorbaCdrProviderInit(void)
  {
    RTC::InPortProviderFactory& factory(RTC::InPortProviderFactory::instance());
    factory.addFactory("corba_cdr",
                       ::coil::Creator< ::RTC::InPortProvider,
                                        ::RTC::InPortCorbaCdrProvider>,
                       ::coil::Destructor< ::RTC::InPortProvider,
                                           ::RTC::InPortCorbaCdrProvider>);
  }
}
