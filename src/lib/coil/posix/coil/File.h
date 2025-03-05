// -*- C++ -*-
/*!
 * @file File_posix.h
 * @brief File functions
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

#ifndef COIL_FILE_H
#define COIL_FILE_H

#include <coil/config_coil.h>
#include <coil/stringutil.h>

#include <dirent.h>
#include <libgen.h>
#include <cstdio>
#include <sys/types.h>
#include <string>
#include <cstring>

#include <cstring>
#include <sys/stat.h>


#ifdef __QNX__
using std::strlen;
using std::strcpy;
#endif

namespace coil
{

  /*!
   * @if jp
   *
   * @brief �ե�����ѥ����ǥ��쥯�ȥ���ʬ���������
   *
   * �ե�����ѥ����ǥ��쥯�ȥ���ʬ��������롣
   *
   * @param path �ե�����ѥ�
   *
   * @return �ǥ��쥯�ȥ�̾��
   *
   * @else
   *
   * @brief Get a directory part than a file pass
   *
   * Get a directory part than a file pass.
   *
   * @param path File path
   *
   * @return Directory name
   *
   * @endif
   */
  std::string dirname(const char* path);

  /*!
   * @if jp
   *
   * @brief �ե�����ѥ����ե�����̾��ʬ���������
   *
   * �ե�����ѥ����ե�����̾��ʬ��������롣
   *
   * @param path �ե�����ѥ�
   *
   * @return �ե�����̾��
   *
   * @else
   *
   * @brief Get a file name part than a file pass
   *
   * Get a directory part than a file pass.
   *
   * @param path File path
   *
   * @return File name
   *
   * @endif
   */
  inline std::string basename(const char* path)
  {
    std::string path_name(path);
    std::string base_name = ::basename(&path_name[0]);
    return base_name;
  }

  /*!
   * @if jp
   *
   * @brief �ե�����ꥹ�Ȥ��������
   *
   * �ǥ��쥯�ȥ�ѥ�����ǻ���ե�����˥ޥå�����ꥹ�Ȥ�������롣
   *
   * @param path �ǥ��쥯�ȥ�ѥ�
   * @param glob_str �ե�����̾
   *
   * @return �ե�����ꥹ��
   *
   * @else
   *
   * @brief Get file list
   *
   * Get a list matching a file designated than a directory path.
   *
   * @param path Directory path
   * @param glob_str File name
   *
   * @return File list
   *
   * @endif
   */
  inline coil::vstring filelist(const char* path, const char* glob_str = "")
  {
    struct dirent* ent;
    coil::vstring flist;
    bool has_glob(false);

    if (path == nullptr) { return flist; }
    if (glob_str[0] != '\0') { has_glob = true; }

    DIR* dir_ptr(::opendir(path));
    if (dir_ptr == nullptr) { return flist; }

    while ((ent = ::readdir(dir_ptr)) != nullptr)
      {
        std::string fname(ent->d_name);
        bool match(true);
        if (has_glob)
          {
            const char* globc(glob_str);
            for (size_t i(0); i < fname.size() && *globc != '\0'; ++i, ++globc)
              {
                if (*globc == '*')
                  {
                    // the last '*' matches every thing
                    if (globc[1] == '\0') { break; }
                    // consecutive * or + are skiped, but fname keeps pointer
                    if (globc[1] == '*' || globc[1] == '+') { --i; continue; }

                    // advance pointer and find normal characters
                    ++globc;
                    size_t pos(fname.find(*globc, i));
                    if (pos == std::string::npos) { match = false; break; }
                    // matched, and advance i to pos
                    i = pos;
                  }
                else if (*globc == '+')
                  {
                    // the last '+' matches last one or more characters
                    if (globc[1] == '\0' && !(i + 1 < fname.size())) { break; }
                    // consecutive * or + are skiped, but fname keeps pointer
                    if (globc[1] == '*' || globc[1] == '+') { --i; continue; }

                    // advance pointer and find normal characters
                    ++globc;
                    size_t pos(fname.find(*globc, i + 1));
                    if (pos == std::string::npos) { match = false; break; }
                    // matched, and advance i to pos
                    i = pos;
                  }
                else
                  {
                    if (fname[i] != *globc) { match = false; }
                  }

                // in the last fname character, if glob is not end,
                // or *, fname is not matched.
                if (i + 1 == fname.size() &&
                    globc[1] != '\0' && globc[1] != '*') { match = false; }
              }
          }
        if (match) { flist.emplace_back(std::move(fname)); }
      }
    ::closedir(dir_ptr);

    return flist;
  }


  /*!
  * @if jp
  *
  * @brief ����ե�����̾�����ǥ��쥯�ȥ꤫��õ������
  *
  * @param dir �ǥ��쥯�ȥ�ѥ�
  * @param filename �ǥ��쥯�ȥ�ѥ�
  * @param filelist �ե��������
  *
  *
  * @else
  *
  * @brief
  *
  * @param dir
  * @param filename
  * @param filelist
  *
  * @endif
  */
  void findFile(const std::string& dir, const std::string& filename, coil::vstring &filelist);

  /*!
  * @if jp
  *
  * @brief �ե�������������ǥ��쥯�ȥ꤫��õ������
  *
  * @param dir �ǥ��쥯�ȥ�ѥ�
  * @param ext ��ĥ��
  * @param filelist �ե��������
  *
  *
  * @else
  *
  * @brief
  *
  * @param dir
  * @param ext
  * @param filelist
  *
  * @endif
  */
  void getFileList(const std::string& dir, const std::string& ext, coil::vstring &filelist);
} // namespace coil

#endif // COIL_FILE_H
