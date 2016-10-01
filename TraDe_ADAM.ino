#include <PROCESS1.h>
#include <PRGSM1.h>

// Hardware Fixed Configuration
#define RS485_DATA_DIRECTION_PIN 21

// Server Configuration
#define NUMBER_OF_SERVERS 2 
#define SET_TRANSMISSION_INTERVAL  5    // Set the Interval in minutes

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

#define INFO_SIZE 3	     // Number of information for individual Parameter "Name:Unit:Maximum Range"
#ifndef REMOTE_CONFIGURATION_ENABLE
	#define MAX_PARAMETERS 3
	#define SPCB_ID 2895
	String g_configuration_parameters = "SPM:PPM:1000,SO2:ug/m3:200,NO2:g/m3:500";
#elif
	String g_configuration_parameters;
#endif // REMOTE_CONFIGURATION_ENABLE

String g_parameter_name[MAX_PARAMETERS];
String g_parameter_unit[MAX_PARAMETERS];
float g_max_concentration[MAX_PARAMETERS];

// Device Configuration Paramaters
String g_device_id = "nr1";
String g_device_key = "mcwlnf51RSPr0kXa";

/* 
.  Extracts SPCB ID, Number of Parameters, Name of Parameters, Units and Maximum Concerntration for each parameters
.  Global variable are used accoringly to get the information required
*/
void get_configuration_data();

/*
.  Sets the Time Period for GPRS Data Transmission using RTC
.  Type : Second, Minute, Hour, Days, Month, Year
.  Duration : Can be floating Number 
*/
void set_GPRS_transmission_period(float time_interval, void(*transmit_data)());

/*
.  This is pointer function which will be called after certain intervals defined by the user
.  It is used to transmit the data to the servers
*/
void transmit_data();

void setup() {
	// Initializing all the ports
	ISASerial.begin(9600);
	gsmSerial.begin(9600);
	
	DataPort.begin(9600);
	pinMode(RS485_DATA_DIRECTION_PIN, OUTPUT);
	digitalWrite(RS485_DATA_DIRECTION_PIN, HIGH);
	set_GPRS_transmission_period(SET_TRANSMISSION_INTERVAL,transmit_data);
}

void get_configuration_data(){
	/* 
	. Sample String File : "SPM:PPM:1000,SO2:ug/m3:200,NO2:g/m3:500"
	. "Parameter Name : Measured Unit : Maximum Range"
	*/
	char begin = 0,end;
	
	for (char i = 0; i < MAX_PARAMETERS; i++)
	{
		// Extracting parameter name
		end = g_configuration_parameters.indexOf(':', begin + 1);
		g_parameter_name[i] = g_configuration_parameters.substring(begin, end);
		begin = end;

		// Extracting parameter unit
		end = g_configuration_parameters.indexOf(':', begin + 1);
		g_parameter_unit[i] = g_configuration_parameters.substring(begin, end);
		begin = end;

		// Extracting parameter maximum range and converting it to float
		end = g_configuration_parameters.indexOf(',', begin + 1);
		g_max_concentration[i] = g_configuration_parameters.substring(begin, end).toFloat();
	}
}

void set_GPRS_transmission_period(float time_interval, void (*transmit_data)()){
	
	unsigned long time;    //  This number will overflow (go back to zero), after approximately 50 days.
	// Initialize clock
	time = millis();
	// Convert time_interval from minutes to milli second
	time_interval = (time_interval * 60) * 1000;

	// check if the time interval has reached then start transmission
	if (time_interval < (millis() - time))
		transmit_data();
}

void transmit_data(){

}
