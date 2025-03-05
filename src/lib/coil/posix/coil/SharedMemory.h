// -*- C++ -*-
/*!
 * @file SharedMemory_vxworks.h
 * @brief SharedMemory class
 * @date $Date$
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2017 Nobuhiko Miyamoto
 *     Robot Innovation Research Center,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#ifndef COIL_SHAREDMEMORY_H
#define COIL_SHAREDMEMORY_H


#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdio>
#include <unistd.h>

#include <string>

#include <coil/config_coil.h>

#define DEFAULT_MEMORY_SIZE 2097152



namespace coil
{
  /*!
   * @if jp
   *
   * @class SharedMemory
   * @brief SharedMemory ���饹
   *
   * @else
   *
   * @class SharedMemory
   * @brief SharedMemory class
   *
   * @endif
   */
  class SharedMemory
  {
  public:

    /*!
     * @if jp
     *
     * @brief ���󥹥ȥ饯��
     *
     * ���󥹥ȥ饯����
     *
     *
     * @else
     *
     * @brief Constructor
     *
     * Constructor
     *
     *
     * @endif
     */
    SharedMemory();



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
    virtual ~SharedMemory();

    /*!
     * @if jp
     *
     * @brief ���ԡ����󥹥ȥ饯��
     *
     * ���ԡ����󥹥ȥ饯����
     *
     * @param rhs ���ԡ�����ͭ���ꥪ�֥�������
     *
     * @else
     *
     * @brief Copy Constructor
     *
     * Copy Constructor
     *
     * @param rhs shared memory object of copy source.
     *
     * @endif
     */
    SharedMemory(const SharedMemory& rhs);

    /*!
     * @if jp
     *
     * @brief �����黻��
     *
     * ��ͭ���ꥪ�֥������Ȥ򥳥ԡ����롣
     *
     * @param rhs ������ưŪ��ͭ���ꥪ�֥�������
     *
     * @return �������
     *
     * @else
     *
     * @brief Assignment operator
     *
     * Copy a shared memory object.
     *
     * @param rhs shared memory object of assignment source.
     *
     * @return Assignment result.
     *
     * @endif
     */
    SharedMemory& operator=(const SharedMemory& rhs);


    /*!
     * @if jp
     *
     * @brief ��ͭ���������
     *��
     *
     * @param shm_address ��ͭ����μ��̻�
     * @param memory_size ��ͭ����Υ�����
     *
     * @return 0: ����, -1: ����
     *
     * @else
     *
     * @brief Create Shared Memory 
     *
     *
     * @param shm_address 
     * @param memory_size 
     *
     * @return 0: successful, -1: failed
     *
     * @endif
     */
    virtual int create(std::string shm_address,
                     unsigned long long memory_size = DEFAULT_MEMORY_SIZE);


    /*!
     * @if jp
     *
     * @brief ��ͭ����ؤΥ�������
     *��
     *
     * @param shm_address ��ͭ����μ��̻�
     *
     * @return 0: ����, -1: ����
     *
     * @else
     *
     * @brief Open Shared Memory 
     *
     *
     * @param shm_address 
     * @param memory_size 
     *
     * @return 0: successful, -1: failed
     *
     * @endif
     */
    virtual int open(std::string shm_address,
                     unsigned long long memory_size = DEFAULT_MEMORY_SIZE);


    /*!
     * @if jp
     *
     * @brief ��ͭ����ؤν񤭹���
     *��
     *
     * @param data �񤭹���ǡ���
     *
     * @return 0: ����, -1: ����
     *
     * @else
     *
     * @brief Write Shared Memory 
     *
     *
     * @param data 
     *
     * @return 0: successful, -1: failed
     *
     * @endif
     */
    virtual int write(const char *data, unsigned long long pos, unsigned long long size);


    /*!
     * @if jp
     *
     * @brief ��ͭ���꤫����ɤ߹���
     *��
     *
     * @param data �ɤ߹���ǡ���
     *
     * @return 0: ����, -1: ����
     *
     * @else
     *
     * @brief Read Shared Memory 
     *
     *
     * @param data 
     *
     * @return 0: successful, -1: failed
     *
     * @endif
     */
    virtual int read(char* data, unsigned long long pos, unsigned long long size);

    /*!
     * @if jp
     *
     * @brief ��ͭ����κ��
     *��
     *
     *
     * @return 0: ����, -1: ����
     *
     * @else
     *
     * @brief Close Shared Memory 
     *
     *
     *
     * @return 0: successful, -1: failed
     *
     * @endif
     */
    virtual int close();
    /*!
     * @if jp
     *
     * @brief ����Υ���������
     *��
     *
     *
     * @return ������
     *
     * @else
     *
     * @brief 
     *
     *
     *
     * @return size
     *
     * @endif
     */
    virtual unsigned long long get_size();
    /*!
     * @if jp
     *
     * @brief ����Υ��ɥ쥹����
     *��
     *
     *
     * @return ���ɥ쥹
     *
     * @else
     *
     * @brief 
     *
     *
     *
     * @return address
     *
     * @endif
     */
    virtual std::string get_addresss();
    /*!
     * @if jp
     *
     * @brief ����μ���
     *
     *
     *
     * @return ����
     *
     * @else
     *
     * @brief 
     *
     *
     *
     * @return memory
     *
     * @endif
     */
    virtual char *get_data();
    /*!
     * @if jp
     *
     * @brief �ե�������(Windows�ξ��ϲ��⤷�ʤ�)
     *
     *
     *
     * @return 0: ����, -1: ����
     *
     * @else
     *
     * @brief 
     *
     *
     *
     * @return 0: successful, -1: failed
     *
     * @endif
     */
    virtual int unlink();
     /*!
     * @if jp
     *
     * @brief ��ͭ����������Ѥߤ��γ�ǧ
     *
     *
     *
     * @return true: �����Ѥ�,false: ̤����
     *
     * @else
     *
     * @brief 
     *
     *
     *
     * @return
     *
     * @endif
     */
    virtual bool created();

  private:
    unsigned long long m_memory_size{0};
    std::string m_shm_address;
    char *m_shm{nullptr};
    bool m_file_create{false};
    int m_fd{-1};
  };  // class SharedMemory

} // namespace coil

#endif // SharedMemory_h
