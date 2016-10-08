// Device Configuration Paramaters
String g_device_id = "nr1";
String g_device_key = "mcwlnf51RSPr0kXa";
// Define Configuration Parameters
#define MAX_PARAMETERS 3
#define SPCB_ID 2895
#define TEST_MODE 1			             // Running the code in testing mode defalut input strings to test the output
#define MASTER_STRING 1                  // if true : CommandString will be sent to get response from Sensor Device

#define REMOTE_CONFIGURATION 1           // if true : Can't be configured remotely
#define ENCRYPTION 0		             // if true : the data transmission will be encrypted
#define ERROR_HANDLING 0                 // if true : Different Error will be acknowledged

#define NUMBER_OF_SERVERS 2 
#define TRANSMISSION_INTERVAL  (5 * 60) * 1000    // Set the Interval in milliseconds (Minutes * 60) * 1000 

// Hardware Fixed Configuration
#define RS485_DATA_DIRECTION_PIN 21
#define MIN_CURRENT_SIGNAL 4
#define MAX_CURRENT_SIGNAL 20
// Define default parameter if remote configutation feature not available
#if REMOTE_CONFIGURATION
	String g_configuration_parameters = "SPM:PPM:1000,SO2:ug/m3:200,NO2:g/m3:500";
#else
	String g_configuration_parameters;
#endif // REMOTE_CONFIGURATION

// In testing mode default values are assigned
#if TEST_MODE && MASTER_STRING
	String g_raw_rs485_data = "#>-5.00-6.00-07.96 ";
#else
	String g_raw_rs485_data;;
#endif	// TEST_MODE

// Enable Command String and get Response String 
#if MASTER_STRING
	#define COMMAND_STRING "#01"         // #01 - Command to be Sent to get response
#endif // ENABLE_MASTER_STRING

String g_parameter_name[MAX_PARAMETERS];
String g_parameter_unit[MAX_PARAMETERS];
float g_max_concentration[MAX_PARAMETERS];
float g_parameter_value[MAX_PARAMETERS];

unsigned long g_time = 0;    //  This number will overflow (go back to zero), after approximately 50 days.

/* 
.  Extracts SPCB ID, Number of Parameters, Name of Parameters, Units and Maximum Concerntration for each parameters
.  Global variable are used accoringly to get the information required
*/
void set_data_format();

/*
.  Checks if the set time interval has lapsed or not
*/
bool check_gprs_transmission_period();

/*
.  This will be called after certain intervals defined by the user
.  It is used to transmit the data to the servers
*/
void transmit_gprs_data();

/*
.	Read data from RS485 port and assign
*/
void read_rs485_raw_data();

/*
.	Send "#01" to get the response data
*/
void send_command();

/*
.	This function will return the formated data string required for sending the data to server
*/
String get_gprs_packet_data(char type);

/*
.	Calibrate the Parameters to find out the sensor value from (4mA - 20mA) Signals.
*/
void calibrate_parameters();

#include <PROCESS1.h>

void setup() {
	// Initializing all the ports
	ISASerial.begin(9600);
	gsmSerial.begin(9600);
	
	DataPort.begin(9600);
	pinMode(RS485_DATA_DIRECTION_PIN, OUTPUT);
	digitalWrite(RS485_DATA_DIRECTION_PIN, HIGH);
	time_initialize();
	set_data_format();
}

void loop(){
	if (check_gprs_transmission_period()){
		
		#if MASTER_STRING
		send_command();       // Used if the convertor needs any command to send the response
		#endif	
		
		read_rs485_raw_data();
		extract_parameter_data();
        
		#if MASTER_STRING
		calibrate_parameters(); // Used if receive 4-20 mA data
		#endif
		transmit_gprs_data();
	}
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

bool check_gprs_transmission_period(){
	
	// check if the time interval has reached then start transmission
	if (TRANSMISSION_INTERVAL < (millis() - g_time)){
		g_time = millis();               // Store the Time for next check
		return 	true;
	}
	else
		return false;
}

void transmit_gprs_data(){
	String gprs_packet_data;
	
	for (char i = 0; i < NUMBER_OF_SERVERS; i++){
		gprs_packet_data = get_gprs_packet_data(i);

		switch (i){
		case 0:   // SPCB Server
			Serial.println("Sending data to SPCB Server");
			if(TEST_MODE)
				gprs_send(gprs_packet_data, i, "TCP", "52.220.52.25", "4567");    // Test TCP Server
			else  
				gprs_send(gprs_packet_data, i, "TCP", "117.239.117.27", "9090");  // SPCB Server
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

void read_rs485_raw_data(){

	char start_byte = '>';             // Starting Delimeter
	char end_byte = ' ';               // End Delimeter
	int start_capture_flag = 0;        
	int end_capture_flag = 0;
	
    #if TEST_MODE
	Serial.println("Receving Data from RS485");
    #endif

	char data_byte;
	while (1)
	{
		if (DataPort.available() > 0)
		{
			data_byte = DataPort.read();
			delay(1);
			if (data_byte == start_byte) 
				start_capture_flag = 1;

			if (start_capture_flag) 
				g_raw_rs485_data += data_byte;

			if (data_byte == end_byte){
				break;
			}
		}
	}
	
	// Double Check if there is any left out, then discard those data bytes
	while (DataPort.available() > 0){
		data_byte = DataPort.read();
	}

	#if TEST_MODE
	Serial.print("Receved Data : ");
	Serial.println(g_raw_rs485_data);       // Sample String : 
	#endif
}

void send_command(){
	digitalWrite(RS485_DATA_DIRECTION_PIN, HIGH);    // Set RS485 Port to Write Mode 
	delay(10);
	DataPort.write(COMMAND_STRING);
	DataPort.write('\x0D');                          // Carriage Return
	DataPort.flush();
	digitalWrite(RS485_DATA_DIRECTION_PIN, LOW);	 // Set RS485 port to Read Mode
	delay(500);
}

void extract_parameter_data(){
	g_raw_rs485_data.replace(" ", "");
	char index[MAX_PARAMETERS+1];  // to hold the indices of the delimeter including string length
	char start = 0;
	bool flag = true;
	
	// find out the postion of all delimeters
	for (char i = 0; i < MAX_PARAMETERS; i++){
		if (flag){
			index[i] = g_raw_rs485_data.indexOf('+', start);
			if (index[i] != -1)
				start = index[i] + 1;
			else{
				flag = false;
				start = 0;
				i--;
			}
		}
		else{
			index[i] = g_raw_rs485_data.indexOf('-', start);
			start = index[i] + 1;
		}
	}
	// Add string length to the index array
	index[MAX_PARAMETERS] = g_raw_rs485_data.length();
	// Sort the array of delimeter positions
	for (char i = 0; i < MAX_PARAMETERS; i++){
		for (char j = 0; j < MAX_PARAMETERS; j++){
			if (index[i] > index[i + 1]){
				char temp = index[i + 1];
				index[i + 1] = index[i];
				index[i] = temp;
			}
		}
	}
   
	// Get the SubStrings using the delimeter and convert them to float
	for (char i = 0; i < MAX_PARAMETERS; i++)
	{
		g_parameter_value[i] = (g_raw_rs485_data.substring(index[i] + 1, index[i + 1])).toFloat();
	}
}

void calibrate_parameters(){
	for (char i = 0; i < MAX_PARAMETERS; i++){
		g_parameter_value[i] = (g_parameter_value[i]-MIN_CURRENT_SIGNAL)*( g_max_concentration[i]/(MAX_CURRENT_SIGNAL-MIN_CURRENT_SIGNAL));
		#if TEST_MODE
		Serial.print(g_parameter_name[i] + " : ");
		Serial.println(g_parameter_value[i]);
        #endif
	}
}