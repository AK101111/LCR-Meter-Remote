#include <stdio.h>
#include <stdlib.h>
#include "lxicontrol.h"
#include <time.h>
#include <unistd.h>

#define LCR "10.225.65.106"
#define LCRPORT 5025

int main()
{
	int e4980a;

	e4980a = CreateConnection(LCR, LCRPORT);

/*
	The following commands were used: Pl refer to the Agilent E4980a precision LCR meter.
	IssueCommand(e4980a,":SYST:PRES");		-> Preset
	IssueCommand(e4980a,":SYST:REST");		-> Reset		
	IssueCommand(e4980a,":MEM:CLE");		-> Clear Memory buffer
	IssueCommand(e4980a, "*IDN?\n");		-> Identity of Device
	IssueCommand(e4980a, ":FUNC:IMP:TYPE CSRS\n");	-> Measurement type of impedence (Ls-Rs,Cs-Rs...)
	IssueCommand(e4980a,":FREQ 1000");		-> Setting Frequency for measurment
	IssueCommand(e4980a,":VOLT:LEV 1");		-> Setting Voltage level for measurement
	IssueCommand(e4980a, "FETC:IMP:CORR?\n");	-> Fetching Impedence
*/





//	IssueCommand(e4980a,":SYST:PRES");
//	IssueCommand(e4980a,":SYST:REST");
	
	IssueCommand(e4980a,":MEM:CLE");	
	IssueCommand(e4980a, "*IDN?\n");
	printf("%s\n", ReadResponse(e4980a, 10000));
	
//	Initial Setup	
	IssueCommand(e4980a, ":FUNC:IMP:TYPE CSRS\n");
	IssueCommand(e4980a,":FREQ 1000");
	IssueCommand(e4980a,":VOLT:LEV 1");
		
	char buffer[5]; 
	char buffer2[5];
	int j; 
	int i;
	int t;
	
//      Data recoreded will be stored in file name 1.csv in the same directory as binary file of form .out
	FILE* fp; 
	fp = fopen("1.csv", "w");
	
	char X1[4000000];


	//initializing frequency sweep:
	//The frequency sweep should ideally be logarithmic, but the device limits the frequency range from 20 - 20khz
	//The following code has two parts [1000,5000,4001] and [5001,9981,250]

	for(i=0; i<4001; i=i+1)
	{
		char temp[] = ":FREQ ";
		sprintf (buffer, "%d",1000+i);
		strcat(temp,buffer);
		printf("%s\n",temp);
		IssueCommand(e4980a, temp);
		//Sleep(10);
		
		//The next line of the code is not at all portable, the for loop was used to introduce a time delay, before taking the next reading.		
		for(t=0;t<90000000;t++){}

		IssueCommand(e4980a, "*WAI\n");
		// Fetching the impedence.		
		IssueCommand(e4980a, "FETC:IMP:CORR?\n");
		IssueCommand(e4980a, "*WAI\n");
		
		//printf("%s\n", ReadResponse(e4980a, 10000));

		strcat(X1,ReadResponse(e4980a,200));
		IssueCommand(e4980a, "*WAI\n");
	}

	for(i=4001; i<9000; i=i+20)
	{
		char temp[] = ":FREQ ";
		sprintf (buffer, "%d",1000+i);
		strcat(temp,buffer);
		printf("%s\n",temp);
		IssueCommand(e4980a, temp);
		//Sleep(10);

		//The next line of the code is not at all portable, the for loop was used to introduce a time delay, before taking the next reading.		
		for(t=0;t<90000000;t++){}
		
		IssueCommand(e4980a, "*WAI\n");
		// Fetching the impedence.			
		IssueCommand(e4980a, "FETC:IMP:CORR?\n");
		IssueCommand(e4980a, "*WAI\n");

		//printf("%s\n", ReadResponse(e4980a, 10000));

		strcat(X1,ReadResponse(e4980a,200));
		IssueCommand(e4980a, "*WAI\n");
	}
	
	IssueCommand(e4980a, "*WAI\n");
			
	fprintf(fp, "%s\n", X1);
	fclose(fp);
	

	// Resetting .
	IssueCommand(e4980a,":FREQ 1000");
	IssueCommand(e4980a,":VOLT:LEV 0.2");
}

