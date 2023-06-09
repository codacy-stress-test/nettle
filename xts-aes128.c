/* xts-aes128.c

   XTS Mode using AES128 as the underlying cipher.

   Copyright (C) 2018 Red Hat, Inc.

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

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <assert.h>

#include "aes.h"
#include "xts.h"


void
xts_aes128_set_encrypt_key(struct xts_aes128_key *xts_key, const uint8_t *key)
{
    aes128_set_encrypt_key(&xts_key->cipher, key);
    aes128_set_encrypt_key(&xts_key->tweak_cipher, &key[AES128_KEY_SIZE]);
}

void
xts_aes128_set_decrypt_key(struct xts_aes128_key *xts_key, const uint8_t *key)
{
    aes128_set_decrypt_key(&xts_key->cipher, key);
    aes128_set_encrypt_key(&xts_key->tweak_cipher, &key[AES128_KEY_SIZE]);
}

void
xts_aes128_encrypt_message(const struct xts_aes128_key *xts_key,
                           const uint8_t *tweak, size_t length,
                           uint8_t *dst, const uint8_t *src)
{
    xts_encrypt_message(&xts_key->cipher, &xts_key->tweak_cipher,
                        (nettle_cipher_func *) aes128_encrypt,
                        tweak, length, dst, src);
}

void
xts_aes128_decrypt_message(const struct xts_aes128_key *xts_key,
                           const uint8_t *tweak, size_t length,
                           uint8_t *dst, const uint8_t *src)
{
    xts_decrypt_message(&xts_key->cipher, &xts_key->tweak_cipher,
                        (nettle_cipher_func *) aes128_decrypt,
                        (nettle_cipher_func *) aes128_encrypt,
                        tweak, length, dst, src);
}
