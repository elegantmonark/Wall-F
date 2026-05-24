# Calibration Sketches

These sketches are placeholders for recalibrating WALL-F on rebuilt hardware.

They intentionally use `xxxx` values where measurements are required. Replace those placeholders before uploading a calibration sketch to the Arduino.

## Sketches

| Folder | Purpose |
|---|---|
| `servo-clamp-calibration/` | Find open and closed servo angles for both clamp arms |
| `motor-turn-calibration/` | Measure 90-degree, 180-degree, and 360-degree turn timings |
| `ultrasonic-distance-test/` | Verify HC-SR04 readings and obstacle threshold |
| `ir-clamp-test/` | Confirm active LOW clamp detection |
| `pixy-signature-check/` | Verify PixyCam signatures and block positions |
| `base-return-calibration/` | Measure base size thresholds for return/deposit stopping |

The final competition sketch keeps the tested values used during the 10-minute run.

