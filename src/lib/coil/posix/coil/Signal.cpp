// -*- C++ -*-
/*!
 * @file  Signal_posix.cpp
 * @brief SignalAction class
 * @date  $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2008
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

#include <coil/config_coil.h>
#include <coil/Signal.h>
#include <cstring>

namespace coil
{
  /*!
   * @if jp
   * @brief ���󥹥ȥ饯��
   * @else
   * @brief Constructor
   * @endif
   */
  SignalAction::SignalAction()
  {
  }

  /*!
   * @if jp
   * @brief ���󥹥ȥ饯��
   * @param handle �����ʥ�ϥ�ɥ�
   * @param signum �����ʥ��ֹ�
   * @else
   * @brief Constructor
   * @param handle Signal handler.
   * @param signum Signal number.
   * @endif
   */
  SignalAction::SignalAction(SignalHandler handle, int signum)
    : m_handle(handle), m_signum(signum)
  {
    struct sigaction action;
    memset(&action, 0, sizeof(action));  // clear.
    action.sa_handler = m_handle;

    signal(m_signum, SIG_IGN);
    if (sigaction(m_signum, &action, nullptr) < 0)
      {
        signal(m_signum, SIG_DFL);
        m_handle = nullptr;
        m_signum = 0;
      }
  }

  /*!
   * @if jp
   * @brief �ǥ��ȥ饯��
   * @else
   * @brief Destructor
   * @endif
   */
  SignalAction::~SignalAction()
  {
    signal(m_signum, SIG_DFL);
    m_handle = nullptr;
    m_signum = 0;
  }

} // namespace coil
