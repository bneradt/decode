/** @file

  Reproduce a compiler optimization bug in GCC 12.

  @section license License

  Licensed to the Apache Software Foundation (ASF) under one
  or more contributor license agreements.  See the NOTICE file
  distributed with this work for additional information
  regarding copyright ownership.  The ASF licenses this file
  to you under the Apache License, Version 2.0 (the
  "License"); you may not use this file except in compliance
  with the License.  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GUC_BASE64_WHITESPACE_CHAR 64
#define GUC_BASE64_EQUALS_CHAR     65
#define GUC_BASE64_INVALID_CHAR    66

static const unsigned char d[] = {
  66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 64, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
  66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 62, 66, 66, 66, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 66, 66, 66, 65, 66, 66,
  66, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 66, 66, 66, 66, 66,
  66, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 66, 66, 66, 66, 66,
  66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
  66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
  66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
  66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66};

static size_t
calcDecodeLength(const char *b64input)
{
  // Removed this implementation because it may be proprietary.
  // Given our input, 5 is correct. Always return that.
  return 5;
}
// Code is copied from
// https://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64
// with minor modification.
static int
base64Decode(char const *in, unsigned char **out_str, size_t *out_str_len)
{ // Decodes a base64 encoded string

  char const    *end       = in + strlen(in);
  char           iter      = 0;
  uint32_t       buf       = 0;
  size_t         out_index = 0;
  size_t         out_len   = calcDecodeLength(in);
  unsigned char *out       = (unsigned char *)calloc(sizeof(unsigned char), out_len + 1);
  if (out == NULL) {
    return 1;
  }

  while (in < end) {
    unsigned char c = d[(int)*in++];

    switch (c) {
    case GUC_BASE64_WHITESPACE_CHAR:
      continue; /* skip whitespace */
    case GUC_BASE64_INVALID_CHAR:
      goto BAIL;                 /* invalid input, return error */
    case GUC_BASE64_EQUALS_CHAR: /* pad character, end of data */
      in = end;
      continue;
    default:
      buf = buf << 6 | c;
      iter++; // increment the number of iteration
      /* If the buffer is full, split it into bytes */
      if (iter == 4) {
        out[out_index++] = (char)((buf >> 16) & 255);
        out[out_index++] = (char)((buf >> 8) & 255);
        out[out_index++] = (char)(buf & 255);
        buf              = 0;
        iter             = 0;
      }
    }
  }

  if (iter == 3) {
    out[out_index++] = (buf >> 10) & 255;
    out[out_index++] = (buf >> 2) & 255;
  } else if (iter == 2) {
    out[out_index++] = (buf >> 4) & 255;
  }

  if (out_len != out_index) {
    goto BAIL;
  }

  out[out_len] = 0;
  *out_str_len = out_index;
  *out_str     = out;
  return 0;

BAIL:
  free(out);
  return 1;
}

int
main(int argc, char *argv[])
{
  char const *input  = "S/CBkDo=";
  size_t      length = strlen(input);
  char       *out    = NULL;
  printf("input is %s\n", input);
  int ret = base64Decode(input, (unsigned char **)&out, &length);
  if (ret != 0 || out == NULL) {
    printf("decoded string is incorrectly NULL\n");
    return 1;
  }
  printf("result is %02x %02x %02x %02x "
         "%02x .\n",
         (unsigned char)out[0], (unsigned char)out[1], (unsigned char)out[2], (unsigned char)out[3], (unsigned char)out[4]);
  free(out);
  return 0;
}
