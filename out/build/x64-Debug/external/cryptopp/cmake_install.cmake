# Install script for directory: C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/out/install/x64-Debug")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/Strawberry/c/bin/objdump.exe")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/out/build/x64-Debug/external/cryptopp/cryptopp-static.lib")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/cryptopp" TYPE FILE FILES
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/3way.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/adler32.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/adv-simd.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/aes.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/algebra.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/algparam.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/arc4.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/argnames.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/aria.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/asn.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/authenc.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/base32.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/base64.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/basecode.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/blake2.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/blowfish.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/blumshub.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/camellia.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/cast.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/cbcmac.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/ccm.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/chacha.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/channels.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/cmac.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/config.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/cpu.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/crc.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/cryptlib.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/default.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/des.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/dh.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/dh2.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/dll.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/dmac.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/drbg.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/dsa.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/eax.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/ec2n.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/eccrypto.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/ecp.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/ecpoint.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/elgamal.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/emsa2.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/eprecomp.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/esign.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/factory.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/fhmqv.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/files.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/filters.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/fips140.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/fltrimpl.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/gcm.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/gf256.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/gf2_32.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/gf2n.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/gfpcrypt.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/gost.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/gzip.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/hashfwd.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/hex.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/hkdf.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/hmac.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/hmqv.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/hrtimer.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/ida.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/idea.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/integer.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/iterhash.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/kalyna.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/keccak.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/lubyrack.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/luc.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/mars.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/md2.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/md4.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/md5.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/mdc.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/mersenne.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/misc.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/modarith.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/modes.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/modexppc.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/mqueue.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/mqv.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/naclite.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/nbtheory.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/network.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/nr.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/oaep.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/oids.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/osrng.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/ossig.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/padlkrng.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/panama.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/pch.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/pkcspad.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/poly1305.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/polynomi.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/ppc-simd.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/pssr.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/pubkey.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/pwdbased.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/queue.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/rabin.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/randpool.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/rc2.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/rc5.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/rc6.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/rdrand.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/resource.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/rijndael.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/ripemd.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/rng.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/rsa.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/rw.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/safer.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/salsa.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/scrypt.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/seal.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/secblock.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/seckey.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/seed.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/serpent.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/serpentp.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/sha.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/sha3.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/shacal2.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/shark.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/simon.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/simple.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/siphash.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/skipjack.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/sm3.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/sm4.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/smartptr.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/socketft.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/sosemanuk.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/speck.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/square.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/stdcpp.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/strciphr.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/tea.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/threefish.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/tiger.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/trap.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/trdlocal.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/trunhash.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/ttmac.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/tweetnacl.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/twofish.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/vmac.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/wait.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/wake.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/whrlpool.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/winpipes.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/words.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/xtr.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/xtrcrypt.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/zdeflate.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/zinflate.h"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/zlib.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/cryptopp" TYPE FILE FILES
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/external/cryptopp/cryptopp-config.cmake"
    "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/out/build/x64-Debug/external/cryptopp/cryptopp-config-version.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/cryptopp/cryptopp-targets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/cryptopp/cryptopp-targets.cmake"
         "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/out/build/x64-Debug/external/cryptopp/CMakeFiles/Export/lib/cmake/cryptopp/cryptopp-targets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/cryptopp/cryptopp-targets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/cryptopp/cryptopp-targets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/cryptopp" TYPE FILE FILES "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/out/build/x64-Debug/external/cryptopp/CMakeFiles/Export/lib/cmake/cryptopp/cryptopp-targets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/cryptopp" TYPE FILE FILES "C:/Users/Administrator/Desktop/nynynyny/Ekrone_core-main/out/build/x64-Debug/external/cryptopp/CMakeFiles/Export/lib/cmake/cryptopp/cryptopp-targets-debug.cmake")
  endif()
endif()

