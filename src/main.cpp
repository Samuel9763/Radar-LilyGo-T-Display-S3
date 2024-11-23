#include <Arduino.h>
#include <TFT_eSPI.h>
#include <Button2.h>   // Button handling library
#include "page1.h"
#include "page2.h"
#include "page3.h"
#include "page4.h"
#include "page5.h"
#include "RadarData.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite background = TFT_eSprite(&tft);  // Background sprite shared across pages

// UART setup
HardwareSerial uart(1);  // Use UART1
const int RX_PIN = 17;
const int TX_PIN = 18;
const int BAUD_RATE = 115200;
//const int BAUD_RATE = 38400;
const int EXPECTED_DATA_SIZE = 96; // Expected data size
uint8_t uartBuffer[EXPECTED_DATA_SIZE];  // Buffer for received UART data
RadarData radarData;
// Define the expected sync header bytes
//const uint8_t expectedSyncHeader[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
const uint8_t expectedSyncHeader[] = {0x02, 0x01, 0x04, 0x03, 0x06, 0x05, 0x08, 0x07};
// Global counters for tracking success rate
int totalPacketsReceived = 0;
int successfulParses = 0;

int currentPage = 5;
const int totalPages = 5;            // Total number of pages

// Pin definitions for buttons
Button2 buttonUp(14);                 // Up button on GPIO0
Button2 buttonDown(0);              // Down button on GPIO14

void showPage(int page) {
  switch (page) {
    case 1: showPage1(tft, background,radarData); break;
    case 2: showPage2(tft, background,radarData); break;
    case 3: showPage3(tft, background,radarData); break;
    case 4: showPage4(tft, background,radarData); break;
    case 5: showPage5(tft, background,radarData); break;
  }
}
void handleUp(Button2 &btn) {
    currentPage--;
    if (currentPage < 1) currentPage = totalPages;  // Wrap around to the last page
    showPage(currentPage);
}

void handleDown(Button2 &btn) {
    currentPage++;
    if (currentPage > totalPages) currentPage = 1;  // Wrap around to the first page
    showPage(currentPage);
}


// Separate function: Logs the parsed RadarData
void printLog(const RadarData& data) {
    printf("Parsed RadarData:\n");
    printf("  Sync Header: 0x%04X, 0x%04X, 0x%04X, 0x%04X\n",
           data.syncHeader[0], data.syncHeader[1],
           data.syncHeader[2], data.syncHeader[3]);
    printf("  Software Version: %u\n", data.softwareVersion);
    printf("  Total Packet Length: %u\n", data.totalPacketLength);
    printf("  Hardware Version: %u\n", data.hardwareVersion);
    printf("  Frame Number: %u\n", data.frameNumber);
    printf("  CPU Time: %u\n", data.cpuTime);
    printf("  Person Detected: %u\n", data.humanDetected);
    printf("  Output Structure Count: %u\n", data.outputStructureCount);
    printf("  Reserved: %u\n", data.reserved);
    printf("  Structure Type: %u\n", data.structureType);
    printf("  Structure Length: %u\n", data.structureLength);
    printf("  Max RCS: %.2f\n", data.maxRCS);
    printf("  Heart Rate Estimate: %.2f\n", data.heartRateEst);
    printf("  Breathing Rate Estimate: %.2f\n", data.breathingRateEst);
    printf("  Sum Energy Breath Waveform: %.2f\n", data.sumEnergyBreathWfm);
    printf("  Sum Energy Heart Waveform: %.2f\n", data.sumEnergyHeartWfm);

    // Calculate and log success rate
    float successRate = (successfulParses / (float)totalPacketsReceived) * 100.0f;
    printf("Success Rate: %.2f%% (%d/%d)\n", successRate, successfulParses, totalPacketsReceived);
}

void printLog2(const RadarData& data) {
    // Print CSV log row
    printf("%04X,%04X,%04X,%04X,%u,%u,%u,%u,%u,%u,%u,%u,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
           data.syncHeader[0], data.syncHeader[1], data.syncHeader[2], data.syncHeader[3],
           data.softwareVersion, data.totalPacketLength, data.hardwareVersion, data.frameNumber,
           data.cpuTime, data.humanDetected, data.outputStructureCount, data.reserved,
           data.maxRCS, data.heartRateEst, data.breathingRateEst,
           data.sumEnergyBreathWfm, data.sumEnergyHeartWfm, (float)totalPacketsReceived);
}

void printLog3Header() {
    // Print CSV column headers
    printf("syncHeader0,syncHeader1,syncHeader2,syncHeader3,"
           "softwareVersion,totalPacketLength,hardwareVersion,frameNumber,"
           "cpuTime,humanDetected,outputStructureCount,reserved,"
           "maxRCS,heartRateEst,breathingRateEst,"
           "sumEnergyBreathWfm,sumEnergyHeartWfm,totalPacketsReceived\n");
}

void printLog3(const RadarData& data) {
    // Calculate success rate
    float successRate = (successfulParses / (float)totalPacketsReceived) * 100.0f;

    // Print CSV log row
    printf("%04X,%04X,%04X,%04X,%u,%u,%u,%u,%u,%u,%u,%u,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
           data.syncHeader[0], data.syncHeader[1], data.syncHeader[2], data.syncHeader[3],
           data.softwareVersion, data.totalPacketLength, data.hardwareVersion, data.frameNumber,
           data.cpuTime, data.humanDetected, data.outputStructureCount, data.reserved,
           data.maxRCS, data.heartRateEst, data.breathingRateEst,
           data.sumEnergyBreathWfm, data.sumEnergyHeartWfm, (float)totalPacketsReceived);
}


bool readUARTData() {

	// Increment total packets received counter
	totalPacketsReceived++;
	
	for (int j = 0; j < ((EXPECTED_DATA_SIZE-8)/8); j++) {
		static int syncPos = 0;  // Tracks the current position in the sync header comparison
		uart.readBytes(uartBuffer, 8);  // Read UART data into buffe
		for (int i = 0; i < 8; i++) {
		 
			if (uartBuffer[i] == expectedSyncHeader[syncPos]) {
				syncPos++;
				if (syncPos == sizeof(expectedSyncHeader) / sizeof(expectedSyncHeader[0])) {
					if (uart.available() >= EXPECTED_DATA_SIZE-8) {
						uart.readBytes(&uartBuffer[8], EXPECTED_DATA_SIZE-8);
						//printf("Sync header matched. Parsing RadarData structure.\n");
						memcpy(&radarData, &uartBuffer[0], sizeof(RadarData));
						successfulParses++;
						syncPos = 0;
						return true;
					}
				}
			} else {
				syncPos = 0;  // Reset if header mismatch occurs
				break;
			}
		}
	}
    return false;  // Failed to parse a valid packet
}


bool readUARTData_old() {
    static int syncPos = 0;  // Tracks the current position in the sync header comparison

    if (uart.available() >= EXPECTED_DATA_SIZE) {
        uart.readBytes(uartBuffer, EXPECTED_DATA_SIZE);  // Read UART data into buffer

        // Increment total packets received counter
        totalPacketsReceived++;
        
        // Debug: Print UART buffer in 0xXX format
        /*
        printf("UART Buffer: ");
        for (int i = 0; i < EXPECTED_DATA_SIZE; i++) {
            printf("0x%02X", uartBuffer[i]);
            if (i < EXPECTED_DATA_SIZE - 1) printf(", ");
        }
        printf("\n");
        */
        // Rolling sync header match
        for (int i = 0; i < EXPECTED_DATA_SIZE; i++) {
            //printf("Checking byte 0x%02X at position %d (syncPos = %d)\n", uartBuffer[i], i, syncPos);
            
            if (uartBuffer[i] == expectedSyncHeader[syncPos]) {
                syncPos++;
                if (syncPos == sizeof(expectedSyncHeader) / sizeof(expectedSyncHeader[0])) {
                    // Sync header matched
                    //printf("Sync header matched. Parsing RadarData structure.\n");

                    // Parse radarData from buffer
                    memcpy(&radarData, &uartBuffer[i - syncPos + 1], sizeof(RadarData));
                    
                    successfulParses++;
                    // Reset sync position
                    syncPos = 0;
                    return true;
                }
            } else {
                syncPos = 0;  // Reset if header mismatch occurs
            }
        }
    }
    return false;  // Failed to parse a valid packet
}



void setup() {
    tft.init();
    tft.setRotation(3);
    background.createSprite(tft.width(), tft.height());
    background.fillSprite(TFT_BLACK);
    // Configure buttons
    buttonUp.setPressedHandler(handleUp);
    buttonDown.setPressedHandler(handleDown);

    // UART initialization
    uart.begin(BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);
    
    printLog3Header();
    // Display the initial page
    showPage(currentPage);
}

void loop() {
    buttonUp.loop();     // Poll the up button
    buttonDown.loop();   // Poll the down button
    //readUARTData();
    if (readUARTData()) {   
        printLog3(radarData);  // Log parsed data
    } else {
        printf("Failed to parse a valid packet.\n");
    }
    showPage(currentPage);
    /*
    if (readUARTData()) {
        // If new UART data is received, show the page with updated data
        showPage(currentPage);
    } else {
        // If no data is received, show page 5 (error or "no data" page)
        //showPage(5);
    }
    */
}
