// -*- C++ -*-
/*!
 * @file  Process.h
 * @brief Process handling functions
 * @date  $Date$
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

#ifndef COIL_PROCESS_H
#define COIL_PROCESS_H

#include <stdlib.h>
#include <unistd.h>

#include <coil/stringutil.h>

namespace coil
{

  /*!
   * @if jp
   * @brief �ץ�����ư����
   *
   * �ץ�����ư���롣�����ˤϵ�ư����ե�����̾��Ϳ���롣�¹ԥե���
   * ��ϴĶ��ѿ� PATH �����ꤵ�줿���ޥ�ɥ������ѥ��⤫�鸡�����졢��
   * �Ĥ���Х��ޥ�ɤ���ư����롣�ץ����ε�ư����������� 0�����Ԥ�
   * ��� -1 ���֤���
   *
   * @param command ��ư����¹ԥե�����Υե�ѥ�
   * @return 0: ����, -1: ����
   *
   * @else
   * @brief Launching a process
   *
   * This function launches a process. The argument is command file
   * name be launched. The command will be searched according to the
   * environment variable PATH of command search path. 0 will be
   * returned if launching process is successful, and -1 will be
   * returned if it is failed.
   * 
   * @param command full path string to a command to be executed.
   * @return 0: successful, -1: failed
   *
   * @endif
   */
  int launch_shell(std::string command);

  int daemon(int nochdir, int noclose);

  int create_process(std::string command, std::vector<std::string> &out);

} // namespace coil
#endif // COIL_PROCESS_H
