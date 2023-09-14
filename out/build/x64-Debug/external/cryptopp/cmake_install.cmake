# Install script for directory: C:/Users/Administrator/Desktop/core/external/cryptopp

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Users/Administrator/Desktop/core/out/install/x64-Debug")
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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/Administrator/Desktop/core/out/build/x64-Debug/external/cryptopp/cryptopp-static.lib")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/cryptopp" TYPE FILE FILES
    "C:/Users/Administrator/Desktop/core/external/cryptopp/3way.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/adler32.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/adv_simd.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/aes.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/aes_armv4.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/algebra.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/algparam.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/arc4.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/argnames.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/aria.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/arm_simd.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/asn.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/authenc.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/base32.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/base64.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/basecode.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/blake2.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/blowfish.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/blumshub.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/camellia.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/cast.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/cbcmac.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/ccm.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/chacha.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/chachapoly.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/cham.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/channels.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/cmac.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/config.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/cpu.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/crc.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/cryptlib.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/darn.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/default.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/des.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/dh.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/dh2.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/dll.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/dmac.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/donna.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/donna_32.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/donna_64.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/donna_sse.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/drbg.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/dsa.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/eax.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/ec2n.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/eccrypto.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/ecp.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/ecpoint.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/elgamal.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/emsa2.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/eprecomp.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/esign.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/factory.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/fhmqv.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/files.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/filters.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/fips140.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/fltrimpl.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/gcm.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/gf256.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/gf2_32.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/gf2n.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/gfpcrypt.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/gost.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/gzip.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/hashfwd.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/hc128.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/hc256.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/hex.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/hight.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/hkdf.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/hmac.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/hmqv.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/hrtimer.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/ida.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/idea.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/integer.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/iterhash.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/kalyna.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/keccak.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/lea.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/lubyrack.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/luc.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/mars.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/md2.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/md4.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/md5.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/mdc.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/mersenne.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/misc.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/modarith.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/modes.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/modexppc.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/mqueue.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/mqv.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/naclite.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/nbtheory.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/nr.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/oaep.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/oids.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/osrng.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/ossig.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/padlkrng.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/panama.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/pch.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/pkcspad.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/poly1305.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/polynomi.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/ppc_simd.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/pssr.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/pubkey.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/pwdbased.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/queue.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/rabbit.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/rabin.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/randpool.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/rc2.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/rc5.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/rc6.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/rdrand.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/resource.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/rijndael.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/ripemd.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/rng.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/rsa.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/rw.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/safer.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/salsa.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/scrypt.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/seal.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/secblock.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/seckey.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/seed.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/serpent.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/serpentp.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/sha.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/sha3.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/shacal2.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/shake.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/shark.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/simeck.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/simon.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/simple.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/siphash.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/skipjack.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/sm3.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/sm4.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/smartptr.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/sosemanuk.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/speck.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/square.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/stdcpp.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/strciphr.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/tea.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/threefish.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/tiger.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/trap.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/trunhash.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/ttmac.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/tweetnacl.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/twofish.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/vmac.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/wake.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/whrlpool.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/words.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/xed25519.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/xtr.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/xtrcrypt.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/zdeflate.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/zinflate.h"
    "C:/Users/Administrator/Desktop/core/external/cryptopp/zlib.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/cryptopp" TYPE FILE FILES
    "C:/Users/Administrator/Desktop/core/external/cryptopp/cryptopp-config.cmake"
    "C:/Users/Administrator/Desktop/core/out/build/x64-Debug/external/cryptopp/cryptopp-config-version.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/cryptopp/cryptopp-targets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/cryptopp/cryptopp-targets.cmake"
         "C:/Users/Administrator/Desktop/core/out/build/x64-Debug/external/cryptopp/CMakeFiles/Export/lib/cmake/cryptopp/cryptopp-targets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/cryptopp/cryptopp-targets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/cryptopp/cryptopp-targets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/cryptopp" TYPE FILE FILES "C:/Users/Administrator/Desktop/core/out/build/x64-Debug/external/cryptopp/CMakeFiles/Export/lib/cmake/cryptopp/cryptopp-targets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/cryptopp" TYPE FILE FILES "C:/Users/Administrator/Desktop/core/out/build/x64-Debug/external/cryptopp/CMakeFiles/Export/lib/cmake/cryptopp/cryptopp-targets-debug.cmake")
  endif()
endif()

