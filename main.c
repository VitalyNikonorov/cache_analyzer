#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "string.h"

#define MAXSIZE 10485760
#define MINSIZE 1000
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
void shuffle(size_t *array, size_t n);

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
	#ifdef __linux__
		cpuInfo = popen("lscpu | grep cache", "r");
	#elif __APPLE__
		cpuInfo = popen("sysctl -a | grep cachesize", "r");
	#else 
		return;
	#endif	

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

	for(size_t size = min; size < max; size *= 2 ) {
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

		// if(size > 100000){
		// 	size += (5000 * min);
		// }else{
		// 	size += min;
		// }		

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

	for(size_t size = min; size < max; size *= 2 ) {

		struct CNode *buff = malloc(size * sizeof(struct CNode));

		size_t *array = malloc(size * sizeof(size_t));
		memset(array, 0, size * sizeof(size_t));

		for(size_t i = 0; i < size; i++) {
			array[i] = i;
		}

		shuffle(array, size);


		for(size_t i = 0; i < size; i++) {

			buff[i].next = &buff[array[i]];
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
		
		// if(size > 100000){
		// 	size += 5000 * min;
		// }else{
		// 	size += min;
		// }

		free(array);
		free(buff);

		fprintf(resultFile, "%zd", cNodeSize * size);
		fprintf(resultFile, "\t \t%f\n", runTime);
		printf("%zd done\n", cNodeSize * size);
	}

	fprintf(resultFile, "\n\n---------Random access about CPU cache--------\n\n");
}


void shuffle(size_t *array, size_t n)
{
    if (n > 1) 
    {
        size_t i;
        for (i = 0; i < n - 1; i++) 
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          size_t t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}

// size_t getRandom(size_t limit, size_t* indexArray, size_t i) {
// 	size_t random_number;
// 	srand(time(NULL));

// 	do {
// 		random_number = rand() % limit;
// 	} while ( (random_number > limit) && isIndexInArray(random_number, indexArray, i) );

// 	indexArray[i] = random_number;
// 	return random_number;
// }

// int isIndexInArray(size_t index, size_t* indexArray, size_t max){

// 	for (size_t i = 0; i < max; i++){
// 		if(indexArray[i] == index){
// 			return 1;
// 		}
// 	}
// 	return 0;
// };