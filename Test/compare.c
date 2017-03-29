#include<stdlib.h>
#include<stdio.h>
#include<string.h>

int main(int argc, char** argv)
{
	if (argc==1)
	{
		return 1;
	}
	FILE* FAns = fopen(argv[1], "r");
	FILE* FNew = fopen(argv[2], "r");
	char LineAns[512], LineNew[512];
	int line = 0;

	if (FAns==NULL||FNew==NULL)
	{
		printf("cannot read the file\n");
	}

	while(!feof(FAns)||!feof(FNew))
	{
		fgets(LineAns,512,FAns);
		fgets(LineNew,512,FNew);
		if(strcmp(LineAns,LineNew)==1){
			printf("the line %d is not equal\n",line);
			return 0;
		}
	}

	fclose(FAns);
	fclose(FNew);
	printf("the two file is the same\n");
	return 0;

}
