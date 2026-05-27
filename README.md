# WALL-F Waste Sorting Robot

<div align="center">

**Autonomous waste-sorting robot built for the UNSW DESN1000 ELEC stream.**

PixyCam vision | Arduino control | RGB sorting | Differential drive | Servo clamp

**2nd place out of 30 teams** with **16 successful deposits in 10 minutes**, all in the correct RGB order, earning a **High Distinction**.

[Algorithm](waste-sorting-algorithm.md) | [Wiring](wiring.md) | [Design Process](design-process.md) | [Final Report](documentation/ELEC_FinalReport_TeamC1.pdf)

</div>

## Overview

WALL-F is an autonomous waste-sorting robot designed to identify, collect, and deposit coloured balls representing different plastic waste categories inside a competition arena. The robot logs its home base colour, searches for red, green, and blue target balls in sequence, captures each ball with a front clamp, returns to base, deposits it, and repeats the cycle.

The final build used a two-level PLA chassis, a removable WALL-E-inspired shell, a PixyCam 2.1 for colour tracking, an ultrasonic sensor for obstacle detection, and an IR sensor inside the clamp for capture confirmation.

## Final Build

| Competition build | Internal layout |
| --- | --- |
| ![Completed WALL-F in the arena](assets/wall-f-arena-front.jpg) | ![WALL-F without shell showing internal layout](assets/wall-f-arena-no-shell.jpg) |

| Full assembly render | No-shell chassis |
| --- | --- |
| ![WALL-F concept render](assets/full-robot-render-isometric.png) | ![No-shell isometric chassis model](assets/chassis-no-shell-isometric.png) |

## What It Does

```text
Log base -> search target -> centre -> chase -> clamp -> return -> deposit -> repeat
```

WALL-F sorts balls in strict RGB order:

```text
RED -> GREEN -> BLUE -> repeat
```

| Subsystem | Role |
| --- | --- |
| Vision | PixyCam 2.1 tracks coloured balls and home-base plates |
| Drive | Differential-drive DC motors execute calibrated movement and turning |
| Obstacle handling | HC-SR04 ultrasonic sensor detects nearby walls and obstacles |
| Capture | Dual FS90MG servos close the front clamp around a detected ball |
| Confirmation | TCRT5000 IR sensor confirms when a ball is inside the clamp |
| Deposit | Robot returns to the logged base colour, opens the clamp, reverses, and continues |

See [waste-sorting-algorithm.md](waste-sorting-algorithm.md) for the full algorithm breakdown.

## Engineering Highlights

- Built around repeated autonomous collection cycles rather than a single scripted run.
- Used PixyCam colour signatures for both target-ball tracking and home-base return.
- Combined proportional visual chasing with timed motor calibration for arena navigation.
- Integrated ultrasonic obstacle handling so the robot could recover from nearby walls.
- Used an IR-confirmed servo clamp so the robot only transitioned after a real capture.
- Debugged electrical integration issues including servo jitter caused by intermittent common-ground connection.
- Finalised an integrated RGB sequence algorithm that performed reliably during competition testing.

## Design Evolution

The design moved from early ball-retrieval concepts to a compact WALL-F gripper robot through concept scoring, mechanical iteration, sensor testing, and final integration.

Key decisions included:

- selecting a servo-actuated clamp over passive scoop and bell-enclosure concepts
- choosing a two-front-wheel and rear-caster layout for simpler turning
- separating the internal drive/sensor chassis from the removable external shell
- validating PixyCam signatures before full mechanical integration
- adding calibration sketches for motors, servos, ultrasonic sensing, IR sensing, and base return

See [design-process.md](design-process.md) for a compact public summary of the proposal, journals, and final design iteration.

## Electrical Schematic

The final circuit schematic is included as both a PNG preview and PDF exports.

![WALL-F final circuit schematic](hardware/schematic/wall-f-schematic.png)

| File | Purpose |
| --- | --- |
| [Schematic PNG](hardware/schematic/wall-f-schematic.png) | GitHub preview image |
| [Schematic PDF](hardware/schematic/wall-f-schematic.pdf) | Full schematic export |
| [No-drawing-sheet PDF](hardware/schematic/wall-f-schematic-no-drawing-sheet.pdf) | Cleaner schematic-only export |
| [Wiring and pin map](wiring.md) | Component wiring, pins, and power notes |

Editable KiCad files will be added under `hardware/kicad/` once exported and checked.

## Documentation

| Document | Purpose |
| --- | --- |
| [Final Report](documentation/ELEC_FinalReport_TeamC1.pdf) | Submitted final report with final design, evaluation, and outcome |
| [Design Proposal](documentation/Design_Proposal.pdf) | Proposal-stage concept, subsystem plan, risks, and project plan |
| [Design Journal 1](documentation/Design_Journal_1.pdf) | Early concept generation, tests, and reflections |
| [Design Journal 2](documentation/Design_Journal_2.pdf) | Later integration, compliance testing, debugging, and final iteration |
| [Design Overview](design-overview.md) | Repo-friendly summary of the final mechanical and electrical design |
| [Design Process](design-process.md) | Compact process summary derived from the proposal and design journals |
| [Visual Assets](assets/README.md) | Photos, CAD renders, and labelled subsystem images |

The public PDFs have been checked for student-number patterns. The design proposal copy excludes the original cover sheet containing student IDs.

## Repository Layout

```text
wall-f/
|-- README.md
|-- design-overview.md
|-- design-process.md
|-- waste-sorting-algorithm.md
|-- wiring.md
|-- documentation/
|   |-- ELEC_FinalReport_TeamC1.pdf
|   |-- Design_Proposal.pdf
|   |-- Design_Journal_1.pdf
|   `-- Design_Journal_2.pdf
|-- finalcode/
|   `-- FINALTESTING/
|       `-- FINALTESTING.ino
|-- calibration/
|   |-- servo-clamp-calibration/
|   |-- motor-turn-calibration/
|   |-- ultrasonic-distance-test/
|   |-- ir-clamp-test/
|   |-- pixy-signature-check/
|   `-- base-return-calibration/
|-- hardware/
|   |-- schematic/
|   `-- kicad/
`-- assets/
```

## Final Code

The final competition sketch is located at:

```text
finalcode/FINALTESTING/FINALTESTING.ino
```

The folder name matches the `.ino` file name so it can be opened directly in the Arduino IDE.

## Calibration Note

The final sketch contains the values used during the competition run. The calibration sketches use `xxxx` placeholders where values should be re-measured for a rebuilt robot, different batteries, surface friction, motor variance, or camera mounting angle.
