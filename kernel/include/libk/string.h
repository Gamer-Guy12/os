#ifndef STRING_H
#define STRING_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/// Int to String
/// @param num The number to convert
/// @param buf The buffer to write to. Recomeneded that this buffer be decently
/// big because itos doesn't do size checks and just writes
void string_itos8(int8_t num, char *buf);
/// See itos8
void string_itos16(int16_t num, char *buf);
/// See itos8
void string_itos32(int32_t num, char *buf);
/// See itos8
void string_itos64(int64_t num, char *buf);

/// Unsigned int to String
/// @param num The number to convert
/// @param buf The buffer to write to. Recomeneded that this buffer be decently
/// big because itos doesn't do size checks and just writes
void string_utos8(uint8_t num, char *buf);
/// See utos8
void string_utos16(uint16_t num, char *buf);
/// See utos8
void string_utos32(uint32_t num, char *buf);
/// See utos8
void string_utos64(uint64_t num, char *buf);

/// Unsinged int to Hex String
/// @param num The number to convert
/// @param buf The buffer to write to. Recomeneded that this buffer be decently
/// big because itos doesn't do size checks and just writes
void string_utohs8(uint8_t num, char *buf);
/// See utohs8
void string_utohs16(uint16_t num, char *buf);
/// See utohs8
void string_utohs32(uint32_t num, char *buf);
/// See utohs8
void string_utohs64(uint64_t num, char *buf);

/// Float to String
/// @param num The number to convert
/// @param buf The buffer to write to. Recomeneded that this buffer be decently
/// big because itos doesn't do size checks and just writes
///
/// IMPORTANT: THIS FUNCTION HAS NOT BEEN IMPLEMENTED
void string_ftos32(float num, char *buf);
/// See ftos32
void string_ftos64(double num, char *buf);

uint16_t strlen(char *buf);
void strrev(char *buf);
bool strcmp(char *str1, char *str2, size_t length);

void string_strcpy(char *dest, const char *src)

#endif
