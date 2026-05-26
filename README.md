# WALL-F Waste Sorting Robot

WALL-F is an autonomous waste-sorting robot built for the ELEC stream of UNSW DESN1000. The robot was designed to identify, collect, and deposit coloured balls representing different plastic waste categories inside a competition arena.

**Final testing result:** WALL-F placed **2nd out of 30 teams**, completing **16 successful ball deposits in 10 minutes**. The robot deposited balls in the correct RGB order and earned a **High Distinction** result.

## Contents

- [Overview](#overview)
- [Final Build](#final-build)
- [How It Works](#how-it-works)
- [Design Evolution](#design-evolution)
- [Electrical Schematic](#electrical-schematic)
- [Documentation](#documentation)
- [Repository Layout](#repository-layout)
- [Final Code](#final-code)
- [Calibration Note](#calibration-note)

## Overview

WALL-F combines colour-based vision, differential drive navigation, obstacle handling, and a servo-actuated clamp to complete repeated collection cycles. The robot first logs its home base colour, then searches the arena for red, green, and blue target balls in sequence. After capturing a ball, it returns to the logged base and deposits it before continuing the next cycle.

The final build used a two-level PLA chassis, a removable WALL-E-inspired shell, a PixyCam 2.1 for colour tracking, an ultrasonic sensor for obstacle detection, and an IR sensor inside the clamp for capture confirmation.

## Final Build

| Completed robot | Internal layout |
| --- | --- |
| ![Completed WALL-F in the arena](assets/wall-f-arena-front.jpg) | ![WALL-F without shell showing internal layout](assets/wall-f-arena-no-shell.jpg) |

| Concept render | No-shell chassis CAD |
| --- | --- |
| ![WALL-F concept render](assets/full-robot-render-isometric.png) | ![No-shell isometric chassis model](assets/chassis-no-shell-isometric.png) |

## How It Works

```text
Log base -> search target -> centre -> chase -> clamp -> return -> deposit -> repeat
```

The robot collects balls in strict RGB order:

```text
RED -> GREEN -> BLUE -> repeat
```

Core subsystems:

- **Vision:** PixyCam 2.1 tracks coloured balls and home-base plates.
- **Navigation:** differential drive with calibrated forward, reverse, and turn timings.
- **Obstacle handling:** HC-SR04 ultrasonic distance sensing with filtered readings.
- **Capture:** two FS90MG servos close a front clamp once the TCRT5000 IR sensor confirms a ball is inside.
- **Return:** the robot searches for the logged base colour, centres on it, approaches, deposits, reverses, and resumes the next cycle.

See [waste-sorting-algorithm.md](waste-sorting-algorithm.md) for the full algorithm breakdown.

## Design Evolution

- Started with several ball retrieval concepts, including a passive funnel scoop, descending bell enclosure, and servo-actuated clamp.
- Used concept comparison and Pugh-matrix style evaluation to select the WALL-F servo-gripper concept.
- Chose a two-front-wheel and rear-caster layout for simpler turning and improved manoeuvrability.
- Validated PixyCam colour signatures early, then integrated ultrasonic obstacle detection and IR clamp capture sensing.
- Moved from isolated breadboard/component tests to an assembled PLA chassis with mounted sensors and drive hardware.
- Debugged electrical integration issues including servo jitter caused by intermittent common-ground connection.
- Finalised an integrated algorithm combining base logging, RGB sequencing, proportional PixyCam chasing, clamp capture, obstacle handling, and return-to-base deposit.

See [design-process.md](design-process.md) for the compact engineering-process summary.

## Electrical Schematic

The final circuit schematic is included as a PNG preview and a PDF reference extracted from the submitted report.

![WALL-F final circuit schematic](hardware/schematic/wall-f-schematic.png)

- [Schematic PNG](hardware/schematic/wall-f-schematic.png)
- [Schematic PDF reference](hardware/schematic/wall-f-schematic-from-report-page.pdf)
- [Wiring and pin map](wiring.md)

## Documentation

| Document | Purpose |
| --- | --- |
| [Final Report](documentation/ELEC_FinalReport_TeamC1.pdf) | Submitted final report with final design, evaluation, and outcome. |
| [Design Proposal](documentation/Design_Proposal.pdf) | Proposal-stage design concept, subsystem plan, risks, and project plan. |
| [Design Journal 1](documentation/Design_Journal_1.pdf) | Early design process, concept generation, initial tests, and reflections. |
| [Design Journal 2](documentation/Design_Journal_2.pdf) | Later design process, compliance testing, integration, debugging, and final iteration. |
| [Design Overview](design-overview.md) | Repo-friendly summary of the final mechanical and electrical design. |
| [Design Process](design-process.md) | Compact process summary derived from the proposal and design journals. |

The public PDFs have been checked for student-number patterns. The design proposal copy excludes the original cover sheet containing student IDs.

## Repository Layout

```text
wall-f/
+-- README.md
+-- design-overview.md
+-- design-process.md
+-- waste-sorting-algorithm.md
+-- wiring.md
+-- documentation/
|   +-- ELEC_FinalReport_TeamC1.pdf
|   +-- Design_Proposal.pdf
|   +-- Design_Journal_1.pdf
|   +-- Design_Journal_2.pdf
+-- finalcode/
|   +-- FINALTESTING/
|       +-- FINALTESTING.ino
+-- calibration/
|   +-- servo-clamp-calibration/
|   +-- motor-turn-calibration/
|   +-- ultrasonic-distance-test/
|   +-- ir-clamp-test/
|   +-- pixy-signature-check/
|   +-- base-return-calibration/
+-- hardware/
|   +-- schematic/
|   +-- kicad/
+-- assets/
```

## Final Code

The final competition sketch is located at:

```text
finalcode/FINALTESTING/FINALTESTING.ino
```

The folder name matches the `.ino` file name so it can be opened directly in the Arduino IDE.

## Calibration Note

The final sketch contains the values used during the competition run. The calibration sketches use `xxxx` placeholders where values should be re-measured for a rebuilt robot, different batteries, surface friction, motor variance, or camera mounting angle.
