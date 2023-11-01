#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const size_t BUFFER_SIZE = 1048576;

void print_usage() {
  fprintf(stdout, "xor <file name> <block size>\n");
  return;
}

int parse_args(int argc, const char **argv, const char **file_name, int *size) {
  if (argc < 3) {
    print_usage();
    return -1;
  }

  *file_name = argv[1];
  *size = atoi(argv[2]);

  switch (*size) {
    case 8:
    case 16:
    case 32:
    case 64:
      break;
    default:
      fprintf(stderr, "size must be one of <8, 16, 32, 64>: %d\n", *size);
      return -1;
  }

  return 0;
}

uint8_t xor_chunk_8(uint8_t *data, size_t segments) {
  int chunk_xor = 0;

  for (int i = 0; i < segments; i++) {
    chunk_xor ^= data[i];
  }

  return chunk_xor;
}

uint16_t xor_chunk_16(uint16_t *data, size_t segments) {
  int chunk_xor = 0;

  for (int i = 0; i < segments; i++) {
    chunk_xor ^= data[i];
  }

  return chunk_xor;
}

uint32_t xor_chunk_32(uint32_t *data, size_t segments) {
  int chunk_xor = 0;

  for (int i = 0; i < segments; i++) {
    chunk_xor ^= data[i];
  }

  return chunk_xor;
}

uint64_t xor_chunk_64(uint64_t *data, size_t segments) {
  int chunk_xor = 0;

  for (int i = 0; i < segments; i++) {
    chunk_xor ^= data[i];
  }

  return chunk_xor;
}

uint64_t xor_chunk(char *data, size_t buffer_size, int size) {
  switch (size) {
    case 8:
      return xor_chunk_8((uint8_t *)data, buffer_size);
    case 16:
      return xor_chunk_16((uint16_t *)data, buffer_size >> 1);
    case 32:
      return xor_chunk_32((uint32_t *)data, buffer_size >> 2);
    case 64:
      return xor_chunk_64((uint64_t *)data, buffer_size >> 3);
  }

  fprintf(stderr, "cannot compute XOR, invalid size: %d\n", size);
  return 0;
}

uint64_t compute_xor(const char *file_name, int size) {
  FILE *fptr;
  char *buffer;
  int file_xor = 0;

  buffer = malloc(BUFFER_SIZE);
  if (buffer == NULL) {
    fprintf(stderr, "failed to alloc read buffer: %s\n", strerror(errno));
  }

  fptr = fopen(file_name, "rb");
  if (fptr == NULL) {
    fprintf(stderr, "failed to open '%s': %s\n", file_name, strerror(errno));
    free(buffer);
    return 0;
  }

  while (!feof(fptr)) {
    size_t bytes;
    memset(buffer, 0, BUFFER_SIZE);

    bytes = fread(buffer, BUFFER_SIZE, 1, fptr);
    if (!bytes) {
      if (ferror(fptr)) {
        free(buffer);
        fprintf(stderr, "failed to read data from file '%s': %s\n", file_name,
                strerror(errno));
        return 0;
      }
      continue;
    }
  }

  file_xor ^= xor_chunk(buffer, BUFFER_SIZE, size);

  free(buffer);

  return file_xor;
}

int main(int argc, const char **argv) {
  const char *file_name;
  int size;
  uint64_t file_xor;

  if (parse_args(argc, argv, &file_name, &size) != 0) {
    return -1;
  }

  fprintf(stdout, "computing %d bit XOR for file '%s'\n", size, file_name);

  file_xor = compute_xor(file_name, size);
  if (file_xor < 0) {
    return -1;
  }

  fprintf(stdout, "0x%lx\n", file_xor);

  return 0;
}
