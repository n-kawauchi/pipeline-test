﻿// -*- C++ -*-
/*!
 * @file RTObjectStateMachine.cpp
 * @brief ExecutionContext's state machine worker class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2011-2012
 *     Noriaki Ando
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#include <rtm/RTObjectStateMachine.h>
#include <rtm/Manager.h>
#include <rtm/RTObject.h>
#include <cstdio>
#include <iostream>

namespace RTC_impl
{
  RTObjectStateMachine::RTObjectStateMachine(RTC::ExecutionContextHandle_t id,
                                             RTC::LightweightRTObject_ptr comp)
    : m_id(id),
      m_rtobj(RTC::LightweightRTObject::_duplicate(comp)),
      m_sm(NUM_OF_LIFECYCLESTATE),
      m_ca(false), m_dfc(false), m_fsm(false), m_mode(false),
      m_rtobjPtr(nullptr), m_measure(false), m_activation(false),
      m_deactivation(false), m_reset(false)
  {
    m_caVar   = RTC::ComponentAction::_nil();
    m_dfcVar  = RTC::DataFlowComponentAction::_nil();
    m_fsmVar  = RTC::FsmParticipantAction::_nil();
    m_modeVar = RTC::MultiModeComponentAction::_nil();
    // Setting Action callback objects
    setComponentAction(comp);
    setDataFlowComponentAction(comp);
    setFsmParticipantAction(comp);
    setMultiModeComponentAction(comp);
    // Setting callback functions to StateMachine
    m_sm.setListener(this);
    m_sm.setEntryAction (RTC::ACTIVE_STATE,
                         &RTObjectStateMachine::onActivated);
    m_sm.setDoAction    (RTC::ACTIVE_STATE,
                         &RTObjectStateMachine::onExecute);
    m_sm.setPostDoAction(RTC::ACTIVE_STATE,
                         &RTObjectStateMachine::onStateUpdate);
    m_sm.setExitAction  (RTC::ACTIVE_STATE,
                         &RTObjectStateMachine::onDeactivated);
    m_sm.setEntryAction (RTC::ERROR_STATE,
                         &RTObjectStateMachine::onAborting);
    m_sm.setDoAction    (RTC::ERROR_STATE,
                         &RTObjectStateMachine::onError);
    m_sm.setExitAction  (RTC::ERROR_STATE,
                         &RTObjectStateMachine::onReset);
    // Setting inital state
    ExecContextStates st;
    st.prev = RTC::INACTIVE_STATE;
    st.curr = RTC::INACTIVE_STATE;
    st.next = RTC::INACTIVE_STATE;
    m_sm.setStartState(st);
    m_sm.goTo(RTC::INACTIVE_STATE);
  }

  RTObjectStateMachine::~RTObjectStateMachine()
  {
    if (m_ca)
      {
        m_ca = false;
        m_caVar = RTC::ComponentAction::_nil();
      }
    if (m_dfc)
      {
        m_dfc = false;
        m_dfcVar = RTC::DataFlowComponentAction::_nil();
      }
    if (m_fsm)
      {
        m_fsm = false;
        m_fsmVar = RTC::FsmParticipantAction::_nil();
      }
    if (m_mode)
      {
        m_mode = true;
        m_modeVar = RTC::MultiModeComponentAction::_nil();
      }
  }

  RTC::LightweightRTObject_ptr RTObjectStateMachine::getRTObject()
  {
    return RTC::LightweightRTObject::_duplicate(m_rtobj);
  }

  bool RTObjectStateMachine::isEquivalent(RTC::LightweightRTObject_ptr comp)
  {
    return m_rtobj->_is_equivalent(comp);
  }

  RTC::ExecutionContextHandle_t RTObjectStateMachine::
  getExecutionContextHandle()
  {
    return m_id;
  }

  void RTObjectStateMachine::
  setComponentAction(const RTC::LightweightRTObject_ptr comp)
  {
    m_caVar = RTC::ComponentAction::_narrow(comp);
    if (CORBA::is_nil(m_caVar)) { return; }
#ifndef ORB_IS_RTORB
    m_ca = true;
    PortableServer::POA_var poa = ::RTC::Manager::instance().getPOA();
    try
      {
        m_rtobjPtr =
          dynamic_cast<RTC::RTObject_impl*>(poa->reference_to_servant(comp));
      }
    catch (CORBA::SystemException&)
      {
        m_rtobjPtr = nullptr;
      }
    catch (...)
      {
        m_rtobjPtr = nullptr;
      }
  #endif
  }

  void RTObjectStateMachine::
  setDataFlowComponentAction(const RTC::LightweightRTObject_ptr comp)
  {
    m_dfcVar = RTC::DataFlowComponentAction::_narrow(comp);
    if (!CORBA::is_nil(m_dfcVar)) { m_dfc = true; }
  }

  void RTObjectStateMachine::
  setFsmParticipantAction(const RTC::LightweightRTObject_ptr comp)
  {
    m_fsmVar = RTC::FsmParticipantAction::_narrow(comp);
    if (!CORBA::is_nil(m_fsmVar)) { m_fsm = true; }
  }

  void RTObjectStateMachine::
  setMultiModeComponentAction(const RTC::LightweightRTObject_ptr comp)
  {
    m_modeVar = RTC::MultiModeComponentAction::_narrow(comp);
    if (!CORBA::is_nil(m_modeVar)) { m_mode = true; }
  }

  // RTC::ComponentAction operations
  void RTObjectStateMachine::onStartup()
  {
    // call Servant
    if (m_rtobjPtr != nullptr)
      {
        m_rtobjPtr->on_startup(m_id);
        return;
      }
    // call Object reference
    if (!m_ca) { return; }
    m_caVar->on_startup(m_id);
  }
  void RTObjectStateMachine::onShutdown()
  {
    // call Servant
    if (m_rtobjPtr != nullptr)
      {
        m_rtobjPtr->on_shutdown(m_id);
        return;
      }
    // call Object reference
    if (!m_ca) { return; }
    m_caVar->on_shutdown(m_id);
  }
  void RTObjectStateMachine::onActivated(const ExecContextStates&  /*st*/)
  {
    // call Servant
    if (m_rtobjPtr != nullptr)
      {
        if (m_rtobjPtr->on_activated(m_id) != RTC::RTC_OK)
          {
            m_sm.goTo(RTC::ERROR_STATE);
          }
        return;
      }
    // call Object reference
    if (!m_ca) { return; }
    if (m_caVar->on_activated(m_id) != RTC::RTC_OK)
      {
        m_sm.goTo(RTC::ERROR_STATE);
        return;
      }
    return;
  }

  void RTObjectStateMachine::onDeactivated(const ExecContextStates&  /*st*/)
  {
    // call Servant
    if (m_rtobjPtr != nullptr)
      {
        m_rtobjPtr->on_deactivated(m_id);
        return;
      }
    // call Object reference
    if (!m_ca) { return; }
    m_caVar->on_deactivated(m_id);
  }

  void RTObjectStateMachine::onAborting(const ExecContextStates&  /*st*/)
  {
    // call Servant
    if (m_rtobjPtr != nullptr)
      {
        m_rtobjPtr->on_aborting(m_id);
        return;
      }
    // call Object reference
    if (!m_ca) { return; }
    m_caVar->on_aborting(m_id);
  }

  void RTObjectStateMachine::onError(const ExecContextStates&  /*st*/)
  {
    // call Servant
    if (m_rtobjPtr != nullptr)
      {
        m_rtobjPtr->on_error(m_id);
        return;
      }
    // call Object reference
    if (!m_ca) { return; }
    m_caVar->on_error(m_id);
  }

  void RTObjectStateMachine::onReset(const ExecContextStates&  /*st*/)
  {
    // call Servant
    if (m_rtobjPtr != nullptr)
      {
        if (m_rtobjPtr->on_reset(m_id) != RTC::RTC_OK)
          {
            m_sm.goTo(RTC::ERROR_STATE);
          }
        return;
      }
    // call Object reference
    if (!m_ca) { return; }
    if (m_caVar->on_reset(m_id) != RTC::RTC_OK)
      {
        m_sm.goTo(RTC::ERROR_STATE);
        return;
      }
    return;
  }

  // RTC::DataflowComponentAction
  void RTObjectStateMachine::onExecute(const ExecContextStates&  /*st*/)
  {
    if (isNextState(RTC::ERROR_STATE)) { return; }
    static int count;
    double max_interval, min_interval, mean_interval, stddev;
    // call Servant
    if (m_rtobjPtr != nullptr)
      {
        if (m_measure) { m_svtMeasure.tick(); }
        if (m_rtobjPtr->on_execute(m_id) != RTC::RTC_OK)
          {
            m_sm.goTo(RTC::ERROR_STATE);
          }
        if (m_measure)
          {
            m_svtMeasure.tack();
            if (count > 1000)
              {
                count = 0;
                m_svtMeasure.getStatistics(max_interval, min_interval,
                                           mean_interval, stddev);
                std::cout << "[servant] ";
                std::cout << " max: "    << max_interval;
                std::cout << " min: "    << min_interval;
                std::cout << " mean: "   << mean_interval;
                std::cout << " stddev: " << stddev;
                std::cout << std::endl;
              }
            ++count;
          }
        return;
      }
    // call Object reference
    if (!m_dfc) { return; }
    if (m_measure) { m_refMeasure.tick(); }
    RTC::ReturnCode_t ret = m_dfcVar->on_execute(m_id);
    if (m_measure)
      {
        m_refMeasure.tack();
        if (count > 1000)
          {
            count = 0;
            m_refMeasure.getStatistics(max_interval, min_interval,
                                       mean_interval, stddev);
            std::cout << "[objref] ";
            std::cout << " max: "    << max_interval;
            std::cout << " min: "    << min_interval;
            std::cout << " mean: "   << mean_interval;
            std::cout << " stddev: " << stddev;
            std::cout << std::endl;
          }
        ++count;
      }
    if (ret != RTC::RTC_OK)
      {
        m_sm.goTo(RTC::ERROR_STATE);
        return;
      }
    return;
  }

  void RTObjectStateMachine::onStateUpdate(const ExecContextStates&  /*st*/)
  {
    if (isNextState(RTC::ERROR_STATE)) { return; }
    // call Servant
    if (m_rtobjPtr != nullptr)
      {
        if (m_rtobjPtr->on_state_update(m_id) != RTC::RTC_OK)
          {
            m_sm.goTo(RTC::ERROR_STATE);
          }
        return;
      }
    // call Object reference
    if (!m_dfc) { return; }
    if (m_dfcVar->on_state_update(m_id) != RTC::RTC_OK)
      {
        m_sm.goTo(RTC::ERROR_STATE);
        return;
      }
    return;
  }

  RTC::ReturnCode_t RTObjectStateMachine::onRateChanged()
  {
    // call Servant
    if (m_rtobjPtr != nullptr)
      {
        RTC::ReturnCode_t ret = m_rtobjPtr->on_rate_changed(m_id);
        if (ret != RTC::RTC_OK)
          {
            m_sm.goTo(RTC::ERROR_STATE);
          }
        return ret;
      }
    // call Object reference
    if (!m_dfc) { return RTC::RTC_ERROR; }
    RTC::ReturnCode_t ret = m_dfcVar->on_rate_changed(m_id);
    if (ret != RTC::RTC_OK)
      {
        m_sm.goTo(RTC::ERROR_STATE);
      }
    return ret;
  }

  // FsmParticipantAction
  void RTObjectStateMachine::onAction(const ExecContextStates&  /*st*/)
  {
    if (!m_fsm) { return; }
    if (m_fsmVar->on_action(m_id) != RTC::RTC_OK)
      {
        m_sm.goTo(RTC::ERROR_STATE);
        return;
      }
    return;
  }
  // MultiModeComponentAction
  void RTObjectStateMachine::onModeChanged(const ExecContextStates&  /*st*/)
  {
    if (!m_mode) { return; }
    if (m_modeVar->on_mode_changed(m_id) != RTC::RTC_OK)
      {
        m_sm.goTo(RTC::ERROR_STATE);
        return;
      }
    return;
  }

  // Getting state of the context
  ExecContextState RTObjectStateMachine::getState()
  {
    return m_sm.getState();
  }

  ExecContextStates RTObjectStateMachine::getStates()
  {
    return m_sm.getStates();
  }

  bool RTObjectStateMachine::isCurrentState(ExecContextState state)
  {
    return getState() == state;
  }

  bool RTObjectStateMachine::isNextState(ExecContextState state)
  {
    return m_sm.getStates().next == state;
  }

  void RTObjectStateMachine::goTo(ExecContextState state)
  {
    m_sm.goTo(state);
  }

  // Workers
  void RTObjectStateMachine::workerPreDo()
  {
    updateState();
    return m_sm.worker_pre();
  }

  void RTObjectStateMachine::workerDo()
  {
    return m_sm.worker_do();
  }

  void RTObjectStateMachine::workerPostDo()
  {
    return m_sm.worker_post();
  }

  bool RTObjectStateMachine::activate()
  {
      if (isCurrentState(RTC::INACTIVE_STATE))
      {
          m_activation.store(true);
          return true;
      }
      return false;
  }
  bool RTObjectStateMachine::deactivate()
  {
      if (isCurrentState(RTC::ACTIVE_STATE))
      {
          m_deactivation.store(true);
          return true;
      }
      return false;
  }
  bool RTObjectStateMachine::reset()
  {
      if (isCurrentState(RTC::ERROR_STATE))
      {
          m_reset.store(true);
          return true;
      }
      return false;
  }

  void RTObjectStateMachine::updateState()
  {
      if (m_activation.load())
      {
          if (isCurrentState(RTC::INACTIVE_STATE) && !isNextState(RTC::ERROR_STATE))
          {
              m_sm.goTo(RTC::ACTIVE_STATE);
          }
          m_activation.store(false);
      }

      if (m_deactivation.load())
      {
          if (isCurrentState(RTC::ACTIVE_STATE) && !isNextState(RTC::ERROR_STATE))
          {
              m_sm.goTo(RTC::INACTIVE_STATE);
          }
          m_deactivation.store(false);
      }

      if (m_reset.load())
      {
          if (isCurrentState(RTC::ERROR_STATE))
          {
              m_sm.goTo(RTC::INACTIVE_STATE);
          }
          m_reset.store(false);
      }
  }
} // namespace RTC_impl
