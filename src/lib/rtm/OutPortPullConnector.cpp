﻿// -*- C++ -*-
/*!
 * @file OutPortPullConnector.cpp
 * @brief OutPortPull type connector class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2009
 *     Noriaki Ando
 *     Task-intelligence Research Group,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#include <rtm/OutPortPullConnector.h>
#include <rtm/OutPortProvider.h>

#include <string>

namespace RTC
{
  /*!
   * @if jp
   * @brief コンストラクタ
   * @elsek
   * @brief Constructor
   * @endif
   */
  OutPortPullConnector::OutPortPullConnector(ConnectorInfo info,
                                             OutPortProvider* provider,
                                             ConnectorListenersBase* listeners,
                                             CdrBufferBase* buffer)
    : OutPortConnector(info, listeners),
      m_provider(provider),
      m_listeners(listeners),
      m_buffer(buffer),
      m_sync_readwrite(false)
  {
    // create buffer
    if (m_buffer == nullptr)
      {
        m_buffer = createBuffer(info);
      }

    if (m_provider == nullptr || m_buffer == nullptr) { throw std::bad_alloc(); }

    m_buffer->init(info.properties.getNode("buffer"));
    m_provider->init(info.properties);
    m_provider->setBuffer(m_buffer);
    m_provider->setConnector(this);
    m_provider->setListener(info, m_listeners);

    if (coil::toBool(info.properties["sync_readwrite"], "YES", "NO", false))
    {
        m_sync_readwrite = true;
    }

    m_marshaling_type = coil::eraseBothEndsBlank(
      info.properties.getProperty("marshaling_type", "cdr"));

    onConnect();
  }

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  OutPortPullConnector::~OutPortPullConnector()
  {
    onDisconnect();
    disconnect();
  }

  /*!
   * @if jp
   * @brief read 関数
   * @else
   * @brief Destructor
   * @endif
   */
  DataPortStatus
  OutPortPullConnector::write(ByteDataStreamBase* data)
  {

    if (m_buffer == nullptr)
    {
        return DataPortStatus::PRECONDITION_NOT_MET;
    }

    if (m_sync_readwrite)
    {
        {
            std::unique_lock<std::mutex> guard(m_readready_worker.mutex_);
            while (!m_readready_worker.completed_)
            {
                m_readready_worker.cond_.wait(guard);
            }
        }
    }

    m_buffer->write(*data);

    if (m_sync_readwrite)
    {
        {
            std::lock_guard<std::mutex> guard(m_writecompleted_worker.mutex_);
            m_writecompleted_worker.completed_ = true;
            m_writecompleted_worker.cond_.notify_one();
        }


          {
              std::unique_lock<std::mutex> guard(m_readcompleted_worker.mutex_);
              while (!m_readcompleted_worker.completed_)
              {
                  m_readcompleted_worker.cond_.wait(guard);
              }
          }
          {
              std::lock_guard<std::mutex> guard(m_writecompleted_worker.mutex_);
              m_writecompleted_worker.completed_ = false;
          }
    }

    return DataPortStatus::PORT_OK;
  }

  BufferStatus
  OutPortPullConnector::read(ByteData &data)
  {
      if (m_buffer == nullptr)
      {
          return BufferStatus::PRECONDITION_NOT_MET;
      }

      if (m_sync_readwrite)
      {

        {
            std::lock_guard<std::mutex> guard(m_readcompleted_worker.mutex_);
            m_readcompleted_worker.completed_ = false;
        }

        {
            std::lock_guard<std::mutex> guard(m_readready_worker.mutex_);
            m_readready_worker.completed_ = true;
            m_readready_worker.cond_.notify_one();
        }
        {
            std::unique_lock<std::mutex> guard(m_writecompleted_worker.mutex_);
            while (!m_writecompleted_worker.completed_)
            {
                m_writecompleted_worker.cond_.wait(guard);
            }
        }
      }

      BufferStatus ret = m_buffer->read(data);

      if (m_sync_readwrite)
      {
        {
            std::lock_guard<std::mutex> guard(m_readcompleted_worker.mutex_);
            m_readcompleted_worker.completed_ = true;
            m_readcompleted_worker.cond_.notify_one();
        }

        {
            std::lock_guard<std::mutex> guard(m_readready_worker.mutex_);
            m_readready_worker.completed_ = false;
        }
      }


      return ret;
  }

  /*!
   * @if jp
   * @brief 接続解除関数
   * @else
   * @brief Disconnect connection
   * @endif
   */
  DataPortStatus OutPortPullConnector::disconnect()
  {
    RTC_TRACE(("disconnect()"));
    // delete provider
    if (m_provider != nullptr)
      {
        OutPortProviderFactory& cfactory(OutPortProviderFactory::instance());
        cfactory.deleteObject(m_provider);
      }
    m_provider = nullptr;

    // delete buffer
    if (m_buffer != nullptr)
      {
        CdrBufferFactory& bfactory(CdrBufferFactory::instance());
        bfactory.deleteObject(m_buffer);
      }
    m_buffer = nullptr;

    return DataPortStatus::PORT_OK;
  }

  /*!
   * @if jp
   * @brief Buffer を取得する
   *
   * Connector が保持している Buffer を返す
   *
   * @else
   * @brief Getting Buffer
   *
   * This operation returns this connector's buffer
   *
   * @endif
   */
  CdrBufferBase* OutPortPullConnector::getBuffer()
  {
    return m_buffer;
  }

  /*!
   * @if jp
   * @brief Bufferの生成
   * @else
   * @brief create buffer
   * @endif
   */
  CdrBufferBase* OutPortPullConnector::createBuffer(ConnectorInfo& info)
  {
    std::string buf_type;
    buf_type = info.properties.getProperty("buffer_type",
                                              "ring_buffer");
    return CdrBufferFactory::instance().createObject(buf_type);
  }

  /*!
   * @if jp
   * @brief 接続確立時にコールバックを呼ぶ
   * @else
   * @brief Invoke callback when connection is established
   * @endif
   */
  void OutPortPullConnector::onConnect()
  {
    m_listeners->notify(ConnectorListenerType::ON_CONNECT, m_profile);
  }

  /*!
   * @if jp
   * @brief 接続切断時にコールバックを呼ぶ
   * @else
   * @brief Invoke callback when connection is destroied
   * @endif
   */
  void OutPortPullConnector::onDisconnect()
  {
    m_listeners->notify(ConnectorListenerType::ON_DISCONNECT, m_profile);
  }
} // namespace RTC

