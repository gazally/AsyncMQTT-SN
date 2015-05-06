/*
 * MqttsnClient.cpp
 *                      The BSD License
 *
 *           Copyright (c) 2015, tomoaki@tomy-tech.com
 *                    All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  Created on: 2015/04/19
 *    Modified: 
 *      Author: tomoaki
 *     Version: 0.0.1
 */

#ifdef ARDUINO
        #include <MqttsnClientApp.h>
        #include <GwProxy.h>
        #include <MqttsnClient.h>
#else
        #include "MqttsnClientApp.h"
        #include "GwProxy.h"
        #include "MqttsnClient.h"
#endif

#if defined(ARDUINO) && ARDUINO >= 100
        #include "Arduino.h"
        #include <inttypes.h>
#endif

#if defined(ARDUINO) && ARDUINO < 100
        #include "WProgram.h"
        #include <inttypes.h>
#endif
#include <string.h>
#include <stdio.h>

using namespace std;
using namespace tomyAsyncClient;

extern void setup(void);
extern void loop(void);
extern void interruptCallback(void);
extern APP_CONFIG theAppConfig;
extern TaskList theTaskList[];
extern MqttsnClient* theCleint;
extern void test();
/*=====================================
          MqttsnClient
 ======================================*/
MqttsnClient* theClient = new MqttsnClient();

#if !defined(ARDUINO) && !defined(MQTTSN_TEST)
int main(int argc, char** argv){
    setup();
    loop();
}
#endif

#if !defined(ARDUINO) && defined(MQTTSN_TEST)
int main(int argc, char** argv){
    test();
}
#endif

void loop(){
    theClient->registerInt0Callback(interruptCallback);
	theClient->addTask();
	theClient->initialize((APP_CONFIG) theAppConfig);
	theClient->setSleepMode(theAppConfig.mqttsnCfg.sleep);

	while(true){
		theClient->run();
	}
}

/*=====================================
        Class MqttsnClient
 ======================================*/
MqttsnClient::MqttsnClient(){
    _intCallback = 0;
    theClient = this;
}

MqttsnClient::~MqttsnClient(){
    
}

void MqttsnClient::initialize(APP_CONFIG config){
    _gwProxy.initialize(config);
}

void MqttsnClient::registerInt0Callback(void (*callback)()){
    _intCallback = callback;
}

void MqttsnClient::addTask(void){
    _taskMgr.add(theTaskList);
}

void MqttsnClient::setSleepMode(bool mode){
    _sleepMode = mode;
}

GwProxy* MqttsnClient::getGwProxy(void){
    return &_gwProxy;
}

PublishManager* MqttsnClient::getPublishManager(void){
    return &_pubMgr;
};

SubscribeManager* MqttsnClient::getSubscribeManager(void){
    return &_subMgr;
};

RegisterManager*  MqttsnClient::getRegisterManager(void){
	return _gwProxy.getRegisterManager();
}

TopicTable* MqttsnClient::getTopicTable(void){
    return _gwProxy.getTopicTable();
}

int MqttsnClient::publish(const char* topicName, Payload* payload, uint8_t qos, bool retain){
    return _pubMgr.publish(topicName, payload, qos, retain);
}

void MqttsnClient::subscribe(const char* topicName, TopicCallback onPublish, uint8_t qos){
    _subMgr.subscribe(topicName, onPublish, qos);
}

void MqttsnClient::subscribe(uint16_t topicId, TopicCallback onPublish, uint8_t qos, uint8_t topicType){
    _subMgr.subscribe(topicId, onPublish, qos, topicType);
}

void  MqttsnClient::unsubscribe(const char* topicName){
    _subMgr.unsubscribe(topicName);
}

void MqttsnClient::disconnect(uint16_t sleepInSecs){
    _gwProxy.disconnect(sleepInSecs);
}
void MqttsnClient::run(void){
    _taskMgr.run();
    if (sleep()){
        _intCallback();
    }
}

#ifdef ARDUINO
int MqttsnClient::sleep(void){
	// Enter idle state for 8 s with the rest of peripherals turned off
	// Each microcontroller comes with different number of peripherals
	// Comment off line of code where necessary

	// ATmega328P, ATmega168
	LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF,
				SPI_OFF, USART0_OFF, TWI_OFF);

	// ATmega32U4
	//LowPower.idle(SLEEP_8S, ADC_OFF, TIMER4_OFF, TIMER3_OFF, TIMER1_OFF,
	//		  TIMER0_OFF, SPI_OFF, USART1_OFF, TWI_OFF, USB_OFF);

	// ATmega2560
	//LowPower.idle(SLEEP_8S, ADC_OFF, TIMER5_OFF, TIMER4_OFF, TIMER3_OFF,
	//		  TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART3_OFF,
	//		  USART2_OFF, USART1_OFF, USART0_OFF, TWI_OFF);
	return 0;
}

#else
int MqttsnClient::sleep(void){
    return 0;
}
#endif