#include <Arduino.h>
// Dependant library I2C_eeprom.h VERSION: 1.2.7
 
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)

 
 static struct DataPacket{
  uint16_t serial;
  uint32_t timestamp;
  uint8_t sensor_id;  
  uint16_t data; //Actual data
} datapacket;

#include <Wire.h> //I2C library
#include <I2C_eeprom.h>

#define EEPROM_SIZE   (32768)
#define EE24LC32MAXBYTES  (EEPROM_SIZE/8)
#define DEVICEADDRESS  (0x57)

class DataLogger {

public:
    uint8_t findData(uint32_t timestamp1, uint32_t timestamp2,    uint8_t sens_id[], uint8_t numsensors, DataPacket * datapacket, uint8_t maxmumdata);
    uint8_t logData(DataPacket datapacket);
	DataPacket getLastWrittenData()	;
	void clearLastWrittenData();
	uint16_t getNextPointer();
	void setNextPointer(uint16_t ); 
	uint8_t getDataSize() ;	
    uint8_t readByte(const uint16_t memoryAddress);
    uint16_t readBlock(const uint16_t memoryAddress, uint8_t* buffer, const uint16_t length);
    uint16_t init();

private:
    uint16_t nextserial; //global 
    uint16_t nextdatapointer; //global
    uint16_t max_address; //global
    uint16_t max_serial; //global 
    uint8_t isvalidTimeStamp(uint32_t tm);   
    uint16_t findLastAddress();
};
 
