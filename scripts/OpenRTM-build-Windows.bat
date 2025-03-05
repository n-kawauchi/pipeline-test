@echo off
@rem �{�X�N���v�g�̓C���X�g�[��msi�ɑg����msm�����p�\�[�X�r���h�X�N���v�g�̂���
@rem Debug/Release�̗����[�h�Ńr���h���Ă���
@rem
@rem Release�r���h�́Abuild-release�f�B���N�g���𗘗p
@rem Debug�r���h�́Abuild-debug�f�B���N�g���𗘗p
@rem

@rem ======================================================== 
@rem ���ɍ��킹�ĉ��L�ϐ���ύX����
set CMAKE_GENERATOR="Visual Studio 16 2019"

@rem Python�̓C���X�g�[�����Ă��邪�p�X�͒ʂ��Ă��Ȃ�������
@rem C:\Python311�փC���X�g�[�����Ă���ꍇ�̐ݒ� 
set PY_VERSION=311
set path=C:\Python%PY_VERSION%;%PATH%

@rem OpenRTM-aist�̃C���X�g�[����w��
set INSTALL_PREFIX=C:\localRTM

@rem Boost�̃C���X�g�[����
set BOOST_PATH=C:\Boost
set Boost_DIR=C:\Boost\lib\cmake\Boost-1.87.0

@rem FluentBit�̃C���X�g�[����
@rem OpenRTM v2.1.0�ł́AFluentBit v3.10.0 �Ńr���h�m�F���Ă���
set FLB_ROOT=C:\localFLB
@rem ======================================================== 

@rem ���L��OpenRTM2.1.0�łŕύX�s�v
@rem OpenSSL��OpenRTM�����Ƀr���h�ς݃o�C�i�����_�E�����[�h���邽��
@rem ���L�o�[�W�����̎w�肪�K�{
set VC_VERSION=vc16
set OMNI_VERSION=4.3.2
set SSL_VC_VERSION=vc14
set SSL_VERSION=3.0.1

if exist %INSTALL_PREFIX% rmdir /s/q %INSTALL_PREFIX%

@rem �p�X���� "\" �� "/" �ɕϊ�����
set current_dir=%~dp0
set RTM_ROOT=%current_dir:\=/%
set BOOST_PATH=%BOOST_PATH:\=/%
set INSTALL_PREFIX=%INSTALL_PREFIX:\=/%
set FLB_ROOT=%FLB_ROOT:\=/%

@rem omniORB download
set base_omni_url="https://openrtm.org/pub/omniORB/win32/omniORB-%OMNI_VERSION%/"
set OMNIORB_DIR=omniORB-%OMNI_VERSION%-x64-%VC_VERSION%-py%PY_VERSION%
set OMNIORB_ZIP=%OMNIORB_DIR%.zip
set OMNIORB_URL=%base_omni_url%/%OMNIORB_ZIP%
if not exist %OMNIORB_ZIP% (
  powershell wget -O %OMNIORB_ZIP% %OMNIORB_URL%
  if exist %OMNIORB_DIR% rmdir /s/q %OMNIORB_DIR%
  powershell Expand-Archive .\%OMNIORB_ZIP% -DestinationPath .\
)
set OMNIORB_ROOT=%RTM_ROOT%/%OMNIORB_DIR%

@rem OpenSSL download
set base_ssl_url="https://openrtm.org/pub/OpenSSL/%SSL_VERSION%"
set OPENSSL_ZIP=openssl-%SSL_VERSION%-win64-%SSL_VC_VERSION%.zip
set OPENSSL_URL=%base_ssl_url%/%OPENSSL_ZIP%
if not exist %OPENSSL_ZIP% (
  powershell wget -O %OPENSSL_ZIP% %OPENSSL_URL%
  if exist OpenSSL rmdir /s/q OpenSSL
  powershell Expand-Archive .\%OPENSSL_ZIP% -DestinationPath .\
)
set SSL_ROOT=%RTM_ROOT%OpenSSL/build

@rem set cmake parameter
set CMAKE_OPT=-DRTM_VC_VER=%VC_VERSION% ^
 -DORB_ROOT=%OMNIORB_ROOT% ^
 -DCORBA=omniORB ^
 -DSSL_ENABLE=ON ^
 -DOPENSSL_ROOT=%SSL_ROOT% ^
 -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% ^
 -DWINDOWS_MSM_BUILD=ON ^
 -DBOOST_ROOT=%BOOST_PATH% ^
 -DBoost_USE_STATIC_LIBS=OFF ^
 -DHTTP_ENABLE=ON ^
 -DFLUENTBIT_ENABLE=ON ^
 -DFLUENTBIT_ROOT=%FLB_ROOT% ^
 -G %CMAKE_GENERATOR% ^
 -A x64 ..
call :CMAKE_Debug
call :CMAKE_Release
exit /b

:CMAKE_Release
if exist build-release rmdir /s/q build-release
mkdir build-release
cd build-release
cmake %CMAKE_OPT% 
cmake --build . --verbose --config Release
cmake --install .
cd ..
exit /b

:CMAKE_Debug
if exist build-debug rmdir /s/q build-debug
mkdir build-debug
cd build-debug
cmake %CMAKE_OPT%
cmake --build . --verbose --config Debug
cmake --install . --config Debug
cd ..
exit /b

