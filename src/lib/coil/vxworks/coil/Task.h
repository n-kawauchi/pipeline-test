// -*- C++ -*-
/*!
 * @file Task_vxworks.h
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

#ifndef COIL_TASK_H
#define COIL_TASK_H

#include <vxWorks.h>
#include <taskLib.h>
#include <coil/Mutex.h>
#include <mutex>

namespace coil
{
  /*!
   * @if jp
   *
   * @class Task
   * @brief Task ���饹
   *
   * @else
   *
   * @class Task
   * @brief Task class
   *
   * @endif
   */
  class Task
  {
  public:
    /*!
     * @if jp
     *
     * @brief ���󥹥ȥ饯��
     *
     * ���󥹥ȥ饯��
     *
     * @else
     *
     * @brief Constructor
     *
     * Constructor
     *
     * @endif
     */
    Task();

    /*!
     * @if jp
     *
     * @brief �ǥ��ȥ饯��
     *
     * �ǥ��ȥ饯��
     *
     * @else
     *
     * @brief Destructor
     *
     * Destructor
     *
     * @endif
     */
    virtual ~Task();

    /*!
     * @if jp
     *
     * @brief �����������ץ�
     *
     * �����������ץ�
     *
     * @param args ����
     *
     * @else
     *
     * @brief Task open
     *
     * Task open
     *
     * @param args Arguments
     *
     * @endif
     */
    virtual int open(void* args = 0);

    /*!
     * @if jp
     *
     * @brief ������������
     *
     * ������������
     *
     * @param flags �ե饰
     *
     * @else
     *
     * @brief Task close
     *
     * Task close
     *
     * @param flags Flags
     *
     * @endif
     */
    virtual int close(unsigned long flags = 0);

    /*!
     * @if jp
     *
     * @brief ����åɤ�¹Ԥ���
     *
     * ����åɤ�¹Ԥ���
     *
     * @else
     *
     * @brief Execute thread
     *
     * Execute thread
     *
     * @endif
     */
    virtual int svc();

    /*!
     * @if jp
     *
     * @brief ����åɤ���������
     *
     * ����åɤ���������
     *
     * @else
     *
     * @brief Create a thread
     *
     * Create a thread
     *
     * @endif
     */
    virtual void activate();

    /*!
     * @if jp
     *
     * @brief ����åɽ�λ���Ԥ�
     *
     * ����åɽ�λ���Ԥ�
     *
     * @else
     *
     * @brief Waiting for the thread terminate
     *
     * Waiting for the thread terminate
     *
     * @endif
     */
    virtual int wait(void);

    /*!
     * @if jp
     *
     * @brief �������¹Ԥ����Ǥ���
     *
     * �������¹Ԥ����Ǥ���
     *
     * @else
     *
     * @brief Suspending the task
     *
     * Suspending the task
     *
     * @endif
     */
    virtual int suspend(void);

    /*!
     * @if jp
     *
     * @brief ���Ǥ���Ƥ��륿������Ƴ�����
     *
     * ���Ǥ���Ƥ��륿������Ƴ�����
     *
     * @else
     *
     * @brief Resuming the suspended task
     *
     * Resuming the suspended task
     *
     * @endif
     */
    virtual int resume(void);

    /*!
     * @if jp
     *
     * @brief ���������ꥻ�å�
     *
     * ���������ꥻ�å�
     *
     * @else
     *
     * @brief Reset of task count
     *
     * Reset of task count
     *
     * @endif
     */
    virtual void reset();

    /*!
     * @if jp
     *
     * @brief �������¹Ԥ�λ����
     *
     * �������¹Ԥ�λ����
     *
     * @else
     *
     * @brief Finalizing the task
     *
     * Finalizing the task.
     *
     * @endif
     */
    virtual void finalize();

    /*!
     * @if jp
     *
     * @brief ����åɼ¹Ԥ򳫻Ϥ���
     *
     * ����åɼ¹Ԥ򳫻Ϥ���
     *
     * @param args ����åɰ���
     *
     * @else
     *
     * @brief Start thread Execution
     *
     * Start thread Execution
     *
     * @param args Thread arguments
     *
     * @endif
     */
    static void* svc_run(void* args = 0);
/*
#ifdef __RTP__
*/
    /*!
     * @if jp
     *
     * @brief ��������ͥ���٤�����
     *
     *
     * @param priority ͥ����
     *
     * @else
     *
     * @brief 
     *
     *
     * @param priority 
     *
     * @endif
     */
    STATUS set_priority(int priority);
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
     int get_priority();
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
    void set_stacksize(int stacksize);
    coil::Mutex m_waitmutex;
  private:
    int m_count;
    int m_tid;
    int m_priority;
    int m_stacksize;
    void* m_args;
  };

};

#endif // COIL_TASK_H
