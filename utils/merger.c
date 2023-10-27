#include <iostream>
#include <sstream>
#include<stdlib.h>
#include <math.h>
#include <string>
#include"cursemu.h"

using namespace std;


float sizef(FILE *fh)
{
	fseek(fh, 0L, SEEK_END);
	int sz = ftell(fh);
	fseek(fh, 0L, SEEK_SET);	
	return sz;
}
int main(int argc, char ** argv)
{
   FILE *fs1, *fs2, *fs3, *ft;
 
   string ch, file1, file2, file3, file4;
   int  sd_1, sd_2;
 
   cout << "Enter name of VMM file ";
   std::cin >> file1;
 
   cout << "Enter name of guest one file ";
   std::cin >> file2;

   cout << "Enter the beginning address of guest one ";
   std::cin >> hex >> sd_1;

   cout << "Enter name of guest two file ";
   std::cin >> file3;
 
   cout << "Enter the beginning address of guest two ";
   std::cin >> hex >> sd_2;

   cout << "Enter name of file which will store contents of two files ";
   std::cin >> file4;

   char* chr1 = strdup(file1.c_str());
   fs1 = fopen(chr1,"rb");
   free(chr1); 

   char* chr2 = strdup(file2.c_str());
   fs2 = fopen(chr2,"rb");
   free(chr2); 

   char* chr3 = strdup(file3.c_str());
   fs3 = fopen(chr3,"rb");
   free(chr3);

   if( fs1 == NULL || fs2 == NULL  || fs3 == NULL)
   {
      perror("Error ");
      cout << "Press any key to exit...\n";
      std::cin.get();
      exit(EXIT_FAILURE);
   }
   char* chr4 = strdup(file4.c_str());
   ft = fopen(chr4,"wb");
   free(chr4); 

   if( ft == NULL )
   {
      perror("Error ");
      cout << "Press any key to exit...\n";
      exit(EXIT_FAILURE);
   }


	FILE *ft1 = fopen("zerof_1","wb");
	int sz = sizef(fs1); int i;
	for(i = 0; i < (sd_1- sz); i++)
		fputc(0,ft1);
	fclose(ft1);

	FILE *ft2 = fopen("zerof_2","wb");
	sz = sizef(fs2);
	for(i = 0; i < abs(sd_2 - sd_1 - sz); i++)
		fputc(0,ft2);
	fclose(ft2);
	
	std::stringstream ss;
	ss << "cat " << file1 << " zerof_1 " << file2 << " zerof_2 " << file3 <<  " >> " << file4;
	std::string s = ss.str();

	char* cm = strdup(s.c_str());
	system( cm ) ;
	free(cm);
	
	system( "rm zerof_1" ) ;
	system( "rm zerof_2" ) ;

 
   fclose(fs1);
   fclose(fs2);
   fclose(ft);
 
	std::cin.get();
   return 0;
}
