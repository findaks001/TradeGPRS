#include <PROCESS1.h>
#include <PRGSM1.h>

// Server Configuration
#define NUMBER_OF_SERVERS 2 

// Hardware Fixed Configuration
#define RS485_DATA_DIRECTION_PIN 21

// GSM Configuration Parameters
#define ISA_GSM_CONFIG 1

// Additonal Feature Configurations
#define ENABLE_MASTER         // if Defined a commandString will be sent to get response from Sensor Device using RS485

#ifdef ENABLE_MASTER
	#define initiationString #01     // #01 - Command to be Sent to get response
	#define maxParameter 8

	String ADAM_data = "#>-5.00-6.00-02.96 ";
	String param_value_string[maxParameter] = { };
	float param_value_float[maxParameter] = { 0, 0, 0, 0, 0 };
#endif

#define ENABLE_ENCRYPTION
#define ENABLE_ERROR_HANDLING
// Parameter Configuration Part
// #define REMOTE_CONFIGURATION_ENABLE

#ifndef REMOTE_CONFIGURATION_ENABLE
	#define MAX_PARAMETERS 3 
	#define SPCB_ID 2895
#endif // REMOTE_CONFIGURATION_ENABLE

String g_parameter_name[MAX_PARAMETERS];
String g_parameter_unit[MAX_PARAMETERS];
String g_max_concentration[MAX_PARAMETERS];
String g_configuration_parameters = "{2895,3,[SPM:PPM:1000,SO2:ug/m3:200,NO2:g/m3:500]}";
// Device Configuration Paramaters
String g_device_id = "nr1";
String g_device_key = "mcwlnf51RSPr0kXa";


/* 
.  Extracts SPCB ID, Number of Parameters, Parameters, Units and Maximum Value for each parameters
.  Global variable are used accoringly to get the information required
*/
void get_configuration_data(String g_configuration_parameters);

/*
.  Sets the Time Period for GPRS Data Transmission using RTC
.  Type : Second, Minute, Hour, Days, Month, Year
.  Duration : Can be floating Number 
*/
void set_GPRS_transmission_period(String Type,float Duration);

void setup() {
	// Initializing all the ports
	ISASerial.begin(9600);
	gsmSerial.begin(9600);
	
	DataPort.begin(9600);
	pinMode(RS485_DATA_DIRECTION_PIN, OUTPUT);
	digitalWrite(RS485_DATA_DIRECTION_PIN, HIGH);
	set_GPRS_transmission_period("minute", 5);
}

void get_configuration_data(String g_configuration_parameters)
{

}