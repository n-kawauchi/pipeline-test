// -*- C++ -*-
/*!
 * @file Affinity.h
 * @brief  Processor affinity operation class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2016
 *     Noriaki Ando
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#ifndef COIL_AFFINITY_H
#define COIL_AFFINITY_H

#include <string>
#include <coil/Singleton.h>
#include <coil/TimeValue.h>
#include <coil/Mutex.h>
#include <mutex>



namespace coil
{
  typedef std::vector<unsigned int> CpuMask;
  /*!
   * @if jp
   * @brief �ץ�����CPU affinity���������
   * @prop cpu_mask �������ꤵ��Ƥ��� affinity �� CPU ID��
   *                std::vector<unsigned int> ���֤���롣
   * @return True: ������False: ����
   * @else
   * @brief Getting current process CPU affinity
   * @prop cpu_mask Current CPU affinity mask is returned as CPU ID in
   *                std::vector<unsigned int>.
   * @return True: success, False: fail
   * @endif
   */
  bool getProcCpuAffinity(CpuMask& cpu_mask);

  /*!
   * @if jp
   * @brief �ץ�����CPU affinity�����ꤹ��
   * @prop cpu_mask ���ꤹ�� CPU affinity �� CPU ID ��
   *                std::vector<unsigned int> �ꥹ�Ȥ�Ϳ����
   * @return True: ������False: ����
   * @else
   * @brief Setting process CPU affinity
   * @prop cpu_mask CPU affinity mask to be set is given with CPU ID in
   *                std::vector<unsigned int> list.
   * @return True: success, False: fail
   * @endif
   */
  bool setProcCpuAffinity(const CpuMask& cpu_mask);

  /*!
   * @if jp
   * @brief �ץ�����CPU affinity��ʸ��������ꤹ��
   * @prop cpu_mask ���ꤹ�� CPU affinity �� CPU ID �Υ���޶��ڤ�ʸ��
   *                ��Υꥹ�Ȥ�Ϳ����
   * @return True: ������False: ����
   * @else
   * @brief Setting process CPU affinity with string
   * @prop cpu_mask CPU affinity mask to be set is given with comma
   *                separated CPU ID string.
   * @return True: success, False: fail
   * @endif
   */
  bool setProcCpuAffinity(const std::string& cpu_mask);

  /*!
   * @if jp
   * @brief ����åɤ�CPU affinity���������
   * @prop cpu_mask �������ꤵ��Ƥ��� affinity �� CPU ID��
   *                std::vector<unsigned int> ���֤���롣
   * @return True: ������False: ����
   * @else
   * @brief Getting current process CPU affinity
   * @prop cpu_mask Current CPU affinity mask is returned as CPU ID in
   *                std::vector<unsigned int>.
   * @return True: success, False: fail
   * @endif
   */
  bool getThreadCpuAffinity(CpuMask& cpu_mask);

  /*!
   * @if jp
   * @brief ����åɤ�CPU affinity�����ꤹ��
   * @prop cpu_mask ���ꤹ�� CPU affinity �� CPU ID ��
   *                std::vector<unsigned int> �ꥹ�Ȥ�Ϳ����
   * @return True: ������False: ����
   * @else
   * @brief Setting process CPU affinity
   * @prop cpu_mask CPU affinity mask to be set is given with CPU ID in
   *                std::vector<unsigned int> list.
   * @return True: success, False: fail
   * @endif
   */
  bool setThreadCpuAffinity(const CpuMask& cpu_mask);

  /*!
   * @if jp
   * @brief ����åɤ�CPU affinity��ʸ��������ꤹ��
   * @prop cpu_mask ���ꤹ�� CPU affinity �� CPU ID �Υ���޶��ڤ�ʸ��
   *                ��Υꥹ�Ȥ�Ϳ����
   * @return True: ������False: ����
   * @else
   * @brief Setting process CPU affinity with string
   * @prop cpu_mask CPU affinity mask to be set is given with comma
   *                separated CPU ID string.
   * @return True: success, False: fail
   * @endif
   */
  bool setThreadCpuAffinity(const std::string& cpu_mask);

} // namespace coil
#endif // COIL_AFFINITY_H
