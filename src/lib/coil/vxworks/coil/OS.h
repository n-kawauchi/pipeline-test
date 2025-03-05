// -*- C++ -*-
/*!
 * @file OS_vxworks.h
 * @brief OS class
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

#ifndef COIL_OS_H
#define COIL_OS_H

#include <string>

#if defined(__RTP__)
#include <sys/utsname.h>
#else
#include <hostLib.h>
#include <kernelLib.h>
#endif
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>

extern "C"
{
  extern char *optarg;
};

namespace coil
{
  /*!
   * @if jp
   *
   * @brief �����ƥ�������
   *
   * �����ƥ�����¤�Τ����ꤷ���֤���
   *
   * @param name ��¤��̾��
   *
   * @return 0: ����, -1: ����
   *
   * @else
   *
   * @brief Get System information
   *
   * Return a system information to a structure.
   *
   * @param name Name of structure
   *
   * @return 0: Successful, -1: failed
   *
   * @endif
   */
#if defined(__RTP__)
  typedef ::utsname utsname;
#else
  #define COIL_UTSNAME_LENGTH 256
  struct utsname
  {
    char sysname[COIL_UTSNAME_LENGTH];
    char nodename[COIL_UTSNAME_LENGTH];
    char release[COIL_UTSNAME_LENGTH];
    char version[COIL_UTSNAME_LENGTH];
    char machine[COIL_UTSNAME_LENGTH];
  };
  
#endif
  inline int uname(utsname* name)
  {
#if defined(__RTP__)
    return ::uname(name);
#else
    if(!name)
    {
        return -1;
    }

    if (gethostname(name->nodename, MAXHOSTNAMELEN) < 0) {
        return -1;
    }

    ::strcpy(name->machine, kernelVersion());
    ::strcpy(name->release, RUNTIME_VERSION);
    ::strcpy(name->sysname, RUNTIME_NAME);
    ::strcpy(name->version, "reserved");

    return 0;
#endif
  }


  /*!
   * @if jp
   *
   * @brief �ƤӽФ����ץ����Υץ���ID����
   *
   * �ƤӽФ����ץ����Υץ���ID���֤���
   *
   * @return �ץ���ID
   *
   * @else
   *
   * @brief Get process ID of the caller process
   *
   * Return a process ID of the caller process.
   *
   * @return Process ID
   *
   * @endif
   */
  typedef RTP_ID pid_t;
  inline pid_t getpid()
  {
    return ::getpid();
  }

  /*!
   * @if jp
   *
   * @brief �ƤӽФ����ץ����οƥץ����Υץ���ID����
   *
   * �ƤӽФ����ץ����οƥץ����Υץ���ID���֤���
   *
   * @return �ץ���ID
   *
   * @else
   *
   * @brief Get process ID of the parent process
   *
   * Return a process ID of the parent process.
   *
   * @return Process ID
   *
   * @endif
   */

  inline pid_t getppid()
  {
#if defined(__RTP__)
    return ::getppid();
#else
    return 0;
#endif
  }


  /*!
   * @if jp
   *
   * @brief �Ķ��ѿ�����
   *
   * �Ķ��ѿ����֤���
   *
   * @param name �Ķ��ѿ�̾��
   *
   * @return �Ķ��ѿ�����(NULL: �����ʤ�)
   *
   * @else
   *
   * @brief Get environment variable
   *
   * Return a environment variable.
   *
   * @param name Name of environment variable
   *
   * @return Value of environment variable(NULL: nonexistent)
   *
   * @endif
   */
  inline bool getenv(const char *name, std::string &env)
  {
    char* c = ::getenv(name);
    if(c == nullptr)
    {
        return false;
    }
    env = c;
    return true;
  }


  /* Global Variables for getopt() */

  /*!
   * @if jp
   *
   * @class GetOpt
   * @brief GetOpt ���饹
   *
   * @else
   *
   * @class GetOpt
   * @brief GetOpt class
   *
   * @endif
   */
  class GetOpt
  {
  public:
    /*!
     * @if jp
     *
     * @brief ���󥹥ȥ饯��
     *
     * ���󥹥ȥ饯����
     *
     * @param name ���֥�������̾
     *
     * @else
     *
     * @brief Constructor
     *
     * Constructor
     *
     * @param name Object name
     *
     * @endif
     */
    GetOpt(int argc, char* const argv[], const char* opt, int flag)
      : optarg(::optarg), optind(1), opterr(1), optopt(0), m_argc(argc), m_argv(argv), m_opt(opt), m_flag(flag)
    {
      ::optind = 1;
    }

    /*!
     * @if jp
     *
     * @brief �ǥ��ȥ饯��
     *
     * �ǥ��ȥ饯����
     *
     * @else
     *
     * @brief Destructor
     *
     * Destructor
     *
     * @endif
     */
    ~GetOpt()
    {
      ::optind = 1;
    }

    /*!
     * @if jp
     *
     * @brief ���ޥ�ɥ饤���������
     *
     * ���ޥ�ɥ饤���������Ϥ��롣
     *
     * @return ���Ϸ��
     *
     * @else
     *
     * @brief Parses the command line arguments
     *
     * Parses the command line arguments.
     *
     * @return Result of parses.
     *
     * @endif
     */
    int operator()()
    {
      ::opterr = opterr;
      ::optind = optind;
      int result = getopt(m_argc, m_argv, m_opt);

      optarg = ::optarg;
      optind = ::optind;
      optopt = ::optopt;
      return result;
    }

    char* optarg;     //! ���ץ�������
    int optind;       //! �����оݰ���
    int opterr;       //! ���顼ɽ�� 0:�޻ߡ�1:ɽ��
    int optopt;       //! ���ץ����ʸ����­��ʤ�����¿�����˥��åȤ����

  private:
    int m_argc;
    char* const * m_argv;
    const char* m_opt;
    int m_flag;
  };
    
};

#endif // COIL_OS_H
