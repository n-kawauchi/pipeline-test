// -*- C++ -*-
/*!
 * @file  OutPortDSProvider.h
 * @brief OutPortDSProvider class
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

#ifndef RTC_OUTPORTDSPROVIDER_H
#define RTC_OUTPORTDSPROVIDER_H

#include <rtm/idl/DataPortSkel.h>
#include <rtm/BufferBase.h>
#include <rtm/OutPortProvider.h>
#include <rtm/CORBA_SeqUtil.h>
#include <rtm/Manager.h>
#include <rtm/ConnectorListener.h>
#include <rtm/ConnectorBase.h>

namespace RTC
{
  /*!
   * @if jp
   * @class OutPortDSProvider
   * @brief OutPortDSProvider クラス
   *
   * OutPortProvider
   *
   * データ転送に CORBA の RTC::DataPullService インターフェースを利用し
   * た、pull 型データフロー型を実現する OutPort プロバイダクラス。
   *
   * @since 2.0.0
   *
   * @else
   * @class OutPortDSProvider
   * @brief OutPortDSProvider class
   *
   * The OutPort provider class which uses the RTC::DataPullService
   * interface in CORBA for data transfer and realizes a pull-type
   * dataflow.
   *
   * @since 2.0.0
   *
   * @endif
   */
  class OutPortDSProvider
    : public OutPortProvider,
      public virtual ::POA_RTC::DataPullService,
      public virtual PortableServer::RefCountServantBase
  {
  public:
    /*!
     * @if jp
     * @brief コンストラクタ
     *
     * コンストラクタ
     *
     * @else
     * @brief Constructor
     *
     * Constructor
     *
     * @endif
     */
    OutPortDSProvider();

    /*!
     * @if jp
     * @brief デストラクタ
     *
     * デストラクタ
     *
     * @else
     * @brief Destructor
     *
     * Destructor
     *
     * @endif
     */
    ~OutPortDSProvider() override;

    /*!
     * @if jp
     * @brief 設定初期化
     *
     * OutPortDSProvider の各種設定を行う。与えられた
     * Propertiesから必要な情報を取得して各種設定を行う。この init() 関
     * 数は、OutPortProvider生成直後および、接続時にそれぞれ呼ばれる可
     * 能性がある。したがって、この関数は複数回呼ばれることを想定して記
     * 述されるべきである。
     *
     * @param prop 設定情報
     *
     * @else
     *
     * @brief Initializing configuration
     *
     * This operation would be called to configure in initialization.
     * In the concrete class, configuration should be performed
     * getting appropriate information from the given Properties data.
     * This function might be called right after instantiation and
     * connection sequence respectivly.  Therefore, this function
     * should be implemented assuming multiple call.
     *
     * @param prop Configuration information
     *
     * @endif
     */
    void init(coil::Properties& prop) override;

    /*!
     * @if jp
     * @brief バッファをセットする
     *
     * OutPortProvider がデータを取り出すバッファをセットする。
     * すでにセットされたバッファがある場合、以前のバッファへの
     * ポインタに対して上書きされる。
     * OutPortProviderはバッファの所有権を仮定していないので、
     * バッファの削除はユーザの責任で行わなければならない。
     *
     * @param buffer OutPortProviderがデータを取り出すバッファへのポインタ
     *
     * @else
     * @brief Setting outside buffer's pointer
     *
     * A pointer to a buffer from which OutPortProvider retrieve data.
     * If already buffer is set, previous buffer's pointer will be
     * overwritten by the given pointer to a buffer.  Since
     * OutPortProvider does not assume ownership of the buffer
     * pointer, destructor of the buffer should be done by user.
     *
     * @param buffer A pointer to a data buffer to be used by OutPortProvider
     *
     * @endif
     */
    void setBuffer(CdrBufferBase* buffer) override;

    /*!
     * @if jp
     * @brief リスナを設定する。
     *
     * OutPort はデータ送信処理における各種イベントに対して特定のリスナ
     * オブジェクトをコールするコールバック機構を提供する。詳細は
     * ConnectorListener.h の ConnectorDataListener, ConnectorListener
     * 等を参照のこと。OutPortDSProvider では、以下のコールバック
     * が提供される。
     *
     * - ON_BUFFER_READ
     * - ON_SEND
     * - ON_BUFFER_EMPTY
     * - ON_BUFFER_READ_TIMEOUT
     * - ON_SENDER_EMPTY
     * - ON_SENDER_TIMEOUT
     * - ON_SENDER_ERROR
     *
     * @param info 接続情報
     * @param listeners リスナオブジェクト
     *
     * @else
     * @brief Set the listener.
     *
     * OutPort provides callback functionality that calls specific
     * listener objects according to the events in the data publishing
     * process. For details, see documentation of
     * ConnectorDataListener class and ConnectorListener class in
     * ConnectorListener.h. In this OutPortDSProvider provides
     * the following callbacks.
     *
     * - ON_BUFFER_READ
     * - ON_SEND
     * - ON_BUFFER_EMPTY
     * - ON_BUFFER_READ_TIMEOUT
     * - ON_SENDER_EMPTY
     * - ON_SENDER_TIMEOUT
     * - ON_SENDER_ERROR
     *
     * @param info Connector information
     * @param listeners Listener objects
     *
     * @endif
     */
    void setListener(ConnectorInfo& info,
                             ConnectorListenersBase* listeners) override;

    /*!
     * @if jp
     * @brief Connectorを設定する。
     *
     * OutPort は接続確立時に OutPortConnector オブジェクトを生成し、生
     * 成したオブジェクトのポインタと共にこの関数を呼び出す。所有権は
     * OutPort が保持するので OutPortProvider は OutPortConnector を削
     * 除してはいけない。
     *
     * @param connector OutPortConnector
     *
     * @else
     * @brief set Connector
     *
     * OutPort creates OutPortConnector object when it establishes
     * connection between OutPort and InPort, and it calls this
     * function with a pointer to the connector object. Since the
     * OutPort has the ownership of this connector, OutPortProvider
     * should not delete it.
     *
     * @param connector OutPortConnector
     *
     * @endif
     */
    void setConnector(OutPortConnector* connector) override;

    /*!
     * @if jp
     * @brief [CORBA interface] バッファからデータを取得する
     *
     * 設定された内部バッファからデータを取得する。
     *
     * @return 取得データ
     *
     * @else
     * @brief [CORBA interface] Get data from the buffer
     *
     * Get data from the internal buffer.
     *
     * @return Data got from the buffer.
     *
     * @endif
     */
    ::RTC::PortStatus pull(::RTC::OctetSeq_out data) override;


  private:
    /*!
     * @if jp
     * @brief リターンコード変換
     * @else
     * @brief Return codes conversion
     * @endif
     */
    ::RTC::PortStatus convertReturn(BufferStatus status,
                                        ByteData& data);


    /*!
     * @if jp
     * @brief ON_BUFFER_READ のリスナへ通知する。
     * @param data cdrMemoryStream
     * @else
     * @brief Notify an ON_BUFFER_READ event to listeners
     * @param data cdrMemoryStream
     * @endif
     */
    inline void onBufferRead(ByteData& data)
    {
      m_listeners->notifyOut(ConnectorDataListenerType::ON_BUFFER_READ, m_profile, data);
    }

    /*!
     * @if jp
     * @brief ON_SEND のリスナへ通知する。
     * @param data cdrMemoryStream
     * @else
     * @brief Notify an ON_SEND event to listeners
     * @param data cdrMemoryStream
     * @endif
     */
    inline void onSend(ByteData& data)
    {
      m_listeners->notifyOut(ConnectorDataListenerType::ON_SEND, m_profile, data);
    }

    /*!
     * @if jp
     * @brief ON_BUFFER_EMPTYのリスナへ通知する。
     * @else
     * @brief Notify an ON_BUFFER_EMPTY event to listeners
     * @endif
     */
    inline void onBufferEmpty()
    {
      m_listeners->notify(ConnectorListenerType::ON_BUFFER_EMPTY, m_profile);
    }

    /*!
     * @if jp
     * @brief ON_BUFFER_READ_TIMEOUT のリスナへ通知する。
     * @else
     * @brief Notify an ON_BUFFER_READ_TIMEOUT event to listeners
     * @endif
     */
    inline void onBufferReadTimeout()
    {
      m_listeners->notify(ConnectorListenerType::ON_BUFFER_READ_TIMEOUT, m_profile);
    }

    /*!
     * @if jp
     * @brief ON_SENDER_EMPTYのリスナへ通知する。
     * @else
     * @brief Notify an ON_SENDER_EMPTY event to listeners
     * @endif
     */
    inline void onSenderEmpty()
    {
      m_listeners->notify(ConnectorListenerType::ON_SENDER_EMPTY, m_profile);
    }

    /*!
     * @if jp
     * @brief ON_SENDER_TIMEOUT のリスナへ通知する。
     * @else
     * @brief Notify an ON_SENDER_TIMEOUT event to listeners
     * @endif
     */
    inline void onSenderTimeout()
    {
      m_listeners->notify(ConnectorListenerType::ON_SENDER_TIMEOUT, m_profile);
    }

    /*!
     * @if jp
     * @brief ON_SENDER_ERRORのリスナへ通知する。
     * @else
     * @brief Notify an ON_SENDER_ERROR event to listeners
     * @endif
     */
    inline void onSenderError()
    {
      m_listeners->notify(ConnectorListenerType::ON_SENDER_ERROR, m_profile);
    }

  private:
    CdrBufferBase* m_buffer{nullptr};
    ::RTC::DataPullService_var m_objref;
    ConnectorListenersBase* m_listeners;
    ConnectorInfo m_profile;
    OutPortConnector* m_connector{nullptr};
    ByteData m_cdr;
  };  // class OutPortDSProvider
} // namespace RTC

extern "C"
{
  /*!
   * @if jp
   * @brief モジュール初期化関数
   *
   * OutPortDSProvider のファクトリを登録する初期化関数。
   *
   * @else
   * @brief Module initialization
   *
   * This initialization function registers OutPortDSProvider's factory.
   *
   * @endif
   */
  void OutPortDSProviderInit(void);
}

#endif  // RTC_OUTPORTDSPROVIDER_H
