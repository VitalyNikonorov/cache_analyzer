#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "string.h"

#define MAXSIZE 1048576000
#define MINSIZE 100
#define RUNTIMES 50
#define ITERATIONS 1000000

struct CNode {
	struct CNode *next;
	char ch;
};

void printSystemCPUInfo(FILE* resultFile);
void printSequentialCPUInfo(FILE* resultFile);
void printRandomAccessCPUInfo(FILE* resultFile);
size_t getRandom(size_t limit, size_t* indexArray, size_t i);
int isIndexInArray(size_t index, size_t* indexArray, size_t max);

int main() {
	printf("Programm is working...\n");

	FILE *result;
	result = fopen("result.txt", "w");

	printf("Analysis of system information...\n");
	printSystemCPUInfo(result);

	printf("Analysis of random access...\n");	
	printRandomAccessCPUInfo(result);

	printf("Analysis of sequential access...\n");	
	printSequentialCPUInfo(result);

	fclose(result);

	printf("All right. Results in results.txt\n");

	return 0;
}


void printSystemCPUInfo(FILE* resultFile) {
	FILE *cpuInfo;
	cpuInfo = popen("sysctl -a | grep cachesize", "r");

	fprintf(resultFile, "\n\n---------System information about CPU cache:--------\n\n");
	
	char *line;
	char buf[1024];

	while ( (line = fgets(buf, sizeof buf, cpuInfo)) != NULL)  {			
		fprintf(resultFile, "%s", line);
	}

	fprintf(resultFile, "\n\n---------System information about CPU cache --------\n\n");
	pclose(cpuInfo);
}

void printSequentialCPUInfo(FILE* resultFile) {

	fprintf(resultFile, "\n\n---------Sequential information about CPU cache:--------\n\n");
	printf("CNode size: %zd\n", sizeof(struct CNode));
	fprintf(resultFile, "\n\nBytes\t\t\tSeconds\n\n");
	
	double runTime = 0;

	size_t cNodeSize = sizeof(struct CNode);
	
	size_t min = MINSIZE / cNodeSize;
	size_t max = MAXSIZE / cNodeSize;

	for(size_t size = min; size < max; size *= 2) {
		struct CNode *buff = malloc(size * sizeof(struct CNode));
		
		for(size_t i = 0; i < size - 1; i++) {
			buff[i].next = &buff[i+1];
		}

		buff[size - 1].next = &buff[0];

		clock_t begin, end; 
				
		for (int j = 0; j < RUNTIMES; j++) {
			begin = clock();
			struct CNode *node = buff;

			for (int i = 0; i < ITERATIONS; i++) {
				node = node->next;	
			}

			end = clock();
			runTime += (double)(end - begin) / CLOCKS_PER_SEC;	
		}


		runTime /= RUNTIMES;
		

		free(buff);

		fprintf(resultFile, "%zd", cNodeSize * size);
		fprintf(resultFile, "\t \t%f\n", runTime);
		printf("%zd done\n", cNodeSize * size);
	}

	fprintf(resultFile, "\n\n---------Sequential information about CPU cache--------\n\n");
}


void printRandomAccessCPUInfo(FILE* resultFile) {

	fprintf(resultFile, "\n\n---------Random access information about CPU cache:--------\n\n");
	fprintf(resultFile, "\n\nBytes\t\t\tSeconds\n\n");
	
	double runTime = 0;

	size_t cNodeSize = sizeof(struct CNode);
	
	size_t min = MINSIZE / cNodeSize;
	size_t max = MAXSIZE / cNodeSize;

	for(size_t size = min; size < max; size *= 2) {
		struct CNode *buff = malloc(size * sizeof(struct CNode));

		size_t *array = malloc(size * sizeof(size_t));
		memset(array, 0, size * sizeof(size_t));

		for(size_t i = 0; i < size; i++) {
			size_t pointer = getRandom(size, array, i);
			buff[i].next = &buff[pointer];
		}

		clock_t begin, end; 
				
		for (int j = 0; j < RUNTIMES; j++) {
			begin = clock();
			struct CNode *node = buff;

			for (int i = 0; i < ITERATIONS; i++) {
				node = node->next;
			}

			end = clock();

			runTime += (double)(end - begin) / CLOCKS_PER_SEC;	
		}

		runTime /= RUNTIMES;
		

		free(array);
		free(buff);

		fprintf(resultFile, "%zd", cNodeSize * size);
		fprintf(resultFile, "\t \t%f\n", runTime);
		printf("%zd done\n", cNodeSize * size);
	}

	fprintf(resultFile, "\n\n---------Random access about CPU cache--------\n\n");
}

size_t getRandom(size_t limit, size_t* indexArray, size_t i) {
	size_t random_number;
	srand(time(NULL));

	do {
		random_number = rand() % limit;
	} while ( (random_number > limit) && isIndexInArray(random_number, indexArray, i) );

	indexArray[i] = random_number;
	return random_number;
}

int isIndexInArray(size_t index, size_t* indexArray, size_t max){

	for (size_t i = 0; i < max; i++){
		if(indexArray[i] == index){
			return 1;
		}
	}
	return 0;
};