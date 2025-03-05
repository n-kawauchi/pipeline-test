﻿// -*- C++ -*-
/*!
 * @file  InPortDSProvider.cpp
 * @brief InPortDSProvider class
 * @date  $Date: 2018-09-20 07:49:59 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2018
 *     Nobuhiko Miyamoto
 *     Robot Innovation Research Center,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 *
 */

#include <rtm/InPortDSProvider.h>

namespace RTC
{
  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  InPortDSProvider::InPortDSProvider()
  {
    // PortProfile setting
    setInterfaceType("data_service");

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
                NVUtil::newNV("dataport.data_service.inport_ior", ior.in()));
    CORBA_SeqUtil::
      push_back(m_properties,
                NVUtil::newNV("dataport.data_service.inport_ref", m_objref));
  }

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  InPortDSProvider::~InPortDSProvider()
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

  void InPortDSProvider::init(coil::Properties& /*prop*/)
  {
  }

  /*!
   * @if jp
   * @brief バッファをセットする
   * @else
   * @brief Setting outside buffer's pointer
   * @endif
   */
  void InPortDSProvider::
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
  void InPortDSProvider::setListener(ConnectorInfo& info,
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
  void InPortDSProvider::setConnector(InPortConnector* connector)
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
  ::RTC::PortStatus
  InPortDSProvider::push(const ::RTC::OctetSeq& data)
  {
    RTC_PARANOID(("InPortDSProvider::push()"));

    if (m_connector == nullptr)
      {
#ifndef ORB_IS_RTORB
        m_cdr.writeData(const_cast<unsigned char*>(data.get_buffer()), static_cast<CORBA::ULong>(data.length()));
#else
        m_cdr.writeData(reinterpret_cast<unsigned char*>(&data[0]), static_cast<CORBA::ULong>(data.length()));
#endif

        onReceiverError(m_cdr);
        return ::RTC::PORT_ERROR;
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
  ::RTC::PortStatus
  InPortDSProvider::convertReturn(BufferStatus status,
                                        ByteData& data)
  {
    switch (status)
      {
      case BufferStatus::OK:
        onBufferWrite(data);
        return ::RTC::PORT_OK;

      case BufferStatus::BUFFER_ERROR:
        onReceiverError(data);
        return ::RTC::PORT_ERROR;

      case BufferStatus::FULL:
        onBufferFull(data);
        onReceiverFull(data);
        return ::RTC::BUFFER_FULL;

      case BufferStatus::EMPTY:
        // never come here
        return ::RTC::BUFFER_EMPTY;

      case BufferStatus::PRECONDITION_NOT_MET:
        onReceiverError(data);
        return ::RTC::PORT_ERROR;

      case BufferStatus::TIMEOUT:
        onBufferWriteTimeout(data);
        onReceiverTimeout(data);
        return ::RTC::BUFFER_TIMEOUT;

      case BufferStatus::NOT_SUPPORTED: /* FALLTHROUGH */
      default:
        return ::RTC::UNKNOWN_ERROR;
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
  void InPortDSProviderInit(void)
  {
    RTC::InPortProviderFactory& factory(RTC::InPortProviderFactory::instance());
    factory.addFactory("data_service",
                       ::coil::Creator< ::RTC::InPortProvider,
                                        ::RTC::InPortDSProvider>,
                       ::coil::Destructor< ::RTC::InPortProvider,
                                           ::RTC::InPortDSProvider>);
  }
}
