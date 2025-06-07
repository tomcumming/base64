#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base64.h"

void pretty_buffer(size_t len, uint8_t *str) {
  for (size_t i = 0; i < len; i += 1)
    printf("0x%02X ", str[i]);
}

void test_string(size_t len, uint8_t *str) {
  size_t enc_bufLength = b64_encodedLength(len);
  size_t dec_bufLength = b64_decodedLength(enc_bufLength);

  uint8_t *enc_buf = malloc(enc_bufLength);
  uint8_t *dec_buf = malloc(dec_bufLength);

  b64_encode(str, len, enc_buf);
  size_t decodedLength = b64_decode(enc_buf, enc_bufLength, dec_buf);

  if (len != decodedLength || memcmp(str, dec_buf, len)) {
    printf("Mismatch buffers of length %lu, %lu\n", len, decodedLength);
    printf("in:\t");
    pretty_buffer(len, str);
    printf("\n");
    printf("dec:\t");
    pretty_buffer(decodedLength, dec_buf);
    printf("\n");
    printf("enc: %s\n", enc_buf);
    exit(1);
  }

  free(enc_buf);
  free(dec_buf);
}

void wikipedia_tests() {
  char *test_inputs[] = {"", "Man", "Ma", "M"};
  char *test_output[] = {"", "TWFu", "TWE=", "TQ=="};

  size_t test_count = sizeof(test_inputs) / sizeof(char *);

  for (size_t i = 0; i < test_count; i += 1) {
    char *input = test_inputs[i];
    char *expected = test_output[i];

    printf("Checking encode(%s)\t=\t%s\n", input, expected);

    size_t len = strlen(input);
    size_t enc_bufLength = b64_encodedLength(len);

    uint8_t *enc_buf = malloc(enc_bufLength);

    b64_encode((uint8_t *)input, len, enc_buf);

    if (memcmp(expected, enc_buf, strlen(expected))) {
      printf("FAIL\n");
      exit(1);
    }

    free(enc_buf);
  }
}

void test_random_string(int len, uint8_t *buf) {
  for (size_t j = 0; j < len; j += 1)
    buf[j] = rand();

  test_string(len, buf);
}

void fuzz(int len) {
  size_t FUZZ_ITERS = 2 << 20;

  uint8_t *buf = malloc(len);

  for (size_t i = 0; i < FUZZ_ITERS; i += 1)
    test_random_string(len, buf);

  free(buf);
}

int main(int argc, char **argv) {

  wikipedia_tests();

  size_t MAX_TEST_LEN = 16;
  printf("Fuzzing lengths: ");
  for (size_t len = 0; len <= MAX_TEST_LEN; len += 1) {
    printf("%lu ", len);
    fflush(stdout);
    fuzz(len);
  }
  printf("\n");

  size_t LONG_STRING_SIZES[] = {2 << 10, 2 << 20, 2 << 25};

  for (size_t i = 0; i < sizeof(LONG_STRING_SIZES) / sizeof(size_t); i += 1) {
    size_t len = LONG_STRING_SIZES[i];
    printf("Testing random string of length %lu\n", len);
    uint8_t *buf = malloc(len);
    test_random_string(len, buf);
    free(buf);
  }

  return 0;
}
