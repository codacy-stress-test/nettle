#include "testutils.h"

#include "nettle-write.h"
#include "sha2.h"

/* Test compression only. */
static void
test_sha256_compress(const struct tstring *input,
		     const struct tstring *expected) {
  size_t split;
  ASSERT (input->length % SHA256_BLOCK_SIZE == 0);
  ASSERT (expected->length == SHA256_DIGEST_SIZE);

  for (split = 0; split <= input->length; split += SHA256_BLOCK_SIZE)
    {
      struct sha256_ctx ctx;
      uint8_t digest[SHA256_DIGEST_SIZE];

      sha256_init (&ctx);
      sha256_update (&ctx, split, input->data);
      sha256_update (&ctx, input->length - split, input->data + split);

      _nettle_write_be32 (SHA256_DIGEST_SIZE, digest, ctx.state);
      if (!MEMEQ (SHA256_DIGEST_SIZE, digest, expected->data)) {
	fprintf (stderr, "sha256_compress failed: length %u, split %u\nInput:",
		 (unsigned) input->length, (unsigned) split);
	tstring_print_hex (input);
	fprintf (stderr, "\nOutput: ");
	print_hex (SHA256_DIGEST_SIZE, digest);
	fprintf(stdout, "\nExpected:\n");
	tstring_print_hex (expected);
	fprintf (stderr, "\n");
	abort ();
      }
    }
}

void
test_main(void)
{
  /* Initial state */
  test_sha256_compress (SDATA(""),
			SHEX("6a09e667 bb67ae85 3c6ef372 a54ff53a"
			     "510e527f 9b05688c 1f83d9ab 5be0cd19"));

  /* Single block compressed */
  test_sha256_compress (SDATA("0123456789abcdefghijklmnopqrstuv"
			      "wxyzABCDEFGHIJKLMNOPQRSTUVWXYZZY"),
			SHEX("932558b453a68273 4daf0008efb6b5e5"
			     "32808baaf92bc749 2ac377107618ac67"));
  /* Two blocks compressed */
  test_sha256_compress (SDATA("0123456789abcdefghijklmnopqrstuv"
			      "wxyzABCDEFGHIJKLMNOPQRSTUVWXYZZY"
			      "ABCDEFGHIJKLMNOPQRSTUVWXYZ012345"
			      "6789abcdefghijklmnopqrstuvwxyzzy"),
			SHEX("d82038b1732bbe97 94b879b41f98e9fc"
			     "2777fd8ab76737f5 60919c4fe1366c8e"));

  /* From FIPS180-2 */
  test_hash(&nettle_sha256, SDATA("abc"),
	    SHEX("ba7816bf8f01cfea 414140de5dae2223"
		 "b00361a396177a9c b410ff61f20015ad"));

  test_hash(&nettle_sha256,
	    SDATA("abcdbcdecdefdefgefghfghighij"
		  "hijkijkljklmklmnlmnomnopnopq"),
	    SHEX("248d6a61d20638b8 e5c026930c3e6039"
		 "a33ce45964ff2167 f6ecedd419db06c1"));

  test_hash(&nettle_sha256,
	    SDATA("abcdefghbcdefghicdefghijdefg"
		  "hijkefghijklfghijklmghijklmn"
		  "hijklmnoijklmnopjklmnopqklmn"
		  "opqrlmnopqrsmnopqrstnopqrstu"),
	    SHEX("cf5b16a778af8380 036ce59e7b049237"
		 "0b249b11e8f07a51 afac45037afee9d1"));

  /* Additional test vectors, from Daniel Kahn Gillmor */
  test_hash(&nettle_sha256, SDATA(""),
	    SHEX("e3b0c44298fc1c14 9afbf4c8996fb924"
		 "27ae41e4649b934c a495991b7852b855"));
  test_hash(&nettle_sha256, SDATA("a"),
	    SHEX("ca978112ca1bbdca fac231b39a23dc4d"
		 "a786eff8147c4e72 b9807785afee48bb"));
  test_hash(&nettle_sha256, SDATA("38"),
	    SHEX("aea92132c4cbeb26 3e6ac2bf6c183b5d"
		 "81737f179f21efdc 5863739672f0f470"));
  test_hash(&nettle_sha256, SDATA("message digest"),
	    SHEX("f7846f55cf23e14e ebeab5b4e1550cad"
		 "5b509e3348fbc4ef a3a1413d393cb650"));
  test_hash(&nettle_sha256, SDATA("abcdefghijklmnopqrstuvwxyz"),
	    SHEX("71c480df93d6ae2f 1efad1447c66c952"
		 "5e316218cf51fc8d 9ed832f2daf18b73"));
  test_hash(&nettle_sha256,
	    SDATA("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef"
		  "ghijklmnopqrstuvwxyz0123456789"),
	    SHEX("db4bfcbd4da0cd85 a60c3c37d3fbd880"
		 "5c77f15fc6b1fdfe 614ee0a7c8fdb4c0"));
  test_hash(&nettle_sha256,
	    SDATA("12345678901234567890123456789012"
		  "34567890123456789012345678901234"
		  "5678901234567890"),
	    SHEX("f371bc4a311f2b00 9eef952dd83ca80e"
		 "2b60026c8e935592 d0f9c308453c813e"));
}

/* These are intermediate values for the single sha1_compress call
   that results from the first testcase, SHA256("abc"). Each row are
   the values for A, B, C, D, E, F, G, H after the i:th round. The row
   i = -1 gives the initial values, and i = 99 gives the output
   values.
   
-1: 6a09e667 bb67ae85 3c6ef372 a54ff53a 510e527f 9b05688c 1f83d9ab 5be0cd19
 0: 6a09e667 bb67ae85 3c6ef372 fa2a4622 510e527f 9b05688c 1f83d9ab 5d6aebcd
 1: 6a09e667 bb67ae85 78ce7989 fa2a4622 510e527f 9b05688c 5a6ad9ad 5d6aebcd
 6: 24e00850 e5030380 2b4209f5  4409a6a d550f666 9b27a401 714260ad 43ada245
 7: 85a07b5f e5030380 2b4209f5  4409a6a  c657a79 9b27a401 714260ad 43ada245
 8: 85a07b5f e5030380 2b4209f5 32ca2d8c  c657a79 9b27a401 714260ad 8e04ecb9
 9: 85a07b5f e5030380 1cc92596 32ca2d8c  c657a79 9b27a401 8c87346b 8e04ecb9
14: 816fd6e9 c0645fde d932eb16 87912990 f71fc5a9  b92f20c 745a48de 1e578218
15: b0fa238e c0645fde d932eb16 87912990  7590dcd  b92f20c 745a48de 1e578218
16: b0fa238e c0645fde d932eb16 8034229c  7590dcd  b92f20c 745a48de 21da9a9b
17: b0fa238e c0645fde 846ee454 8034229c  7590dcd  b92f20c c2fbd9d1 21da9a9b
18: b0fa238e cc899961 846ee454 8034229c  7590dcd fe777bbf c2fbd9d1 21da9a9b
19: b0638179 cc899961 846ee454 8034229c e1f20c33 fe777bbf c2fbd9d1 21da9a9b
20: b0638179 cc899961 846ee454 9dc68b63 e1f20c33 fe777bbf c2fbd9d1 8ada8930
21: b0638179 cc899961 c2606d6d 9dc68b63 e1f20c33 fe777bbf e1257970 8ada8930
22: b0638179 a7a3623f c2606d6d 9dc68b63 e1f20c33 49f5114a e1257970 8ada8930
23: c5d53d8d a7a3623f c2606d6d 9dc68b63 aa47c347 49f5114a e1257970 8ada8930
24: c5d53d8d a7a3623f c2606d6d 2823ef91 aa47c347 49f5114a e1257970 1c2c2838
25: c5d53d8d a7a3623f 14383d8e 2823ef91 aa47c347 49f5114a cde8037d 1c2c2838
26: c5d53d8d c74c6516 14383d8e 2823ef91 aa47c347 b62ec4bc cde8037d 1c2c2838
27: edffbff8 c74c6516 14383d8e 2823ef91 77d37528 b62ec4bc cde8037d 1c2c2838
28: edffbff8 c74c6516 14383d8e 363482c9 77d37528 b62ec4bc cde8037d 6112a3b7
29: edffbff8 c74c6516 a0060b30 363482c9 77d37528 b62ec4bc ade79437 6112a3b7
30: edffbff8 ea992a22 a0060b30 363482c9 77d37528  109ab3a ade79437 6112a3b7
31: 73b33bf5 ea992a22 a0060b30 363482c9 ba591112  109ab3a ade79437 6112a3b7
32: 73b33bf5 ea992a22 a0060b30 9cd9f5f6 ba591112  109ab3a ade79437 98e12507
33: 73b33bf5 ea992a22 59249dd3 9cd9f5f6 ba591112  109ab3a fe604df5 98e12507
34: 73b33bf5  85f3833 59249dd3 9cd9f5f6 ba591112 a9a7738c fe604df5 98e12507
35: f4b002d6  85f3833 59249dd3 9cd9f5f6 65a0cfe4 a9a7738c fe604df5 98e12507
36: f4b002d6  85f3833 59249dd3 41a65cb1 65a0cfe4 a9a7738c fe604df5  772a26b
37: f4b002d6  85f3833 34df1604 41a65cb1 65a0cfe4 a9a7738c a507a53d  772a26b
38: f4b002d6 6dc57a8a 34df1604 41a65cb1 65a0cfe4 f0781bc8 a507a53d  772a26b
39: 79ea687a 6dc57a8a 34df1604 41a65cb1 1efbc0a0 f0781bc8 a507a53d  772a26b
40: 79ea687a 6dc57a8a 34df1604 26352d63 1efbc0a0 f0781bc8 a507a53d d6670766
41: 79ea687a 6dc57a8a 838b2711 26352d63 1efbc0a0 f0781bc8 df46652f d6670766
42: 79ea687a decd4715 838b2711 26352d63 1efbc0a0 17aa0dfe df46652f d6670766
43: fda24c2e decd4715 838b2711 26352d63 9d4baf93 17aa0dfe df46652f d6670766
44: fda24c2e decd4715 838b2711 26628815 9d4baf93 17aa0dfe df46652f a80f11f0
45: fda24c2e decd4715 72ab4b91 26628815 9d4baf93 17aa0dfe b7755da1 a80f11f0
46: fda24c2e a14c14b0 72ab4b91 26628815 9d4baf93 d57b94a9 b7755da1 a80f11f0
47: 4172328d a14c14b0 72ab4b91 26628815 fecf0bc6 d57b94a9 b7755da1 a80f11f0
48: 4172328d a14c14b0 72ab4b91 bd714038 fecf0bc6 d57b94a9 b7755da1  5757ceb
49: 4172328d a14c14b0 6e5c390c bd714038 fecf0bc6 d57b94a9 f11bfaa8  5757ceb
50: 4172328d 52f1ccf7 6e5c390c bd714038 fecf0bc6 7a0508a1 f11bfaa8  5757ceb
51: 49231c1e 52f1ccf7 6e5c390c bd714038 886e7a22 7a0508a1 f11bfaa8  5757ceb
52: 49231c1e 52f1ccf7 6e5c390c 101fd28f 886e7a22 7a0508a1 f11bfaa8 529e7d00
53: 49231c1e 52f1ccf7 f5702fdb 101fd28f 886e7a22 7a0508a1 9f4787c3 529e7d00
54: 49231c1e 3ec45cdb f5702fdb 101fd28f 886e7a22 e50e1b4f 9f4787c3 529e7d00
55: 38cc9913 3ec45cdb f5702fdb 101fd28f 54cb266b e50e1b4f 9f4787c3 529e7d00
56: 38cc9913 3ec45cdb f5702fdb 9b5e906c 54cb266b e50e1b4f 9f4787c3 fcd1887b
57: 38cc9913 3ec45cdb 7e44008e 9b5e906c 54cb266b e50e1b4f c062d46f fcd1887b
58: 38cc9913 6d83bfc6 7e44008e 9b5e906c 54cb266b ffb70472 c062d46f fcd1887b
59: b21bad3d 6d83bfc6 7e44008e 9b5e906c b6ae8fff ffb70472 c062d46f fcd1887b
60: b21bad3d 6d83bfc6 7e44008e b85e2ce9 b6ae8fff ffb70472 c062d46f 961f4894
61: b21bad3d 6d83bfc6  4d24d6c b85e2ce9 b6ae8fff ffb70472 948d25b6 961f4894
62: b21bad3d d39a2165  4d24d6c b85e2ce9 b6ae8fff fb121210 948d25b6 961f4894
63: 506e3058 d39a2165  4d24d6c b85e2ce9 5ef50f24 fb121210 948d25b6 961f4894
99: ba7816bf 8f01cfea 414140de 5dae2223 b00361a3 96177a9c b410ff61 f20015ad
*/
