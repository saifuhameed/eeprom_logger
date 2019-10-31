#include "datalogger.h"
 
I2C_eeprom eeprom (DEVICEADDRESS,EE24LC32MAXBYTES);

uint8_t  DataLogger::readByte(const uint16_t memoryAddress){
    return eeprom.readByte(memoryAddress);
}
uint16_t  DataLogger::readBlock(const uint16_t memoryAddress, uint8_t* buffer, const uint16_t length){
    return eeprom.readBlock(memoryAddress,buffer,length);
}

uint16_t DataLogger::init(){        
        eeprom.begin();
        uint16_t dataSize=sizeof(DataPacket);
        max_address=((EEPROM_SIZE/dataSize/8u)*dataSize)-dataSize; //will give last address that can be safely written without overflow
        max_serial= EEPROM_SIZE/dataSize/8u;
        nextdatapointer=findLastAddress();
        return nextdatapointer;
}

uint8_t DataLogger::isvalidTimeStamp(uint32_t tm){
  // FROM Saturday, January 1, 2000 0:00:00 
  //TO Friday, December 31, 2100 0:00:00
  return ((tm>=946684800U) && (tm<4133894400));
}

uint16_t DataLogger::findLastAddress(){
  uint16_t dataSize=sizeof(DataPacket);  
  nextserial=1; //default starting serial number
  uint32_t last_timestamp=0;
  DataPacket datapacket; 
  for (uint16_t i=0; i<=max_address;i+=dataSize){ 
    eeprom.readBlock(i,(unsigned char*)&datapacket,sizeof(datapacket)); 
    if((datapacket.serial==nextserial) && (datapacket.timestamp>=last_timestamp) && isvalidTimeStamp(datapacket.timestamp)){
        last_timestamp=datapacket.timestamp;
        nextserial++;         
    }else{
      DEBUG_PRINT(F("\nnextserial= "));
      DEBUG_PRINTLN(nextserial);     
      return i;
    } 
    yield();
  }  
  nextserial=1;
  DEBUG_PRINTLN(F("EEPROM Memory full, so writing back from zero"));
  return 0;
}
 

//@ Returns serial used in this log
uint8_t DataLogger::logData(DataPacket datapacket){
  datapacket.serial=nextserial; 
  int r=eeprom.writeBlock(nextdatapointer,(unsigned char*)&datapacket,sizeof(datapacket));  
  if(r==0){ //writeBlock return 0 if write OK else non zero
    nextdatapointer+=sizeof(DataPacket); 
    if(nextdatapointer>max_address) { //MEMORY FULL WRITE BACK FROM ZERO
      nextdatapointer=  0;
      nextserial=1;
      return 1; //Restart serial from 1;
    }else{
        return   ++nextserial;
    }    
  }
  return 0;
}

uint8_t DataLogger::getDataSize(){
      return sizeof(DataPacket);
}

uint16_t DataLogger::getNextPointer(){
      return nextdatapointer;
}

void DataLogger::setNextPointer(uint16_t dpointer){
       nextdatapointer=dpointer;
}

DataPacket DataLogger::getLastWrittenData(){
  DataPacket datapacket;
  datapacket.serial=0;
	if(nextdatapointer>0){		
		eeprom.readBlock(nextdatapointer-sizeof(datapacket),(unsigned char*)&datapacket,sizeof(datapacket));			
	}	
	return datapacket;
}
void DataLogger::clearLastWrittenData(){
	uint16_t sz=getDataSize();
	if(nextdatapointer>=sz){
		unsigned char buffer[sz];
		for(int8_t i=0;i<sz;i++){
			buffer[i]=0xFF;
		}		
		eeprom.writeBlock(nextdatapointer-sz,&buffer[0],sz);			
		nextdatapointer=nextdatapointer-sz;
	}	
}

uint8_t DataLogger::findData (uint32_t timestamp1, uint32_t timestamp2,  uint8_t sens_id[], uint8_t numsensors, DataPacket * datapackets,uint8_t maxmumdata){
       
    uint16_t dataSize=sizeof(DataPacket);
    uint8_t circular=false;
    DataPacket datapacket1,datapacket2;
    eeprom.readBlock(0,(unsigned char*)&datapacket1,sizeof(datapacket1));  //To check for circular data  
    eeprom.readBlock(max_address,(unsigned char*)&datapacket2,sizeof(datapacket2));    //To check for circular data
    uint8_t foundnums=0;
    if((datapacket1.timestamp>datapacket2.timestamp) && (datapacket2.serial==max_serial &&  nextdatapointer !=max_address)){ //To check for circular data
        circular=true;        
    }   

    if(circular){
      for (uint16_t i=nextdatapointer; i<max_address;i+=dataSize){ //Start from previous data stored in circular way till last possible address
        DataPacket datapacket;
        eeprom.readBlock(i,(unsigned char*)&datapacket,sizeof(datapacket));  
        if(datapacket.timestamp>=timestamp1 &&  datapacket.timestamp<=timestamp2){          
            if(sens_id!=NULL){
              for (uint8_t a=0;a<numsensors;a++){                 
                if ( datapacket.sensor_id== sens_id[a] ){                
                  datapackets[foundnums]=datapacket;
                  foundnums++;
                  if(foundnums==maxmumdata) return foundnums;
                }
              }
            }else{              
                datapackets[foundnums]=datapacket;
                foundnums++; 
                if(foundnums==maxmumdata) return foundnums;             
            } 
        }
        yield();
      }
    }
   
    for (uint16_t i=0; i<=(nextdatapointer-dataSize);i+=dataSize){ //check from zero till last written data.
        DataPacket datapacket;   
        eeprom.readBlock(i,(unsigned char*)&datapacket,sizeof(datapacket));         
        
        if(datapacket.timestamp>=timestamp1 &&  datapacket.timestamp<=timestamp2){          
            if(sens_id!=NULL){
              for (uint8_t a=0;a<numsensors;a++){
                if (datapacket.sensor_id==sens_id[a]){
                  datapackets[foundnums]=datapacket;
                  foundnums++;
                  if(foundnums==maxmumdata) return foundnums;
                }
              }
            }else{
                datapackets[foundnums]=datapacket;
                foundnums++;
                if(foundnums==maxmumdata) return foundnums;
            }           
        }
        yield();
    }  
  return foundnums;
}
