# WALL-F Waste Sorting Algorithm

This document describes the algorithm implemented in `finalcode/FINALTESTING/FINALTESTING.ino`.

## Colour Signatures

The PixyCam 2.1 is used in Colour Connected Components mode.

| Signature | Meaning |
|---:|---|
| 1 | Red ball |
| 2 | Green ball |
| 3 | Blue ball |
| 4 | Orange base |
| 5 | Yellow base |
| 6 | Pink base |
| 7 | Purple base |

The main loop runs the collection order:

```text
RED -> GREEN -> BLUE -> repeat
```

## Startup Sequence

1. Configure motor, ultrasonic, IR, servo, LCD, and PixyCam interfaces.
2. Stop both motors.
3. Attach both clamp servos.
4. Open the clamp to establish a known mechanical state.
5. Initialise the LCD and PixyCam.
6. Keep the PixyCam lamp off.
7. Scan for the home base before moving.
8. Store the detected base signature, width, height, and area.
9. Reverse briefly away from the base.
10. Enter the repeating mission loop.

The base is logged before movement so the robot knows which colour plate to return to after collecting a ball.

## One Colour Cycle

Each colour cycle is handled by `doOneColourCycle(...)`.

1. Display the target colour on the LCD.
2. Open the clamp.
3. If the robot is still facing the base, turn 180 degrees to face the arena.
4. Perform an initial 180-degree sweep for the target ball.
5. If the target is not found, drive forward briefly.
6. Search until the target signature is found.
7. Centre on the target using PixyCam x-position.
8. Chase the target using proportional steering.
9. Confirm capture with the clamp IR sensor.
10. Close the clamp.
11. Search for the logged home base.
12. Centre on the base.
13. Drive toward the base until its apparent PixyCam size reaches the stored startup reference.
14. Inch forward for the deposit.
15. Open the clamp to release the ball.
16. Reverse away from the base.
17. Turn 180 degrees back toward the arena.
18. Continue to the next colour.

## Search Behaviour

The search routine is forward-biased. The robot first checks whether the target is already visible. If not, it drives forward in short steps while repeatedly checking the PixyCam.

At intervals, it performs a 180-degree sweep. If multiple sweeps fail, it escalates to a full sweep using the configured `TURN_360_MS` timing. These turn timings are calibration-dependent and should be re-measured if the robot is rebuilt.

## Chase Behaviour

The chase routine uses proportional steering from the target's horizontal position in the PixyCam frame:

```text
error = target_x - frame_center_x
correction = STEER_KP * error
left_pwm = base_speed + correction
right_pwm = base_speed - correction
```

When the target block appears large, the robot slows down to approach more carefully. If the target is lost briefly, the robot rotates toward the side where it was last seen. If it remains lost, it performs local sweep recovery.

## Capture Behaviour

The clamp starts open. During chase, the TCRT5000 IR sensor checks whether a ball has entered the clamp area.

The code assumes the IR sensor is active LOW:

```text
LOW = ball detected
HIGH = no ball detected
```

Once a ball is detected, the robot stops and closes both servos around the ball.

## Obstacle Behaviour

Obstacle detection uses the HC-SR04 ultrasonic sensor with median filtering. The current competition threshold is 10 cm.

If an obstacle is detected during normal navigation:

1. Stop.
2. Wait up to 3 seconds for the path to clear.
3. If clear, resume the current task.
4. If still blocked, reverse slightly.
5. Perform a 360-degree target scan.
6. If the target is not found, turn 180 degrees and drive forward briefly.
7. Resume search.

Obstacle handling is suppressed while intentionally approaching the home base. Otherwise, the base itself would be treated as an obstacle.

## Return And Deposit

After capture, the robot searches for the base signature recorded during startup. It performs a full sweep until the base is found, centres on it, then approaches using the same PixyCam proportional steering style.

Arrival is estimated from the base's apparent size. The robot compares the current base block width, height, and area against the stored startup values.

Once arrival is detected:

1. Stop.
2. Inch forward for a calibrated time.
3. Open the clamp.
4. Reverse away from the base.
5. Turn 180 degrees toward the arena.
6. Mark itself as facing the arena for the next cycle.

## Calibration-Dependent Values

The final competition sketch contains real tested values, but these should be recalibrated for a rebuilt robot:

- Servo open and closed angles
- 90-degree, 180-degree, and 360-degree turn timings
- Forward, approach, inching, and turning PWM values
- PixyCam centre x-position and centring deadband
- Base return size thresholds
- Deposit inch-forward time
- Obstacle threshold and reverse timing

See the `calibration/` folder for placeholder sketches.

