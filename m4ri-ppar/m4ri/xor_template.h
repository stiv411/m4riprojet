#include <m4ri/m4ri_config.h>
#include <m4ri/misc.h>

/**
 * Compute c[i] += sum(t[j][i], 0 <= j < N) for 0 <= i < wide
 */

static inline void __M4RI_TEMPLATE_NAME(_mzd_combine)(word *m, word const *t[N], wi_t wide) {
  assert(1 <= N && N <= 8);

  for (wi_t i = 0; i < wide; i++) {
    switch (N) { /* we rely on the compiler to optimise this switch away, it reads nicer than #if */
    case 8:
      *m++ ^= *t[0]++ ^ *t[1]++ ^ *t[2]++ ^ *t[3]++ ^ *t[4]++ ^ *t[5]++ ^ *t[6]++ ^ *t[7]++;
      break;
    case 7: *m++ ^= *t[0]++ ^ *t[1]++ ^ *t[2]++ ^ *t[3]++ ^ *t[4]++ ^ *t[5]++ ^ *t[6]++; break;
    case 6: *m++ ^= *t[0]++ ^ *t[1]++ ^ *t[2]++ ^ *t[3]++ ^ *t[4]++ ^ *t[5]++; break;
    case 5: *m++ ^= *t[0]++ ^ *t[1]++ ^ *t[2]++ ^ *t[3]++ ^ *t[4]++; break;
    case 4: *m++ ^= *t[0]++ ^ *t[1]++ ^ *t[2]++ ^ *t[3]++; break;
    case 3: *m++ ^= *t[0]++ ^ *t[1]++ ^ *t[2]++; break;
    case 2: *m++ ^= *t[0]++ ^ *t[1]++; break;
    case 1: *m++ ^= *t[0]++; break;
    }
  }

  return;
}
