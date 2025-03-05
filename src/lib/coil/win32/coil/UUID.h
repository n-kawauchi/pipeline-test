﻿// -*- C++ -*-
/*!
 * @file  MutexPosix.h
 * @brief RT-Middleware Service interface
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

#ifndef COIL_UUID_H
#define COIL_UUID_H

#include <rpc.h>

namespace coil
{
#ifdef uuid_t
#undef uuid_t
  using uuid_t = ::GUID;
#endif
  //------------------------------------------------------------
  // UUID class
  //------------------------------------------------------------
  /*!
   * @if jp
   * @class UUID クラス
   * @brief UUID の値を保持するクラス
   * @else
   * @class UUID class
   * @brief UUID value class
   * @endif
   */
  class UUID
  {
  public:
    /*!
     * @if jp
     * @brief UUIDクラス コンストラクタ
     *
     * @else
     * @brief UUID class constructor
     *
     * @endif
     */
    UUID();

    /*!
     * @if jp
     * @brief UUIDクラス コンストラクタ
     *
     * @else
     * @brief UUID class constructor
     *
     * @endif
     */
    explicit UUID(const uuid_t& uuid);

    /*!
     * @if jp
     * @brief UUIDクラス デストラクタ
     *
     * @else
     * @brief UUID class destructor
     *
     * @endif
     */
    ~UUID();

    /*!
     * @if jp
     * @brief UUID値を文字列に変換する
     *
     * @else
     * @brief Converting from UUID value to string
     *
     * @endif
     */
    const char* to_string();

  private:
    uuid_t m_uuid;
    char* m_uuidstr;
  };  // class UUID


  namespace UUID_Generator
  {
    /*!
     * @if jp
     * @brief UUIDを生成する
     *
     * @param n 未使用
     * @param h 未使用
     * @return 生成されたUUID値. 呼び出し側で delete する必要がある。
     *
     * @else
     * @brief Generate UUID value
     *
     * @param n unused
     * @param h unused
     * @return new UUID value. This value should be deleted by user.
     *
     * @endif
     */
    coil::UUID* generateUUID(int n, int h);
  } // namespace UUID_Generator
} // namespace coil

#endif  // COIL_UUID_H
