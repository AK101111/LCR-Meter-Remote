#include <stdio.h>
#include <stdlib.h>
#include "lxicontrol.h"
#include <time.h>
#include <unistd.h>

#define LCR "10.225.65.106"
#define OUTPUTFILE "out.csv"
#define LCRPORT 5025
#define RCVMSGLEN 4000000
#define SENDMSGLEN 5

char buffer[SENDMSGLEN];
char responseFromLCRMeter[RCVMSGLEN];
int e4980a;

void sample(int frequency){
    char temp[] = ":FREQ ";
	sprintf (buffer, "%d",1000+frequency);
	strcat(temp,buffer);
	printf("%s\n",temp);
	IssueCommand(e4980a, temp);
	
	// introduce a time delay, before taking the next reading.		
    usleep(50);
    // sleep 50 mics, wait for oscillation to die.

	IssueCommand(e4980a, "*WAI\n");
	// Fetching the impedence.		
	IssueCommand(e4980a, "FETC:IMP:CORR?\n");
	IssueCommand(e4980a, "*WAI\n");
		
	strcat(responseFromLCRMeter,ReadResponse(e4980a,200));
	IssueCommand(e4980a, "*WAI\n");
}

int main()
{
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

	IssueCommand(e4980a,":MEM:CLE");	
	IssueCommand(e4980a, "*IDN?\n");
	printf("%s\n", ReadResponse(e4980a, 10000));
	
    //	Initial Setup	
	IssueCommand(e4980a, ":FUNC:IMP:TYPE CSRS\n");
	IssueCommand(e4980a,":FREQ 1000");
	IssueCommand(e4980a,":VOLT:LEV 1");
		
    //Data recoreded will be stored in file name 1.csv in the same directory as binary file of form .out
	FILE* fp; 
	fp = fopen(OUTPUTFILE, "w");
	

	//initializing frequency sweep:
	//The frequency sweep should ideally be logarithmic, but the device limits the frequency range from 20 - 20khz
	//The following code has two parts [1000,5000,4001] and [5001,9981,250]

    int freq;
	for(freq = 0; freq <= 4000; ++freq)
        sample(freq);

	for(freq=4001; freq <= 9000; freq += 20)
        sample(freq);
	
	IssueCommand(e4980a, "*WAI\n");
			
	fprintf(fp, "%s\n", responseFromLCRMeter);
	fclose(fp);

	// Resetting .
	IssueCommand(e4980a,":FREQ 1000");
	IssueCommand(e4980a,":VOLT:LEV 0.2");
}

