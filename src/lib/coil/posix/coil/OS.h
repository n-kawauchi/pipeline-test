// -*- C++ -*-
/*!
 * @file OS_posix.h
 * @brief OS class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2008
 *     Task-intelligence Research Group,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#ifndef COIL_OS_H
#define COIL_OS_H

#include <sys/utsname.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>
#include <string>

extern "C"
{
  extern char *optarg;
}

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
  using utsname = ::utsname;
  inline int uname(utsname* name)
  {
    return ::uname(name);
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
  using pid_t = ::pid_t;
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
    return ::getppid();
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
    GetOpt(int argc, char* const argv[], const char* opt, int  /*flag*/)
      : optarg(::optarg), optind(1), opterr(1), optopt(0),
        m_argc(argc), m_argv(argv), m_opt(opt)
    {
      ::optind = 1;
#ifdef __QNX__
      optind_last = 1;
#endif
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
#ifdef __QNX__
      optind_last = 1;
#endif
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
#ifndef __QNX__
      ::optind = optind;
#else
      ::optind = optind_last;
      ::optarg = 0;
#endif
      int result = getopt(m_argc, m_argv, m_opt);
#ifdef __QNX__
        if (::optind == optind_last)
          {
            ::optind++;
            result = getopt(m_argc, m_argv, m_opt);
            optind_last = ::optind;
          }
#endif
      optarg = ::optarg;
      optind = ::optind;
      optopt = ::optopt;
#ifdef __QNX__
      if (optind_last < m_argc)
        {
          ++optind_last;
        }
#endif
      return result;
    }

    char* optarg;     //! ���ץ�������
    int optind;       //! �����оݰ���
    int opterr;       //! ���顼ɽ�� 0:�޻ߡ�1:ɽ��
    int optopt;       //! ���ץ����ʸ����­��ʤ�����¿�����˥��åȤ����
#ifdef __QNX__
    int optind_last;
#endif

  private:
    int m_argc;
    char* const * m_argv;
    const char* m_opt;
  };

} // namespace coil

#endif  // COIL_OS_H
