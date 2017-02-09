

#include "tea.h"

void main(int argc, char **argv){
	uint32_t key[4];
	char filename[100];
	int encryptOrDecrypt;
	if (argc != 4)//sdhsdhs
	{
		printf("\nUsage: tea.exe filename.ext [e|d] key\n");
		printf("          e=encode   d=decode \n");
		printf("       NOTE: Key must be no longer than 8 characters!\n");

		exit(-1);
	} else {
		strncpy(filename, argv[1], 99);

		filename[99] = '\0';

		if (argv[2][0] == 'e') encryptOrDecrypt = 1;
		else if (argv[2][0] == 'd') encryptOrDecrypt = 0;
		else
		{
			printf("\nUsage: tea.exe filename.ext [e|d] key\n");
			printf("		e=encrypt d=decrypt w=overwrite file\n");
			printf("       NOTE: Key must be no longer than 8 characters!\n");
			exit(-1);
		}
		getKeyFromArgv(key, argv[3]);
	}

	FILE *srcFile, *resultFile;

	if ((srcFile = fopen(filename, "rb")) == NULL) {
			printf("Cannot open source file.\n");
			exit(1);
		}

	if (encryptOrDecrypt){
		char *resultFileName = strcat(filename, ".enc");
		if ((resultFile = fopen(resultFileName, "wb")) == NULL) {
			printf("Cannot open result file.\n");
			exit(1);
		}
		encodeFile(srcFile, resultFile, key);
	}else{
		char resultFileName[105];
		strcpy(resultFileName, "");
		char* separator = strrchr(filename, '/');
		if (separator != NULL){
			separator++;
			strncat(resultFileName, filename, strlen(filename) - strlen(separator));
			strcat(resultFileName, "decoded_");
			strncat(resultFileName, separator, strlen(separator) - 4);
		} else {
			strcat(resultFileName, "decoded_");
			strncat(resultFileName, filename, strlen(filename) - 4);
		}
		if ((resultFile = fopen(resultFileName, "wb")) == NULL) {
			printf("Cannot open file.\n");
			exit(1);
		}
		decodeFile(srcFile, resultFile, key);
	}
	fclose(resultFile);
	fclose(srcFile);
	return 0;
}

void encodeFile(FILE* srcFile, FILE* resultFile, uint32_t* key){
	printf("Start encoding\n");
	fseek(srcFile, 0L, SEEK_END);
	long size = ftell(srcFile);
	fseek(srcFile, 0L, SEEK_SET);
	if ((fwrite(&size, sizeof(long), 1, resultFile)) != 1) {
		printf("Fatal Error writing output file!!!\n");
		exit(-1);
	}
	char buffer[8];
	while (!feof(srcFile)) {
		fread(buffer, sizeof(char), 8, srcFile);
		encrypt(buffer, key);
		if ((fwrite(buffer, sizeof(char), 8, resultFile)) != 8) {
			printf("Fatal Error writing output file!!!\n");
			exit(-1);
		}
	}
	printf("Encoding complete\n");
}

void decodeFile(FILE* srcFile, FILE* resultFile, uint32_t* key){
	printf("Start decoding\n");
	char buffer[8];
	long size;
	fread(&size, sizeof(long), 1, srcFile);
	for (size; size > 8; size -= 8){
		fread(buffer, sizeof(char), 8, srcFile);
		decrypt(buffer, key);
		if ((fwrite(buffer, sizeof(char), 8, resultFile)) != 8) {
			printf("Fatal Error writing output file!!!\n");
			exit(-1);
		}
	}
	fread(buffer, sizeof(char), 8, srcFile);
	decrypt(buffer, key);
	if ((fwrite(buffer, sizeof(char), size, resultFile)) != size) {
		printf("Fatal Error writing output file!!!\n");
		exit(-1);
	}
	printf("Decoding complete\n");
}

void encrypt(uint32_t* v, uint32_t* k){
	/* set up */
	uint32_t v0 = v[0];
	uint32_t v1 = v[1];
	uint32_t sum = 0;
	uint32_t i;

	/* a key schedule constant */
	uint32_t delta = 0x9e3779b9;

	/* cache key */
	uint32_t k0 = k[0];
	uint32_t k1 = k[1];
	uint32_t k2 = k[2];
	uint32_t k3 = k[3];

	/* basic cycle start */
	for (i = 0; i < 32; i++){
		sum += delta;
		v0 += ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
		v1 += ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
	}
	/* end cycle */

	v[0] = v0;
	v[1] = v1;
}

void decrypt(uint32_t* v, uint32_t* k){
	/* set up */
	uint32_t v0 = v[0];
	uint32_t v1 = v[1];
	uint32_t sum = 0xC6EF3720;
	uint32_t i;

	/* a key schedule constant */
	uint32_t delta = 0x9e3779b9;

	/* cache key */
	uint32_t k0 = k[0];
	uint32_t k1 = k[1];
	uint32_t k2 = k[2];
	uint32_t k3 = k[3];

	/* basic cycle start */
	for (i = 0; i < 32; i++){
		v1 -= ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
		v0 -= ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
		sum -= delta;
	}
	/* end cycle */

	v[0] = v0;
	v[1] = v1;
}

void getKeyFromArgv(char *key, char *argv){
	int x;

	for (x = 0; x<strlen(argv) && x<8; x++){
		if (x == 0) key[x] = argv[x] << 8;
		else key[x] = ((argv[x] << 8) | (key[x - 1] >> 8));
	}

	if (strlen(argv)>8) printf("\nONLY first *8* characters of key used!\n");

	if (x<8) while (x<8) key[x++] = 0;
}