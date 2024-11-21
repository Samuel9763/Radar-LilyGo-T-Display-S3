#ifndef RADAR_DATA_H
#define RADAR_DATA_H

#include <stdint.h>


struct RadarData {
// Frame Header
    uint16_t syncHeader[4];       // Sync Header: 0x0102, 0x0304, 0x0506, 0x0708
    uint32_t softwareVersion;     // Software version number
    uint32_t totalPacketLength;   // Total packet length (fixed at 96 bytes)
    uint32_t hardwareVersion;     // Hardware version number
    uint32_t frameNumber;         // Current frame sequence number
    uint32_t cpuTime;             // CPU time
    uint32_t humanDetected;       // Human detection status: 0 (no human), 1 (human detected)
    uint32_t outputStructureCount;// Number of output structures (always 1)
    uint32_t reserved;            // Reserved field (always 0)

    // Output Structure
    uint32_t structureType;       // Structure type (6)
    uint32_t structureLength;     // Structure length (0x14 or 20 bytes)

    // Measurements
    float maxRCS;                 // Maximum Radar Cross Section (RCS) or echo strength
    float heartRateEst;           // Heart rate estimation output
    float breathingRateEst;       // Breathing rate estimation output
    float sumEnergyBreathWfm;     // Total energy of breathing waveform (decays when no human is detected)
    float sumEnergyHeartWfm;      // Total energy of heart waveform (decays when no human is detected)
};

/*
struct RadarData {
    // Frame Header
    uint16_t syncHeader[4];       // 0x0102, 0x0304, 0x0506, 0x0708
    uint32_t softwareVersion;     // Software version number
    uint32_t totalPacketLength;   // Total bytes including frame header
    uint32_t hardwareVersion;     // Hardware version number
    uint32_t frameNumber;         // Current frame sequence number
    uint32_t cpuTime;             // CPU time
    uint32_t targetCount;         // Number of detected targets
    uint32_t outputStructureCount;// Always 2
    uint32_t reserved;            // Always 0

    // Target Data
    uint32_t structureType;       // Always 6
    uint32_t structureLength;     // 0x80 (128 bytes)
    uint16_t rangeBinIndexMax;    // Maximum range bin index
    uint16_t rangeBinIndexPhase;  // Phase range bin index
    float maxVal;                 // Maximum value
    uint32_t processingCyclesOut; // Number of processing cycles
    uint16_t rangeBinStartIndex;  // Starting index of range bin
    uint16_t rangeBinEndIndex;    // Ending index of range bin
    float unwrapPhasePeak_mm;     // Phase peak value in mm
    float outputFilterBreathOut;  // Filtered breathing output
    float outputFilterHeartOut;   // Filtered heart output
    float heartRateEst_FFT;       // Heart rate estimation using FFT
    float heartRateEst_FFT_4Hz;   // 4Hz heart rate FFT estimation
    float heartRateEst_xCorr;     // Heart rate cross-correlation estimation
    float heartRateEst_peakCount_filtered; // Filtered peak count for heart rate
    float breathingRateEst_FFT;   // Breathing rate estimation using FFT
    float breathingRateEst_xCorr; // Breathing rate cross-correlation estimation
    float breathingRateEst_peakCount; // Peak count for breathing rate
    float confidenceMetricBreathOut; // Confidence metric for breathing output
    float confidenceMetricBreathOut_xCorr; // Cross-correlation confidence metric for breathing
    float confidenceMetricHeartOut; // Confidence metric for heart output
    float confidenceMetricHeartOut_4Hz; // 4Hz confidence metric for heart output
    float confidenceMetricHeartOut_xCorr; // Cross-correlation confidence metric for heart
    float sumEnergyBreathWfm;     // Total energy of breathing waveform
    float sumEnergyHeartWfm;      // Total energy of heart waveform
    float motionDetectedFlag;     // Flag indicating motion detection
    float breathingRateEst_harmonicEnergy; // Harmonic energy estimation for breathing rate
    float heartRateEst_harmonicEnergy;    // Harmonic energy estimation for heart rate
}; 
*/
#endif // RADAR_DATA_H
