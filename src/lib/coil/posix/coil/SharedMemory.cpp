// -*- C++ -*-
/*!
 * @file SharedMemory_posix.cpp
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


#include <coil/SharedMemory.h>
#include <cstring>
#include <utility>


namespace coil
{
  /*!
   * @if jp
   * @brief ���󥹥ȥ饯��
   * @else
   * @brief Constructor
   * @endif
   */
  SharedMemory::SharedMemory()
  {
  }


  /*!
   * @if jp
   * @brief �ǥ��ȥ饯��
   * @else
   * @brief Destructor
   * @endif
   */
  SharedMemory::~SharedMemory()
  {
    close();
  }

  /*!
   * @if jp
   * @brief ���ԡ����󥹥ȥ饯��
   * @else
   * @brief Copy Constructor
   * @endif
   */
  SharedMemory::SharedMemory(const SharedMemory& rhs)
  {
    m_memory_size = rhs.m_memory_size;
    m_shm_address = rhs.m_shm_address;
    m_shm = rhs.m_shm;
    m_fd = rhs.m_fd;
 

  }

  /*!
   * @if jp
   * @brief �����黻��
   * @else
   * @brief Assignment operator
   * @endif
   */
  SharedMemory& SharedMemory::operator=(const SharedMemory& rhs)
  {
    SharedMemory tmp(rhs);
    std::swap(this->m_memory_size, tmp.m_memory_size);
    std::swap(this->m_shm_address, tmp.m_shm_address);
    std::swap(this->m_shm, tmp.m_shm);
    std::swap(this->m_fd, tmp.m_fd);
    std::swap(this->m_file_create, tmp.m_file_create);

    return *this;
  }

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
  int SharedMemory::create(std::string shm_address,
                     unsigned long long memory_size)
  {

    m_shm_address = std::move(shm_address);
    m_memory_size = memory_size;
 

    m_fd = shm_open(m_shm_address.c_str(), O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
    if(m_fd < 0)
    {
        return -1;
    }
    ftruncate(m_fd, m_memory_size);
    m_shm = static_cast<char*>(mmap(nullptr,
            m_memory_size,
            PROT_READ|PROT_WRITE,
            MAP_SHARED,
            m_fd,
            0));

    m_file_create = true;
    return 0;
  }

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
  int SharedMemory::open(std::string shm_address, unsigned long long memory_size)
  {
    m_shm_address = std::move(shm_address);
    m_memory_size = memory_size;


    m_fd = shm_open(m_shm_address.c_str(), O_RDWR|O_CREAT, 0);
    if(m_fd < 0)
    {
        return -1;
    }
    ftruncate(m_fd, m_memory_size);
    m_shm = static_cast<char*>(mmap(nullptr,
            m_memory_size,
            PROT_READ|PROT_WRITE,
            MAP_SHARED,
            m_fd,
            0));
 
    return 0;
  }

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
  int SharedMemory::write(const char *data, const unsigned long long pos, const unsigned long long size)
  {
    if (!created())
    {
      return -1;
    }

    memcpy(&m_shm[pos],&data[0],static_cast<size_t>(size));
    
    return 0;
  }

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
  int SharedMemory::read(char* data, const unsigned long long pos, const unsigned long long size)
  {
    if (!created())
    {
      return -1;
    }

    memcpy(&data[0],&m_shm[pos],static_cast<size_t>(size));

    return 0;
  }

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
  int SharedMemory::close()
  {
    
    if (created())
    {
        ::close(m_fd);
    }
    else
    {
        return -1;
    }
    return 0;

  }
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
  unsigned long long SharedMemory::get_size()
  {
    return m_memory_size;
  }
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
  std::string SharedMemory::get_addresss()
  {
    return m_shm_address;
  }
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
  char *SharedMemory::get_data()
  {
    return m_shm;
  }



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
  int SharedMemory::unlink()
  {
     shm_unlink(m_shm_address.c_str());
     return 0;
  }


  /*!
  * @if jp
  *
  * @brief ��ͭ����������Ѥߤ��γ�ǧ
  *
  *
  *
  * @return true: �����Ѥ�, false: ̤����
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
  bool SharedMemory::created()
  {
    return m_fd >= 0;
  }




} // namespace coil
