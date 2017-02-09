#ifndef _TEA_DOT_H
#define _TEA_DOT_H
#define _CRT_SECURE_NO_WARNINGS

#include <stdint.h>
#include <stdio.h>
#include <string.h>

void encrypt(uint32_t* v, uint32_t* k);
void decrypt(uint32_t* v, uint32_t* k);
void encodeFile(FILE* srcFile, FILE* resultFile, uint32_t* key);
void decodeFile(FILE* srcFile, FILE* resultFile, uint32_t* key);
void getKeyFromArgv(uint32_t *key, char *arg);

#endif