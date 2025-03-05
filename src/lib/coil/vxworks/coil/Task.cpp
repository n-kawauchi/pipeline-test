// -*- C++ -*-
/*!
 * @file Task_vxworks.cpp
 * @brief Task class
 * @date $Date$
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2017
 *     Nobuhiko Miyamoto
 *     Robot Innovation Research Center
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#include <coil/Task.h>

#define DEFAULT_PRIORITY 110
#define DEFAULT_STACKSIZE 60000

namespace coil
{

  /*!
   * @if jp
   * @brief ���󥹥ȥ饯��
   * @else
   * @brief Constructor
   * @endif
   */
  Task::Task()
    : m_count(0)
    ,m_priority(DEFAULT_PRIORITY)
    ,m_stacksize(DEFAULT_STACKSIZE)
    ,m_tid(-1)
  {
  }

  /*!
   * @if jp
   * @brief �ǥ��ȥ饯��
   * @else
   * @brief Destructor
   * @endif
   */
  Task::~Task()
  {
    m_count = 0;
  }

  /*!
   * @if jp
   * @brief �����������ץ�
   * @else
   * @brief Task open
   * @endif
   */
  int Task::open(void* args)
  {
    return 0;
  }

  /*!
   * @if jp
   * @brief ������������
   * @else
   * @brief Task close
   * @endif
   */
  int Task::close(unsigned long flags)
  {
    return 0;
  }

  /*!
   * @if jp
   * @brief ����åɤ�¹Ԥ���
   * @else
   * @brief Execute thread
   * @endif
   */
  int Task::svc()
  {
    return 0;
  }

  /*!
   * @if jp
   * @brief ����åɤ���������
   * @else
   * @brief Create a thread
   * @endif
   */
  void Task::activate()
  {
    if (m_count == 0)
      {
        m_tid = taskSpawn(
                         0,
                         m_priority,
                         VX_FP_TASK | VX_NO_STACK_FILL,
                         m_stacksize,
                         (FUNCPTR)Task::svc_run,
                         (int)this,
                         0,0,0,0,0,0,0,0,0
                         );
        ++m_count;
      };
  }

  /*!
   * @if jp
   * @brief ����åɽ�λ���Ԥ�
   * @else
   * @brief Waiting for the thread terminate
   * @endif
   */
  int Task::wait(void)
  {
    if (m_count > 0)
      {
        int task_priority = get_priority();
        int current_priority = -1;
        taskPriorityGet(taskIdSelf(), &current_priority);
        if(task_priority >= current_priority)
        {
          set_priority(current_priority-1);
        }
        std::lock_guard<coil::Mutex> guard(m_waitmutex);
      }
    return 0;
  }

  /*!
   * @if jp
   * @brief �������¹Ԥ����Ǥ���
   * @else
   * @brief Suspending the task
   * @endif
   */
  int Task::suspend(void)
  {
    return (OK == taskSuspend(m_tid));
  }

  /*!
   * @if jp
   * @brief ���Ǥ���Ƥ��륿������Ƴ�����
   * @else
   * @brief Resuming the suspended task
   * @endif
   */
  int Task::resume(void)
  {
    return (OK == taskResume(m_tid));
  }

  /*!
   * @if jp
   * @brief ���������ꥻ�å�
   * @else
   * @brief Reset of task count
   * @endif
   */
  void Task::reset()
  {
    m_count = 0;
  }

  /*!
   * @if jp
   * @brief �������¹Ԥ�λ����
   * @else
   * @brief Finalizing the task
   * @endif
   */
  void Task::finalize()
  {
    reset();
  }

  /*!
   * @if jp
   * @brief ����åɼ¹Ԥ򳫻Ϥ���
   * @else
   * @brief Start thread Execution
   * @endif
   */


  extern "C" void* Task::svc_run(void* args)
  {
    Task* t = (coil::Task*)args;

    std::lock_guard<coil::Mutex> guard(t->m_waitmutex);
    int status;
    status = t->svc();
    t->finalize();
    return 0;
  }

  /*!
   * @if jp
   *
   * @brief ��������ͥ���٤�����
   *
   *
   * @param priority ͥ����
   * 
   * @return 
   *
   * @else
   *
   * @brief 
   *
   *
   * @param priority 
   * 
   * @return 
   *
   * @endif
   */
  STATUS Task::set_priority(int priority)
  {
        if(m_tid == -1)
        {
            m_priority = priority;
            return OK;
        }
        else
        {
            m_priority = priority;
            return taskPrioritySet(m_tid, m_priority);
        }
  }
  /*!
   * @if jp
   *
   * @brief ��������ͥ���٤����
   *
   *
   * @return priority ͥ����
   *
   * @else
   *
   * @brief 
   *
   *
   * @return priority 
   *
   * @endif
   */
  int Task::get_priority()
  {
        if(m_tid == -1)
        {
            return m_priority;
        }
        else
        {
            int priority = -1;
            taskPriorityGet(m_tid, &priority);
            return priority;
        }
  }
  /*!
   * @if jp
   *
   * @brief �����å�������������
   *
   *
   * @param stacksize �����å�������
   *
   * @else
   *
   * @brief 
   *
   *
   * @param stacksize 
   *
   * @endif
   */
  void Task::set_stacksize(int stacksize)
  {
    m_stacksize = stacksize;
  }
};


