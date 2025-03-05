﻿// -*- C++ -*-
/*!
 * @file  PublisherNew.cpp
 * @brief PublisherNew class
 * @date  $Date: 2007-12-31 03:08:06 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2006-2008
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

#include <coil/Properties.h>
#include <coil/stringutil.h>

#include <rtm/RTC.h>
#include <rtm/PublisherNew.h>
#include <rtm/InPortConsumer.h>
#include <rtm/PeriodicTaskFactory.h>
#include <rtm/idl/DataPortSkel.h>
#include <rtm/ConnectorListener.h>

#include <cassert>
#include <iostream>
#include <string>

namespace RTC
{
  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  PublisherNew::PublisherNew()
  {
  }

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  PublisherNew::~PublisherNew()
  {
    RTC_TRACE(("~PublisherNew()"));
    if (m_task != nullptr)
      {
        m_task->resume();
        m_task->finalize();

        RTC::PeriodicTaskFactory::instance().deleteObject(m_task);
        RTC_PARANOID(("task deleted."));
      }

    // "consumer" should be deleted in the Connector
    m_consumer = nullptr;
    // "buffer"   should be deleted in the Connector
    m_buffer = nullptr;
  }

  /*!
   * @if jp
   * @brief 初期化
   * @else
   * @brief initialization
   * @endif
   */
  DataPortStatus PublisherNew::init(coil::Properties& prop)
  {
    RTC_TRACE(("init()"));
    RTC_DEBUG_STR((prop));

    setPushPolicy(prop);
    if (!createTask(prop))
      {
        return DataPortStatus::INVALID_ARGS;
      }
    return DataPortStatus::PORT_OK;
  }

  /*!
   * @if jp
   * @brief InPortコンシューマのセット
   * @else
   * @brief Store InPort consumer
   * @endif
   */
  DataPortStatus PublisherNew::setConsumer(InPortConsumer* consumer)
  {
    RTC_TRACE(("setConsumer()"));

    if (consumer == nullptr)
      {
        RTC_ERROR(("setConsumer(consumer = 0): invalid argument."));
        return DataPortStatus::INVALID_ARGS;
      }
    m_consumer = consumer;
    return DataPortStatus::PORT_OK;
  }

  /*!
   * @if jp
   * @brief バッファのセット
   * @else
   * @brief Setting buffer pointer
   * @endif
   */
  DataPortStatus PublisherNew::setBuffer(CdrBufferBase* buffer)
  {
    RTC_TRACE(("setBuffer()"));

    if (buffer == nullptr)
      {
        RTC_ERROR(("setBuffer(buffer == 0): invalid argument"));
        return DataPortStatus::INVALID_ARGS;
      }
    m_buffer = buffer;
    return DataPortStatus::PORT_OK;
  }

  /*!
   * @if jp
   * @brief リスナのセット
   * @else
   * @brief Setting buffer pointer
   * @endif
   */
  DataPortStatus
  PublisherNew::setListener(ConnectorInfo& info,
                            ConnectorListenersBase* listeners)
  {
    RTC_TRACE(("setListeners()"));

    if (listeners == nullptr)
      {
        RTC_ERROR(("setListeners(listeners == 0): invalid argument"));
        return DataPortStatus::INVALID_ARGS;
      }
    m_profile = info;
    m_listeners = listeners;
    return DataPortStatus::PORT_OK;
  }

  /*!
   * @if jp
   * @brief データを書き込む
   * @else
   * @brief Write data
   * @endif
   */
  DataPortStatus PublisherNew::write(ByteDataStreamBase* data,
                                                std::chrono::nanoseconds timeout)
  {
    RTC_PARANOID(("write()"));

    if (m_consumer == nullptr) { return DataPortStatus::PRECONDITION_NOT_MET; }
    if (m_buffer == nullptr) { return DataPortStatus::PRECONDITION_NOT_MET; }
    if (m_listeners == nullptr) { return DataPortStatus::PRECONDITION_NOT_MET; }

    if (m_retcode == DataPortStatus::CONNECTION_LOST)
      {
        RTC_DEBUG(("write(): connection lost."));
        return m_retcode;
      }

    m_data = *data;

    if (m_retcode == DataPortStatus::SEND_FULL)
      {
        RTC_DEBUG(("write(): InPort buffer is full."));
        m_buffer->write(m_data, timeout);
        m_task->signal();
        return DataPortStatus::BUFFER_FULL;
      }

    assert(m_buffer != nullptr);

    onBufferWrite(m_data);
    BufferStatus ret(m_buffer->write(m_data, timeout));

    m_task->signal();
    RTC_DEBUG(("%s = write()", toString(ret)));

    return convertReturn(ret, m_data);
  }

  /*!
   * @if jp
   * @brief アクティブ化確認
   * @else
   * @brief Confirm to activate
   * @endif
   */
  bool PublisherNew::isActive()
  {
    return m_active;
  }

  /*!
   * @if jp
   * @brief アクティブ化
   * @else
   * @brief activation
   * @endif
   */
  DataPortStatus PublisherNew::activate()
  {
    m_active = true;
    return DataPortStatus::PORT_OK;
  }

  /*!
   * @if jp
   * @brief 非アクティブ化
   * @else
   * @brief deactivation
   * @endif
   */
  DataPortStatus PublisherNew::deactivate()
  {
    m_active = false;
    return DataPortStatus::PORT_OK;
  }

  /*!
   * @if jp
   * @brief PublisherNew::スレッド実行関数
   * @else
   * @brief Thread execution function
   * @endif
   */
  int PublisherNew::svc()
  {

    std::lock_guard<std::mutex> guard(m_retmutex);
    switch (m_pushPolicy)
      {
      case PUBLISHER_POLICY_ALL:
        m_retcode = pushAll();
        break;
      case PUBLISHER_POLICY_FIFO:
        m_retcode = pushFifo();
        break;
      case PUBLISHER_POLICY_SKIP:
        m_retcode = pushSkip();
        break;
      case PUBLISHER_POLICY_NEW:
        m_retcode = pushNew();
        break;
      default:
        m_retcode = pushNew();
        break;
      }
    return 0;
  }

  /*!
   * @if jp
   * @brief PushPolicy の設定
   * @else
   * @brief Setting PushPolicy
   * @endif
   */
  void PublisherNew::setPushPolicy(const coil::Properties& prop)
  {
    // push_policy default: NEW
    std::string push_policy{coil::normalize(
      prop.getProperty("publisher.push_policy", "new"))};
    RTC_DEBUG(("push_policy: %s", push_policy.c_str()));

    if      (push_policy == "all")  { m_pushPolicy = PUBLISHER_POLICY_ALL;  }
    else if (push_policy == "fifo") { m_pushPolicy = PUBLISHER_POLICY_FIFO; }
    else if (push_policy == "skip") { m_pushPolicy = PUBLISHER_POLICY_SKIP; }
    else if (push_policy == "new")  { m_pushPolicy = PUBLISHER_POLICY_NEW;  }
    else
      {
        RTC_ERROR(("invalid push_policy value: %s", push_policy.c_str()));
        m_pushPolicy = PUBLISHER_POLICY_NEW;     // default push policy
      }

    // skip_count default: 0
    std::string skip_count = prop.getProperty("publisher.skip_count", "0");
    RTC_DEBUG(("skip_count: %s", skip_count.c_str()));

    if (!coil::stringTo(m_skipn, skip_count.c_str()))
      {
        RTC_ERROR(("invalid skip_count value: %s", skip_count.c_str()));
        m_skipn = 0;           // default skip count
      }
    if (m_skipn < 0)
      {
        RTC_ERROR(("invalid skip_count value: %d", m_skipn));
        m_skipn = 0;           // default skip count
      }
  }

  /*!
   * @if jp
   * @brief Task の設定
   * @else
   * @brief Setting Task
   * @endif
   */
  bool PublisherNew::createTask(const coil::Properties& prop)
  {
    RTC::PeriodicTaskFactory& factory(RTC::PeriodicTaskFactory::instance());
    coil::vstring th = factory.getIdentifiers();
    RTC_DEBUG(("available task types: %s", coil::flatten(th).c_str()));

    m_task = factory.createObject(prop.getProperty("thread_type", "default"));
    if (m_task == nullptr)
      {
        RTC_ERROR(("Task creation failed: %s",
                   prop.getProperty("thread_type", "default").c_str()));
        return false;
      }
    RTC_PARANOID(("Task creation succeeded."));

    // setting task function
    m_task->setTask([this]{ svc(); });
    m_task->setPeriod(std::chrono::seconds(0));
    m_task->executionMeasure(coil::toBool(prop["measurement.exec_time"],
                                    "enable", "disable", true));

    int ecount(1000);
    if (coil::stringTo(ecount, prop["measurement.exec_count"].c_str()))
      {
        m_task->executionMeasureCount(ecount);
      }

    m_task->periodicMeasure(coil::toBool(prop["measurement.period_time"],
                                   "enable", "disable", true));
    int pcount(1000);
    if (coil::stringTo(pcount, prop["measurement.period_count"].c_str()))
      {
        m_task->periodicMeasureCount(pcount);
      }

    // Start task in suspended mode
    m_task->suspend();
    m_task->activate();
    m_task->suspend();

    return true;
  }


  /*!
   * @brief push all policy
   */
  DataPortStatus PublisherNew::pushAll()
  {
    RTC_TRACE(("pushAll()"));

    while (m_buffer->readable() > 0)
      {
        ByteData& cdr(m_buffer->get());
        onBufferRead(cdr);

        onSend(cdr);
        DataPortStatus ret(m_consumer->put(cdr));
        if (ret != DataPortStatus::PORT_OK)
          {
            RTC_DEBUG(("%s = consumer.put()", toString(ret)));
            return invokeListener(ret, cdr);
          }
        onReceived(cdr);

        m_buffer->advanceRptr();
      }
    return DataPortStatus::PORT_OK;
  }

  /*!
   * @brief push "fifo" policy
   */
  DataPortStatus PublisherNew::pushFifo()
  {
    RTC_TRACE(("pushFifo()"));

    ByteData& cdr(m_buffer->get());

    onBufferRead(cdr);

    onSend(cdr);
    DataPortStatus ret(m_consumer->put(cdr));
    if (ret != DataPortStatus::PORT_OK)
      {
        RTC_DEBUG(("%s = consumer.put()", toString(ret)));
        return invokeListener(ret, cdr);
      }
    onReceived(cdr);

    m_buffer->advanceRptr();

    return DataPortStatus::PORT_OK;
  }

  /*!
   * @brief push "skip" policy
   */
  DataPortStatus PublisherNew::pushSkip()
  {
    RTC_TRACE(("pushSkip()"));

    DataPortStatus ret(DataPortStatus::PORT_OK);
    int preskip(static_cast<int>(m_buffer->readable()) + m_leftskip);
    int loopcnt(preskip/(m_skipn +1));
    int postskip(m_skipn - m_leftskip);
    for (int i(0); i < loopcnt; ++i)
      {
        m_buffer->advanceRptr(postskip);

        ByteData& cdr(m_buffer->get());

        onBufferRead(cdr);

        onSend(cdr);
        ret = m_consumer->put(cdr);
        if (ret != DataPortStatus::PORT_OK)
          {
            m_buffer->advanceRptr(-postskip);
            RTC_DEBUG(("%s = consumer.put()", toString(ret)));
            return invokeListener(ret, cdr);
          }
        onReceived(cdr);
        postskip = m_skipn + 1;
      }
    m_buffer->advanceRptr(static_cast<long>(m_buffer->readable()));
    if (loopcnt == 0)
      {  // Not put
        m_leftskip = preskip % (m_skipn +1);
      }
    else
      {
        if ( m_retcode != DataPortStatus::PORT_OK )
          {  // put Error after
            m_leftskip = 0;
          }
        else
          {  // put OK after
            m_leftskip = preskip % (m_skipn +1);
          }
      }
    return ret;
  }

   /*!
    * @brief push "new" policy
    */
  DataPortStatus PublisherNew::pushNew()
  {
    RTC_TRACE(("pushNew()"));

    m_buffer->advanceRptr(static_cast<long>(m_buffer->readable()) - 1);

    ByteData& cdr(m_buffer->get());
    onBufferRead(cdr);

    onSend(cdr);
    DataPortStatus ret(m_consumer->put(cdr));
    if (ret != DataPortStatus::PORT_OK)
      {
        RTC_DEBUG(("%s = consumer.put()", toString(ret)));
        return invokeListener(ret, cdr);
      }
    onReceived(cdr);

    m_buffer->advanceRptr();

    return DataPortStatus::PORT_OK;
  }

  /*!
   * @if jp
   * @brief BufferStatus から DataPortStatus への変換
   * @else
   * @brief Convertion from BufferStatus to DataPortStatus
   * @endif
   */
  DataPortStatus
  PublisherNew::convertReturn(BufferStatus status,
                              ByteData& data)
  {
    /*
     * BufferStatus -> DataPortStatus
     *
     * BUFFER_OK     -> PORT_OK
     * BUFFER_ERROR  -> BUFFER_ERROR
     * BUFFER_FULL   -> BUFFER_FULL
     * NOT_SUPPORTED -> PORT_ERROR
     * TIMEOUT       -> BUFFER_TIMEOUT
     * PRECONDITION_NOT_MET -> PRECONDITION_NOT_MET
     */
    switch (status)
      {
      case BufferStatus::OK:
        // no callback
        return DataPortStatus::PORT_OK;
      case BufferStatus::BUFFER_ERROR:
        // no callback
        return DataPortStatus::BUFFER_ERROR;
      case BufferStatus::FULL:
        onBufferFull(data);
        return DataPortStatus::BUFFER_FULL;
      case BufferStatus::NOT_SUPPORTED:
        // no callback
        return DataPortStatus::PORT_ERROR;
      case BufferStatus::TIMEOUT:
        onBufferWriteTimeout(data);
        return DataPortStatus::BUFFER_TIMEOUT;
      case BufferStatus::PRECONDITION_NOT_MET:
        // no callback
        return DataPortStatus::PRECONDITION_NOT_MET;
      case BufferStatus::EMPTY: /* FALLTHROUGH */
      default:
        // no callback
        return DataPortStatus::PORT_ERROR;
      }
  }

  /*!
   * @if jp
   * @brief DataPortStatusに従ってリスナへ通知する関数を呼び出す。
   * @else
   * @brief Call listeners according to the DataPortStatus
   * @endif
   */
  DataPortStatus
  PublisherNew::invokeListener(DataPortStatus status,
                               ByteData& data)
  {
    // ret:
    // PORT_ERROR, SEND_FULL, SEND_TIMEOUT, CONNECTION_LOST, UNKNOWN_ERROR
    //
    // PORT_OK never come here
    switch (status)
      {
      case DataPortStatus::PORT_ERROR:
        onReceiverError(data);
        return DataPortStatus::PORT_ERROR;

      case DataPortStatus::SEND_FULL:
        onReceiverFull(data);
        return DataPortStatus::SEND_FULL;

      case DataPortStatus::SEND_TIMEOUT:
        onReceiverTimeout(data);
        return DataPortStatus::SEND_TIMEOUT;

      case DataPortStatus::CONNECTION_LOST:
        onReceiverError(data);
        return DataPortStatus::CONNECTION_LOST;

      case DataPortStatus::UNKNOWN_ERROR:
        onReceiverError(data);
        return DataPortStatus::UNKNOWN_ERROR;

      case DataPortStatus::PORT_OK:              /* FALLTHROUGH */
      case DataPortStatus::BUFFER_ERROR:         /* FALLTHROUGH */
      case DataPortStatus::BUFFER_FULL:          /* FALLTHROUGH */
      case DataPortStatus::BUFFER_EMPTY:         /* FALLTHROUGH */
      case DataPortStatus::BUFFER_TIMEOUT:       /* FALLTHROUGH */
      case DataPortStatus::RECV_EMPTY:           /* FALLTHROUGH */
      case DataPortStatus::RECV_TIMEOUT:         /* FALLTHROUGH */
      case DataPortStatus::INVALID_ARGS:         /* FALLTHROUGH */
      case DataPortStatus::PRECONDITION_NOT_MET: /* FALLTHROUGH */
      default:
        onReceiverError(data);
        return DataPortStatus::PORT_ERROR;
      }
  }

} // namespace RTC

extern "C"
{
  void PublisherNewInit()
  {
    ::RTC::PublisherFactory::
      instance().addFactory("new",
                            ::coil::Creator< ::RTC::PublisherBase,
                                             ::RTC::PublisherNew>,
                            ::coil::Destructor< ::RTC::PublisherBase,
                                                ::RTC::PublisherNew>);
    // FSM4RTC formal/16-04-01 p.25
    // In the FSM4RTC specification, publisher type is defined as "io_mode"
    // Publisher "new" is functionally equals to "io_mode: nonblock".
    ::RTC::PublisherFactory::
      instance().addFactory("nonblock",
                            ::coil::Creator< ::RTC::PublisherBase,
                                             ::RTC::PublisherNew>,
                            ::coil::Destructor< ::RTC::PublisherBase,
                                                ::RTC::PublisherNew>);
  }
}
