/* siv-cmac-test.c

   Self-test and vectors for AES-SIV-CMAC mode ciphers

   Copyright (C) 2018 Nikos Mavrogiannopoulos

   This file is part of GNU Nettle.

   GNU Nettle is free software: you can redistribute it and/or
   modify it under the terms of either:

     * the GNU Lesser General Public License as published by the Free
       Software Foundation; either version 3 of the License, or (at your
       option) any later version.

   or

     * the GNU General Public License as published by the Free
       Software Foundation; either version 2 of the License, or (at your
       option) any later version.

   or both in parallel, as here.

   GNU Nettle is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received copies of the GNU General Public License and
   the GNU Lesser General Public License along with this program.  If
   not, see http://www.gnu.org/licenses/.
*/

#include "testutils.h"
#include "aes.h"
#include "nettle-types.h"
#include "siv-cmac.h"

static const struct nettle_aead_message
siv_cmac_aes128 = {
  "siv_cmac_aes128",
  sizeof(struct siv_cmac_aes128_ctx),
  SIV_CMAC_AES128_KEY_SIZE,
  SIV_DIGEST_SIZE,
  0, /* No in-place operation. */
  (nettle_set_key_func*) siv_cmac_aes128_set_key,
  (nettle_set_key_func*) siv_cmac_aes128_set_key,
  (nettle_encrypt_message_func*) siv_cmac_aes128_encrypt_message,
  (nettle_decrypt_message_func*) siv_cmac_aes128_decrypt_message,
};

static const struct nettle_aead_message
siv_cmac_aes256 = {
  "siv_cmac_aes256",
  sizeof(struct siv_cmac_aes256_ctx),
  SIV_CMAC_AES256_KEY_SIZE,
  SIV_DIGEST_SIZE,
  0, /* No in-place operation. */
  (nettle_set_key_func*) siv_cmac_aes256_set_key,
  (nettle_set_key_func*) siv_cmac_aes256_set_key,
  (nettle_encrypt_message_func*) siv_cmac_aes256_encrypt_message,
  (nettle_decrypt_message_func*) siv_cmac_aes256_decrypt_message,
};

void
test_main(void)
{
  /* The following tests were checked for interoperability against libaes_siv */

  /*
   * Example with small nonce, no AD and no plaintext
   */
  test_aead_message(&siv_cmac_aes128,
		  SHEX("fffefdfc fbfaf9f8 f7f6f5f4 f3f2f1f0"
		       "f0f1f2f3 f4f5f6f7 f8f9fafb fcfdfeff"),
		  SHEX("01"),
		  SHEX(""),
		  SHEX(""),
		  SHEX("c696f84f df92aba3 c31c23d5 f2087513"));
  /*
   * Example with small nonce, no AD and plaintext
   */
  test_aead_message(&siv_cmac_aes128,
		  SHEX("fffefdfc fbfaf9f8 f7f6f5f4 f3f2f1f0"
		       "f0f1f2f3 f4f5f6f7 f8f9fafb fcfdfeff"),
		  SHEX("02"),
		  SHEX(""),
		  SHEX("00112233 44556677 8899aabb ccddeeff"),
		  SHEX("5027b101 589747b8 865a9790 d3fd51d7"
		       "1f259d40 5bfa260b 9ba1d60a a287fd0b"));

  /*
   * Example with length < 16
   */
  test_aead_message(&siv_cmac_aes128,
		  SHEX("fffefdfc fbfaf9f8 f7f6f5f4 f3f2f1f0"
		       "f0f1f2f3 f4f5f6f7 f8f9fafb fcfdfeff"),
		  SHEX("02"),
		  SHEX("10111213 14151617 18191a1b 1c1d1e1f"
		       "20212223 24252627"),
		  SHEX("11223344 55667788 99aabbcc ddee"),
		  SHEX("7300cd9b 3f514a44 ed660db6 14157f59"
		       "f0382e23 ae0e6e62 27a03dd3 2619"));

  /*
   * Example with length > 16
   */
  test_aead_message(&siv_cmac_aes128,
		  SHEX("7f7e7d7c 7b7a7978 77767574 73727170"
		       "40414243 44454647 48494a4b 4c4d4e4f"),
		  SHEX("020304"),
		  SHEX("00112233 44556677 8899aabb ccddeeff"
		       "deaddada deaddada ffeeddcc bbaa9988"
		       "77665544 33221100"),
		  SHEX("74686973 20697320 736f6d65 20706c61"
		       "696e7465 78742074 6f20656e 63727970"
		       "74207573 696e6720 5349562d 414553"),
		  SHEX("f1dba33d e5b3369e 883f67b6 fc823cee"
		       "a4ffb87f dba97c89 44a62325 f133b4e0"
		       "1ca55276 e2261c1a 1d1d4248 d1da30ba"
		       "52b9c8d7 955d65c8 d2ce6eb7 e367d0"));

  /*
   * Example with single AAD, length > 16
   */
  test_aead_message(&siv_cmac_aes128,
		  SHEX("7f7e7d7c 7b7a7978 77767574 73727170"
		       "40414243 44454647 48494a4b 4c4d4e4f"),
		  SHEX("09f91102 9d74e35b d84156c5 635688c0"),
		  SHEX("00112233 44556677 8899aabb ccddeeff"
		       "deaddada deaddada ffeeddcc bbaa9988"
		       "77665544 33221100"),
		  SHEX("74686973 20697320 736f6d65 20706c61"
		       "696e7465 78742074 6f20656e 63727970"
		       "74207573 696e6720 5349562d 414553"),
		  SHEX("85825e22 e90cf2dd da2c548d c7c1b631"
		       "0dcdaca0 cebf9dc6 cb90583f 5bf1506e"
		       "02cd4883 2b00e4e5 98b2b22a 53e6199d"
		       "4df0c166 6a35a043 3b250dc1 34d776"));

  /*
   * Example with single AAD, length < 16
   */
  test_aead_message(&siv_cmac_aes128,
		  SHEX("7f7e7d7c 7b7a7978 77767574 73727170"
		       "40414243 44454647 48494a4b 4c4d4e4f"),
		  SHEX("09f91102 9d74e35b d84156c5 635688c0"),
		  SHEX("00112233 44556677 8899aabb ccddeeff"
		       "deaddada deaddada ffeeddcc bbaa9988"
		       "77665544 33221100"),
		  SHEX("11223344 55667788 99aabbcc ddee"),
		  SHEX("15f83882 14bdc94e 3ec4c7c3 69863746"
		       "cd72d317 4b20a1e4 a0894fb7 cd78"));

  /* AES-SIV-CMAC-512 (AES-256) from dchest/siv repo
   */
  test_aead_message(&siv_cmac_aes256,
		  SHEX("fffefdfc fbfaf9f8 f7f6f5f4 f3f2f1f0"
		       "6f6e6d6c 6b6a6968 67666564 63626160"
		       "f0f1f2f3 f4f5f6f7 f8f9fafb fcfdfeff"
		       "00010203 04050607 08090a0b 0c0d0e0f"),
		  SHEX("02"),
		  SHEX("10111213 14151617 18191a1b 1c1d1e1f"
		       "20212223 24252627"),
		  SHEX("11223344 55667788 99aabbcc ddee"),
		  SHEX("6f740b42 1e2972d8 5e76189e 99842843"
		       "ad9e6ff1 4ea97c32 ab315e67 464c"));


  /* AES-SIV-CMAC-512 (AES-256)
   */
  test_aead_message(&siv_cmac_aes256,
		  SHEX("c27df2fd aec35d4a 2a412a50 c3e8c47d"
		       "2d568e91 a38e5414 8abdc0b6 e86caf87"
		       "695c0a8a df4c5f8e b2c6c8b1 36529864"
		       "f3b84b3a e8e3676c e760c461 f3a13e83"),
		  SHEX("02"),
		  SHEX("10111213 14151617 18191a1b 1c1d1e1f"
		       "20212223 24252627"),
		  SHEX("11223344 55667788 99aabbcc ddee"),
		  SHEX("c3366ef8 92911eac 3d17f29a 37d4ebad"
		       "ddc1219e bbde06d1 ee893e55 a39f"));

  /*
   * Example with length > 16
   */
  test_aead_message(&siv_cmac_aes256,
		  SHEX("c27df2fd aec35d4a 2a412a50 c3e8c47d"
		       "2d568e91 a38e5414 8abdc0b6 e86caf87"
		       "695c0a8a df4c5f8e b2c6c8b1 36529864"
		       "f3b84b3a e8e3676c e760c461 f3a13e83"),
		  SHEX("02"),
		  SHEX("00112233 44556677 8899aabb ccddeeff"
		       "deaddada deaddada ffeeddcc bbaa9988"
		       "77665544 33221100"),
		  SHEX("74686973 20697320 736f6d65 20706c61"
		       "696e7465 78742074 6f20656e 63727970"
		       "74207573 696e6720 5349562d 414553"),
		  SHEX("bbe4751a 549d2fce 410c2efd e0df4d13"
		       "1a6eac0d 030028f8 dc16b6c4 3a557d4e"
		       "3e846ad7 52c5a030 c75a85ff 8b07ff10"
		       "71b133f5 edac3c60 8bb6eb13 dd1fd9"));

  /*
   * Example with single AAD, length > 16
   */
  test_aead_message(&siv_cmac_aes256,
		  SHEX("c27df2fd aec35d4a 2a412a50 c3e8c47d"
		       "2d568e91 a38e5414 8abdc0b6 e86caf87"
		       "695c0a8a df4c5f8e b2c6c8b1 36529864"
		       "f3b84b3a e8e3676c e760c461 f3a13e83"),
		  SHEX("09f91102 9d74e35b d84156c5 635688c0"),
		  SHEX("00112233 44556677 8899aabb ccddeeff"
		       "deaddada deaddada ffeeddcc bbaa9988"
		       "77665544 33221100"),
		  SHEX("74686973 20697320 736f6d65 20706c61"
		       "696e7465 78742074 6f20656e 63727970"
		       "74207573 696e6720 5349562d 414553"),
		  SHEX("5a979b0d a58fde80 51621ae6 bf96feda"
		       "50933da8 047bc306 fabaf0c3 d9fa8471"
		       "c70a7def 39a2f91d 68a2021c 99ac7e2a"
		       "24535a13 4ba23ec1 5787cebe 5c53cc"));

  /* The following tests were checked for interoperability against miscreant.js */

  /*
   * Example from miscreant.js with no AD
   * https://github.com/miscreant/miscreant.js/blob/master/vectors/aes_siv_aead.tjson
   */
  test_aead_message(&siv_cmac_aes128,
		  SHEX("fffefdfc fbfaf9f8 f7f6f5f4 f3f2f1f0"
		       "f0f1f2f3 f4f5f6f7 f8f9fafb fcfdfeff"),
		  SHEX("10111213 1415161718191a1b1 c1d1e1f2"
		       "02122232 4252627"),
		  SHEX(""),
		  SHEX("11223344 55667788 99aabbcc ddee"),
		  SHEX("4b3d0f15 ae9ffa9e 65b94942 1582ef70"
		       "e410910d 6446c775 9ebff9b5 385a"));

  /*
   * Example from miscreant.js with AD
   */
  test_aead_message(&siv_cmac_aes128,
		  SHEX("7f7e7d7c 7b7a7978 77767574 73727170"
		       "40414243 44454647 48494a4b 4c4d4e4f"),
		  SHEX("09f91102 9d74e35b d84156c5 635688c0"),
		  SHEX("00112233 44556677 8899aabb ccddeeff"
		       "deaddada deaddada ffeeddcc bbaa9988"
		       "77665544 33221100"),
		  SHEX("74686973 20697320 736f6d65 20706c61"
		       "696e7465 78742074 6f20656e 63727970"
		       "74207573 696e6720 5349562d 414553"),
		  SHEX("85825e22 e90cf2dd da2c548d c7c1b631"
		       "0dcdaca0 cebf9dc6 cb90583f 5bf1506e"
		       "02cd4883 2b00e4e5 98b2b22a 53e6199d"
		       "4df0c166 6a35a043 3b250dc1 34d776"));
}
