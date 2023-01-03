/*
 * test_misc.c
 *
 * Testing small helper functions.
 *
 * Copyright (C) 2011  Martin Albrecht <martinralbrecht@googlemail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "testing.h"
#include <m4ri/m4ri.h>
#include <stdarg.h>

#define b(n) (m4ri_one << (n))

int test_spread_and_shrink(const word to, const int length, ...) {
  word from = 0xFF;
  rci_t *Q  = (rci_t *)calloc(sizeof(rci_t), length);
  va_list l;
  va_start(l, length);
  for (size_t i = 0; i < length; i++) { Q[i] = va_arg(l, size_t); }

  const word res = m4ri_spread_bits(from, Q, length, 0);
  const word pre = m4ri_shrink_bits(res, Q, length, 0);

  free(Q);
  va_end(l);

  if (pre != (b(length) - 1)) { return 1; }
  if (res != to) { return 1; }
  return 0;
}

int test_png(rci_t m, rci_t n) {
  int ret = 0;
#if __M4RI_HAVE_LIBPNG
  printf("png: m: %4d, n: %4d", m, n);

  const char *fn = "test_misc__test_png.png";

  mzd_t *A = mzd_init(m, n);
  mzd_randomize(A);
  mzd_to_png(A, fn, 0, NULL, 0);
  mzd_t *B = mzd_from_png(fn, 0);

  ret += mzd_cmp(A, B);

  remove(fn);
  mzd_free(B);
  mzd_free(A);

  if (ret == 0) {
    printf(" ... passed\n");
  } else {
    printf(" ... FAILED\n");
  }
#endif
  return ret;
}

int test_submatrix(const rci_t m, const rci_t n, const rci_t lowr, const rci_t lowc,
                   const rci_t highr, const rci_t highc) {
  printf("submatrix: m: %4d, n: %4d, (%4d, %4d, %4d, %4d)", m, n, lowr, lowc, highr, highc);
  assert(highr - lowr > 0);
  assert(highc - lowc > 0);
  mzd_t *M = mzd_init(m, n);
  mzd_randomize(M);
  mzd_t *S = mzd_init(highr - lowr, highc - lowc);
  mzd_submatrix(S, M, lowr, lowc, highr, highc);
  int ret = 0;
  for (rci_t i = 0; i < highr - lowr; i++) {
    for (rci_t j = 0; j < highc - lowc; j++) {
      ret += (mzd_read_bit(M, lowr + i, lowc + j) ^ mzd_read_bit(S, i, j));
    }
  }

  mzd_free(M);
  mzd_free(S);

  if (ret == 0) {
    printf(" ... passed\n");
  } else {
    printf(" ... FAILED\n");
  }
  return ret;
}

int test_extract(const rci_t m, const rci_t n) {
  int ret  = 0;
  mzd_t *A = mzd_init(m, n);
  mzd_randomize(A);
  mzd_t *L0 = mzd_extract_l(NULL, A);
  mzd_t *L1 = mzd_init(MIN(m, n), MIN(m, n));
  mzd_extract_l(L1, A);

  if (mzd_equal(L0, L1) != TRUE) { ret += 1; }

  mzd_t *U0 = mzd_extract_u(NULL, A);
  mzd_t *U1 = mzd_init(MIN(m, n), MIN(m, n));
  mzd_extract_u(U1, A);

  if (mzd_equal(U0, U1) != TRUE) { ret += 1; }

  mzd_free(U1);
  mzd_free(U0);
  mzd_free(L1);
  mzd_free(L0);
  mzd_free(A);
  return ret;
}

int main(int argc, char *argv[]) {
  int status = 0;

  status += test_spread_and_shrink(b(1) | b(0), 2, 0, 1);
  status += test_spread_and_shrink(b(2) | b(0), 2, 0, 2);
  status += test_spread_and_shrink(b(3) | b(1), 2, 1, 3);
  status += test_spread_and_shrink(b(3) | b(2), 2, 2, 3);
  status += test_spread_and_shrink(b(2) | b(1) | b(0), 3, 0, 1, 2);
  status += test_spread_and_shrink(b(3) | b(2) | b(0), 3, 0, 2, 3);
  status += test_spread_and_shrink(b(4) | b(3) | b(1), 3, 1, 3, 4);
  status += test_spread_and_shrink(b(5) | b(3) | b(2), 3, 2, 3, 5);

  status += test_png(1, 1);
  status += test_png(16, 15);
  status += test_png(32, 32);
  status += test_png(63, 63);
  status += test_png(64, 64);
  status += test_png(113, 114);
  status += test_png(125, 102);
  status += test_png(126, 12);
  status += test_png(128, 200);

  status += test_submatrix(2, 127, 1, 1, 2, 127);
  status += test_submatrix(2, 128, 1, 1, 2, 128);
  status += test_submatrix(2, 129, 1, 1, 2, 129);
  status += test_submatrix(2, 130, 1, 1, 2, 130);
  status += test_submatrix(2, 131, 1, 1, 2, 131);

  status += test_submatrix(2, 63, 1, 1, 2, 63);
  status += test_submatrix(2, 64, 1, 1, 2, 64);
  status += test_submatrix(2, 65, 1, 1, 2, 65);
  status += test_submatrix(2, 66, 1, 1, 2, 66);
  status += test_submatrix(2, 67, 1, 1, 2, 67);

  status += test_submatrix(2, 127, 1, 63, 2, 127);
  status += test_submatrix(2, 128, 1, 64, 2, 128);
  status += test_submatrix(2, 129, 1, 65, 2, 129);
  status += test_submatrix(2, 130, 1, 66, 2, 130);

  status += test_extract(64, 64);
  status += test_extract(63, 65);
  status += test_extract(65, 63);

  if (!status) {
    printf("All tests passed.\n");
  } else {
    printf("TEST FAILED!\n");
    return 1;
  }
  return 0;
}
