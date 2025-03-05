#!/bin/sh
#
# @file pkg_install_debian.sh
# @brief OpenRTM-aist dependent packages install script for Debian
# @author Noriaki Ando <n-ando@aist.go.jp>
#         Shinji Kurihara
#         Tetsuo Ando
#         Harumi Miyamoto
#         Nobu   Kawauchi
#

VERSION=2.0.0.00

#---------------------------------------
# usage
#---------------------------------------
usage()
{
  cat <<EOF
  Usage: 

    $(basename ${0}) -l {all|c++} [-r|-d|-s|-c] [-u|--yes]
    $(basename ${0}) [-u]
    $(basename ${0}) -l {python|java} [-r|-d|-c] [-u|--yes]
    $(basename ${0}) -l {openrtp|rtshell} [-d] [-u|--yes]
    $(basename ${0}) {--help|-h|--version}

  Example:
    $(basename ${0}) [= $(basename ${0}) -l all -d]
    $(basename ${0}) -l all -d
    $(basename ${0}) -l c++ -c --yes
    $(basename ${0}) -l all -u

  Options:
    -l <argument>  language or tool [c++|python|java|openrtp|rtshell|all]
        all        install packages of all the supported languages and tools
    -r             install robot component runtime
    -d             install robot component developer [default]
    -s             install tool_packages for build source packages
    -c             install tool_packages for core developer
    -u             uninstall packages
    --yes          force yes
    --help, -h     print this
    --version      print version number
EOF
}

version()
{
  echo ${VERSION}
}

#---------------------------------------
# パッケージリスト
#---------------------------------------
ace="libace libace-dev"
openrtm04="openrtm-aist=0.4.2-1 openrtm-aist-doc=0.4.2-1 openrtm-aist-dev=0.4.2-1 openrtm-aist-example=0.4.2-1 python-yaml"

default_reposerver="openrtm.org"
reposervers="openrtm.org"
reposerver=""

#--------------------------------------- C++
res=`grep wheezy /etc/os-release`
if test ! "x$res" = "x" ; then
  # wheezy
  autotools="autoconf libtool"
else
  autotools="autoconf libtool libtool-bin"
fi
base_tools="bc iputils-ping net-tools zip"
cxx_devel="gcc g++ make python-yaml"
cmake_tools="cmake doxygen graphviz nkf"
build_tools="subversion git"
deb_pkg="uuid-dev libboost-filesystem-dev"
pkg_tools="build-essential debhelper devscripts"
omni_devel="libomniorb4-dev omniidl"
omni_runtime="omniorb-nameserver"
openrtm_devel="openrtm-aist-doc openrtm-aist-dev"
openrtm_runtime="openrtm-aist openrtm-aist-example"

runtime_pkgs="$omni_runtime $openrtm_runtime"
u_runtime_pkgs=$runtime_pkgs

src_pkgs="$cxx_devel $cmake_tools $deb_pkg $base_tools $omni_runtime $omni_devel"
u_src_pkgs="$omni_runtime $omni_devel"

dev_pkgs="$runtime_pkgs $src_pkgs $openrtm_devel"
u_dev_pkgs="$u_runtime_pkgs $omni_devel $openrtm_devel"

core_pkgs="$src_pkgs $autotools $build_tools $pkg_tools"
u_core_pkgs="$u_src_pkgs"

#--------------------------------------- Python
res=`grep buster /etc/os-release`
if test ! "x$res" = "x" ; then
  # buster
  omnipy=""
  python_runtime="python"
else
  omnipy="omniidl-python"
  python_runtime="python python-pyorbit-omg"
fi
python_devel="python-pip $cmake_tools $base_tools $omnipy"
openrtm_py_devel="openrtm-aist-python-doc"
openrtm_py_runtime="openrtm-aist-python openrtm-aist-python-example"

python_runtime_pkgs="$omni_runtime $python_runtime $openrtm_py_runtime"
u_python_runtime_pkgs="$omni_runtime $openrtm_py_runtime"

python_dev_pkgs="$python_runtime_pkgs $python_devel $openrtm_py_devel"
u_python_dev_pkgs="$u_python_runtime_pkgs $omnipy $openrtm_py_devel"

python_core_pkgs="$omni_runtime $python_runtime $python_devel $build_tools $pkg_tools"
u_python_core_pkgs="$omni_runtime $omnipy"

#--------------------------------------- Java
java_devel="default-jdk"
java_build="ant"
openrtm_j_devel="openrtm-aist-java-doc"
openrtm_j_runtime="openrtm-aist-java openrtm-aist-java-example"

java_runtime_pkgs="$omni_runtime $java_devel $openrtm_j_runtime"
u_java_runtime_pkgs="$omni_runtime $openrtm_j_runtime"

java_dev_pkgs="$java_runtime_pkgs $cmake_tools $base_tools $openrtm_j_devel"
u_java_dev_pkgs="$omni_runtime $openrtm_j_runtime $openrtm_j_devel"

java_core_pkgs="$omni_runtime $java_devel $cmake_tools $base_tools $build_tools $java_build $pkg_tools"
u_java_core_pkgs="$omni_runtime"

#--------------------------------------- OpenRTP
openrtp_pkgs="openrtp"

#---------------------------------------
# Script options, argument analysis
#---------------------------------------
init_param()
{
OPT_RT=false
OPT_DEV=false
OPT_SRC=false
OPT_CORE=false
OPT_FLG=true
install_pkgs=""
uninstall_pkgs=""
}

check_arg()
{
  local arg=$1
  arg_err=0
  
  case "$arg" in
    all ) arg_all=true ;;
    c++ ) arg_cxx=true ;;
    python ) arg_python=true ;;
    java ) arg_java=true ;;
    openrtp ) arg_openrtp=true ;;
    rtshell ) arg_rtshell=true ;;
    *) arg_err=-1 ;;
  esac
}

get_opt()
{ 
  # オプション指定が無い場合のデフォルト設定
  if [ $# -eq 0 ] ; then
    arg_cxx=true
    OPT_DEV=true
  fi
  arg_num=$#
 
  OPT=`getopt -o l:rcsdhu -l help,yes,version -- $@` > /dev/null 2>&1
  # return code check
  if [ $? -ne 0 ] ; then
    echo "[ERROR] Invalid option '$1'"
    usage
    exit
  fi
  eval set -- $OPT

  while true
  do
    case "$1" in
        -h|--help ) usage ; exit ;;
        --version ) version ; exit ;;
        --yes ) FORCE_YES=true ;;
        -l )  if [ -z "$2" ] ; then
                echo "$1 option requires an argument." 1>&2
                exit
              fi
              check_arg $2
              if [ "$arg_err" = "-1" ]; then
                echo "[ERROR] Invalid argument '$2'"
                usage
                exit
              fi
              shift ;;
        -r )  OPT_RT=true ;;
        -d )  OPT_DEV=true ;;
        -s )  OPT_SRC=true ;;
        -c )  OPT_CORE=true ;;
        -u )  OPT_FLG=false ;;
        -- ) shift ; break ;;                 
        * )
          echo "Internal Error" 1>&2
          exit ;;
    esac
    shift
  done

  # オプション指定が -u のみの場合
  if [ $arg_num -eq 1 ] ; then
    if test "x$OPT_FLG" = "xfalse" ; then 
      arg_cxx=true
    fi
  fi
}

#---------------------------------------
# ロケールの言語確認
#---------------------------------------
check_lang()
{
lang="en"

locale | grep ja_JP > /dev/null && lang="jp"

if test "$lang" = "jp" ;then
    msg1="ディストリビューションを確認してください。\nDebian以外のOSの可能性があります。"
    msg2="コードネーム :"
    msg3="このOSはサポートしておりません。"
    msg4="OpenRTM-aist のリポジトリが登録されていません。"
    msg5="Source.list に OpenRTM-aist のリポジトリ: "
    msg6="を追加します。よろしいですか？(y/n)[y] "
    msg7="中断します。"
    msg8="ルートユーザーで実行してください。"
    msg9="インストール中です..."
    msg10="完了"
    msg11="アンインストール中です."
else
    msg1="This distribution may not be debian."
    msg2="The code name is : "
    msg3="This OS is not supported."
    msg4="No repository entry for OpenRTM-aist is configured in your system."
    msg5="repository entry for OpenRTM-aist: "
    msg6="Do you want to add new repository entry for OpenRTM-aist in source.list? (y/n) [y] "
    msg7="Abort."
    msg8="This script should be run as root."
    msg9="Now installing: "
    msg10="done."
    msg11="Now uninstalling: "
fi

}

#---------------------------------------
# コードネーム取得
#---------------------------------------
check_codename () {
  if test -f /etc/os-release ; then
    . /etc/os-release
    if test "x$ID" = "xdebian" ; then
      if test "x$VERSION_CODENAME" != "x" ; then
        code_name=$VERSION_CODENAME
      else
        code_name=`lsb_release -cs`
      fi
    else
      echo $msg1
      exit
    fi
  fi 
  if test ! "x$code_name" = "x"; then
    echo $msg2 $code_name
  else
    echo $msg3
    exit
  fi
}

#----------------------------------------
# 近いリポジトリサーバを探す
#----------------------------------------
check_reposerver()
{
  minrtt=65535
  nearhost=''
  for host in $reposervers; do
    rtt=`ping -c 1 $host | grep 'time=' | sed -e 's/^.*time=\([0-9\.]*\) ms.*/\1/' 2> /dev/null`
    if test "x$rtt" = "x"; then
      rtt=65535
    fi
    if test `echo "scale=2 ; $rtt < $minrtt" | bc` -gt 0; then
      minrtt=$rtt
      nearhost=$host
    fi
  done
  if test "x$nearhost" = "x"; then
    echo "Repository servers unreachable.", $hosts
    echo "Check your internet connection. (or are you using proxy?)"
    nearhost=$default_reposerver
  fi
  reposerver=$nearhost
}

#---------------------------------------
# リポジトリサーバ
#---------------------------------------
create_srclist () {
  openrtm_repo="deb http://$reposerver/pub/Linux/debian/ $code_name main"
}

#---------------------------------------
# ソースリスト更新関数の定義
#---------------------------------------
update_source_list () {
  rtmsite=`grep "$openrtm_repo" /etc/apt/sources.list`
  if test "x$rtmsite" = "x" ; then
    echo $msg4
    echo $msg5
    echo "  " $openrtm_repo
    read -p "$msg6" kick_shell

    if test "x$kick_shell" = "xn" ; then
      echo $msg7
      exit 0
    else
      echo $openrtm_repo >> /etc/apt/sources.list
    fi
  fi

  if test "x$FORCE_YES" = "xtrue" ; then
    apt-get install --assume-yes --allow-unauthenticated dirmngr
  else
    apt-get install dirmngr
  fi
  # 公開鍵登録
  apt-key adv --keyserver keys.gnupg.net --recv-keys 4BCE106E087AFAC0
}

#----------------------------------------
# root かどうかをチェック
#----------------------------------------
check_root () {
  if test ! `id -u` = 0 ; then
    echo ""
    echo $msg8
    echo $msg7
    echo ""
    exit 1
  fi
}

#----------------------------------------
# パッケージインストール関数
#----------------------------------------
install_packages () {
  for p in $*; do
    echo $msg9 $p
    echo $install_pkgs | grep $p > /dev/null 2>&1
    if [ $? -ne 0 ]; then
      tmp_pkg="$install_pkgs $p"
      install_pkgs=$tmp_pkg
    fi

    if test "x$FORCE_YES" = "xtrue" ; then
      apt-get install --assume-yes --allow-unauthenticated $p
    else
      apt-get install $p
    fi
    if [ $? -ne 0 ]; then
      echo $msg7
      exit
    fi
    echo $msg10
    echo ""
  done
}

#------------------------------------------------------------
# リストを逆順にする
#------------------------------------------------------------
reverse () {
  for i in $*; do
    echo $i
  done | sed '1!G;h;$!d'
}

#----------------------------------------
# パッケージをアンインストールする
#----------------------------------------
uninstall_packages () {
  for p in $*; do
    echo $msg11 $p
    echo $uninstall_pkgs | grep $p > /dev/null 2>&1
    if [ $? -ne 0 ]; then
      tmp_pkg="$uninstall_pkgs $p"
      uninstall_pkgs=$tmp_pkg
    fi
    apt-get --purge remove $p
    if test "$?" != 0; then
      apt-get purge $p
    fi
    echo $msg10
    echo ""
  done
}

#---------------------------------------
# install_branch
#---------------------------------------
install_branch()
{
  if test "x$arg_cxx" = "xtrue" ; then
    if test "x$OPT_CORE" = "xtrue" ; then
      select_opt_c="[c++] install tool_packages for core developer"
      install_packages $core_pkgs
    elif test "x$OPT_SRC" = "xtrue" ; then
      select_opt_c="[c++] install tool_packages for source packages"
      install_packages $src_pkgs
    elif test "x$OPT_RT" = "xtrue" ; then
      select_opt_c="[c++] install robot component runtime"
      install_packages $runtime_pkgs
    else
      select_opt_c="[c++] install robot component developer"
      install_packages $dev_pkgs
    fi
  fi

  if test "x$arg_python" = "xtrue" ; then
    if test "x$OPT_CORE" = "xtrue" ; then
      select_opt_p="[python] install tool_packages for core developer"
      install_packages $python_core_pkgs
    elif test "x$OPT_RT" = "xtrue" ; then
      select_opt_p="[python] install robot component runtime"
      install_packages $python_runtime_pkgs
    else
      select_opt_p="[python] install robot component developer"
      install_packages $python_dev_pkgs
    fi
  fi

  if test "x$arg_java" = "xtrue" ; then
    if test "x$OPT_CORE" = "xtrue" ; then
      select_opt_j="[java] install tool_packages for core developer"
      install_packages $java_core_pkgs
    elif test "x$OPT_RT" = "xtrue" ; then
      select_opt_j="[java] install robot component runtime"
      install_packages $java_runtime_pkgs
    else
      select_opt_j="[java] install robot component developer"
      install_packages $java_dev_pkgs
    fi
  fi

  if test "x$arg_openrtp" = "xtrue" ; then
    select_opt_rtp="[openrtp] install"
    install_packages $openrtp_pkgs
  fi

  if test "x$arg_rtshell" = "xtrue" ; then
    select_opt_shl="[rtshell] install"
    install_packages python-pip
    rtshell_ret=`pip install rtshell-aist`
  fi
}

#---------------------------------------
# uninstall_branch
#---------------------------------------
uninstall_branch()
{
  if test "x$arg_cxx" = "xtrue" ; then
    if test "x$OPT_CORE" = "xtrue" ; then
      select_opt_c="[c++] uninstall tool_packages for core developer"
      uninstall_packages `reverse $u_core_pkgs`
    elif test "x$OPT_SRC" = "xtrue" ; then
      select_opt_c="[c++] uninstall tool_packages for source packages"
      uninstall_packages `reverse $u_src_pkgs`
    elif test "x$OPT_RT" = "xtrue" ; then
      select_opt_c="[c++] uninstall robot component runtime"
      uninstall_packages `reverse $u_runtime_pkgs`
    else
      select_opt_c="[c++] uninstall robot component developer"
      uninstall_packages `reverse $u_dev_pkgs`
    fi
  fi

  if test "x$arg_python" = "xtrue" ; then
    if test "x$OPT_CORE" = "xtrue" ; then
      select_opt_p="[python] uninstall tool_packages for core developer"
      uninstall_packages `reverse $u_python_core_pkgs`
    elif test "x$OPT_RT" = "xtrue" ; then
      select_opt_p="[python] uninstall robot component runtime"
      uninstall_packages `reverse $u_python_runtime_pkgs`
    else
      select_opt_p="[python] uninstall robot component developer"
      uninstall_packages `reverse $u_python_dev_pkgs`
    fi
  fi

  if test "x$arg_java" = "xtrue" ; then
    if test "x$OPT_CORE" = "xtrue" ; then
      select_opt_j="[java] uninstall tool_packages for core developer"
      uninstall_packages `reverse $u_java_core_pkgs`
    elif test "x$OPT_RT" = "xtrue" ; then
      select_opt_j="[java] uninstall robot component runtime"
      uninstall_packages `reverse $u_java_runtime_pkgs`
    else
      select_opt_j="[java] uninstall robot component developer"
      uninstall_packages `reverse $u_java_dev_pkgs`
    fi
  fi

  if test "x$arg_openrtp" = "xtrue" ; then
    select_opt_rtp="[openrtp] uninstall"
    uninstall_packages $openrtp_pkgs
  fi

  if test "x$arg_rtshell" = "xtrue" ; then
    select_opt_shl="[rtshell] uninstall"
    rtshell_ret=`pip uninstall -y rtshell-aist rtctree-aist rtsprofile-aist`
  fi
}

#---------------------------------------
# print_option
#---------------------------------------
print_option()
{
  cat <<EOF

=============================================
 Selected options is ...
=============================================
EOF

  if test ! "x$select_opt_c" = "x" ; then
    echo $select_opt_c
  fi
  if test ! "x$select_opt_p" = "x" ; then
    echo $select_opt_p
  fi
  if test ! "x$select_opt_j" = "x" ; then
    echo $select_opt_j
  fi
  if test ! "x$select_opt_rtp" = "x" ; then
    echo $select_opt_rtp
  fi
  if test ! "x$select_opt_shl" = "x" ; then
    echo $select_opt_shl
  fi
}

#---------------------------------------
# install_result
#---------------------------------------
install_result()
{
  print_option
  cat <<EOF

=============================================
 Install package is ...
=============================================
EOF
  if [ $# -eq 0 ] && test "x$OPT_FLG" = "xfalse"; then
    echo "There is no installation package."
    return
  fi

  for p in $*; do
    echo $p
  done
  if test "x$arg_rtshell" = "xtrue" && test "x$OPT_FLG" = "xtrue"; then
    if test "x$rtshell_ret" != "x"; then
      echo "rtshell" 
    fi
  fi
}

#---------------------------------------
# uninstall_result
#---------------------------------------
uninstall_result()
{
  cat <<EOF

=============================================
 Uninstall package is ...
=============================================
EOF
  if [ $# -eq 0 ] && test "x$OPT_FLG" = "xtrue"; then
    echo "There is no uninstall package."
    return
  fi

  for p in $*; do
    echo $p
  done
  if test "x$arg_rtshell" = "xtrue" && test "x$OPT_FLG" = "xfalse"; then
    if test "x$rtshell_ret" != "x"; then
      echo "rtshell" 
    fi
  fi
}

#---------------------------------------
# メイン
#---------------------------------------
init_param
get_opt $@

check_lang
check_root
check_reposerver
check_codename
create_srclist
update_source_list
apt-get autoclean
apt-get update

if test "x$arg_all" = "xtrue" ; then
  arg_cxx=true
  arg_python=true
  arg_java=true
  arg_openrtp=true
  arg_rtshell=true

  if test "x$OPT_RT" != "xtrue" && 
     test "x$OPT_DEV" != "xtrue" &&
     test "x$OPT_SRC" != "xtrue" && 
     test "x$OPT_CORE" != "xtrue" ; then
    # set default option
    OPT_DEV=true
  fi
fi

if test "x$OPT_FLG" = "xtrue" ; then
  install_branch
else
  uninstall_branch
fi

install_result $install_pkgs
uninstall_result $uninstall_pkgs
