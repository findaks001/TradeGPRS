#include <PROCESS1.h>
#include <PRGSM1.h>

// Hardware Fixed Configuration
#define RS485_DATA_DIRECTION_PIN 21

// Device Configuration Paramaters
String g_device_id = "nr1";
String g_device_key = "mcwlnf51RSPr0kXa";

// Define Configuration Parameters
#define NUMBER_OF_SERVERS 2 
#define SET_TRANSMISSION_INTERVAL  5     // Set the Interval in minutes
#define MAX_PARAMETERS 3
#define SPCB_ID 2895
#define TEST_MODE 1			         // Running the code in testing mode defalut input strings to test the output
#define MASTER_STRING 0                  // if true : CommandString will be sent to get response from Sensor Device
#define REMOTE_CONFIGURATION 1           // if true : Can't be configured remotely
#define ENCRYPTION 0		             // if true : the data transmission will be encrypted
#define ERROR_HANDLING 0                 // if true : Different Error will be acknowledged


// Define default parameter if remote configutation feature not available
#if REMOTE_CONFIGURATION
	String g_configuration_parameters = "SPM:PPM:1000,SO2:ug/m3:200,NO2:g/m3:500";
#else
	String g_configuration_parameters;
#endif // REMOTE_CONFIGURATION

// In testing mode default values are assigned
#if TEST_MODE
	String g_sample_response_data = "#>-5.00-6.00-02.96 ";
#else
	String g_sample_response_data;
#endif	// TEST_MODE

// Enable Command String and get Response String 
#if MASTER_STRING
	#define initiationString #01         // #01 - Command to be Sent to get response
#endif // ENABLE_MASTER_STRING

String g_parameter_name[MAX_PARAMETERS];
String g_parameter_unit[MAX_PARAMETERS];
float g_max_concentration[MAX_PARAMETERS];
float g_parameter_value[MAX_PARAMETERS];

/* 
.  Extracts SPCB ID, Number of Parameters, Name of Parameters, Units and Maximum Concerntration for each parameters
.  Global variable are used accoringly to get the information required
*/
void set_data_format();

/*
.  Sets the Time Period for GPRS Data Transmission using RTC
.  Type : Second, Minute, Hour, Days, Month, Year
.  Duration : Can be floating Number 
*/
void set_gprs_transmission_period(float time_interval, void(*transmit_data)());

/*
.  This is pointer function which will be called after certain intervals defined by the user
.  It is used to transmit the data to the servers
*/
void transmit_gprs_data();

/*
.	Read data from RS485 port and assign
*/
void read_data();

/*
.	This function will return the formated data string required for sending the data to server
*/
String get_gprs_packet_data(char type);

void setup() {
	// Initializing all the ports
	ISASerial.begin(9600);
	gsmSerial.begin(9600);
	
	DataPort.begin(9600);
	pinMode(RS485_DATA_DIRECTION_PIN, OUTPUT);
	digitalWrite(RS485_DATA_DIRECTION_PIN, HIGH);
	set_gprs_transmission_period(SET_TRANSMISSION_INTERVAL,transmit_gprs_data);
	time_initialize();
	set_data_format();
}

void set_data_format(){
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

void set_gprs_transmission_period(float time_interval, void (*transmit_data)()){
	
	unsigned long time;    //  This number will overflow (go back to zero), after approximately 50 days.
	// Initialize clock
	time = millis();
	// Convert time_interval from minutes to milli second
	time_interval = (time_interval * 60) * 1000;

	// check if the time interval has reached then start transmission
	if (time_interval < (millis() - time))
		transmit_gprs_data();
}

void transmit_gprs_data(){
	String gprs_packet_data;
	
	for (char i = 0; i < NUMBER_OF_SERVERS; i++){
		gprs_packet_data = get_gprs_packet_data(i);

		switch (i){
		case 0:   // SPCB Server
			Serial.println("Sending data to SPCB Server");
			#if TEST_MODE
			gprs_send(gprs_packet_data, i, "TCP", "52.220.52.25", "4567");    // Test TCP Server
			#else  
			gprs_send(gprs_packet_data, i, "TCP", "117.239.117.27", "9090");  // SPCB Server
			#endif
			break;

		case 1:   // Phoenix Server
			Serial.println("Sending data to Phoenix Server");
			gprs_send(gprs_packet_data, i, "HTTP", "data.phoenixrobotix.com", "80", "/data_logger/log_data.php?");
			break;
		default:
			break;
		}
	}
}

String get_gprs_packet_data(char type){

	String prepacket_data, data_packet;
	
	switch (type){
	case 0:
		// data format for Phoenix Server
		prepacket_data = "[";
		for (char i = 0; i < MAX_PARAMETERS; i++) {
			prepacket_data = prepacket_data + "'" + String(g_parameter_value[i]) + "'" + ",";
		}
		prepacket_data = prepacket_data.substring(0, prepacket_data.length() - 1) + "]";
		data_packet = config_packet(g_device_id, g_device_key, prepacket_data);
		break;

	case 1:
		// data format for spcb server
		for (char i = 0; i < MAX_PARAMETERS; i++) {
			if (g_parameter_value[i]<0)
				g_parameter_value[i] = 0;
			prepacket_data = prepacket_data + g_parameter_name[i] + "," + String(g_parameter_value[i]) + "," + g_parameter_unit[i];
			prepacket_data = prepacket_data + ",";
		}
		data_packet = "*packet=" + date_server + "," + time_server + "," + SPCB_ID + "," + prepacket_data + "#";
		break;
	default:
		break;
	}
	
	#if TEST_MODE
	Serial.println(data_packet);
	#endif
}
