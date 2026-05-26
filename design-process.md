# WALL-F Design Process

This document condenses the design proposal and design journals into a repo-friendly summary of how WALL-F evolved from early concepts into the final competition robot.

## 1. Problem Framing

The project goal was to build a self-contained autonomous robot that could sort coloured balls representing plastic waste categories inside a defined arena. The robot needed to identify target colours, avoid non-target material, collect balls, return to its base, and deposit in the required order.

The main engineering constraints were:

- Fit within a 200 mm x 200 mm x 200 mm size envelope.
- Use the provided DESN1000 kit plus limited additional materials.
- Operate autonomously during testing.
- Reliably detect coloured balls and base plates.
- Handle repeated collection cycles within a 10-minute run.

## 2. Early Concept Exploration

The team explored several ball-retrieval and chassis concepts before committing to the final architecture:

- Passive funnel scoop with internal storage.
- Descending bell/enclosure mechanism.
- Servo-actuated clamp around the target ball.
- Three-wheel layout with a rear caster.
- Four-wheel layout for stability.
- WALL-E-inspired tank-track style concept.

The servo clamp and rear-caster layout became the most practical direction because it kept the mechanism simple, controllable, and easier to calibrate within the project timeline.

## 3. Sensor And Subsystem Development

Early work focused on validating the sensing stack before full robot integration.

- PixyCam colour signatures were tested against red and green sample balls before the final foam balls were available.
- HC-SR04 ultrasonic distance sensing was simulated and then physically tested.
- TCRT5000 IR sensing was evaluated for close-range ball capture confirmation.
- Servo sweep tests established the usable 0-180 degree range for the clamp mechanism.

These tests shaped the final sensor placement: PixyCam and ultrasonic at the front, with the IR sensor positioned inside the clamp region.

## 4. Compliance Testing Pivot

During compliance-test planning, the team compared subsystem demonstration options. The perception/navigation path was selected as the most realistic route because the PixyCam colour signatures, motor driver, and ultrasonic obstacle logic were already progressing.

This decision pushed development toward:

- base recognition,
- obstacle detection,
- PixyCam-guided centring,
- calibrated motor movement,
- LCD status feedback,
- and a repeatable navigation loop.

## 5. Mechanical Iteration

The robot moved from early sketches and isolated tests into a two-level PLA chassis.

Important mechanical decisions:

- Two front drive wheels with a rear caster for manoeuvrability.
- A two-level chassis separating drive/power-heavy components from control electronics.
- A front clamp shaped around the fixed ball size used in the arena.
- A removable WALL-E-inspired shell for final presentation.
- A dedicated battery mount to reduce battery movement during turns.

In testing, the rear caster improved turning consistency compared with earlier assumptions that could have been compensated for incorrectly in software.

## 6. Electrical And Integration Debugging

The final electrical architecture used separate power rails for the Arduino, motors, and servos while tying all grounds together.

Key integration lessons:

- Servo jitter was traced to an intermittent common-ground connection.
- The I2C LCD reduced pin pressure on the Arduino Uno.
- PixyCam SPI pins constrained the remaining sensor pin assignments.
- Large capacitors were added for transient smoothing.
- Servo power was separated from the Arduino 5 V rail to avoid brownouts.

These changes made the final robot more stable during repeated motor and clamp actuation.

## 7. Algorithm Evolution

The final software evolved from component tests into a repeated mission cycle:

```text
Log base -> search target -> centre -> chase -> clamp -> return -> deposit -> repeat
```

Important algorithmic choices:

- Log the home base before leaving it.
- Use strict RGB target ordering.
- Centre targets using PixyCam x-position error.
- Slow down as the target block grows larger.
- Confirm capture using the clamp IR sensor.
- Use ultrasonic obstacle handling during general navigation.
- Suppress obstacle handling while intentionally approaching the base.
- Use base apparent size to estimate return arrival.

The final sketch is documented in [waste-sorting-algorithm.md](waste-sorting-algorithm.md).

## 8. Final Outcome

The final competition robot completed 16 successful deposits in 10 minutes, deposited balls in the correct RGB sequence, placed 2nd out of 30 teams, and earned a High Distinction result.

The result came from the combined reliability of the PixyCam tracking, calibrated movement, clamp capture confirmation, obstacle handling, and return-to-base logic.

## Source Documents

- [Design Proposal](documentation/Design_Proposal.pdf)
- [Design Journal 1](documentation/Design_Journal_1.pdf)
- [Design Journal 2](documentation/Design_Journal_2.pdf)
- [Final Report](documentation/ELEC_FinalReport_TeamC1.pdf)
