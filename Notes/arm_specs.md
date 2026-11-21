## Arm
- Type: acrylic kit arm, 4 servos (beginner build)
- Link 1 (upper arm): 92 mm
- Link 2 (forearm): 85 mm
- Servos: 3× MG90S + 1× MG996R
- Driver: PCA9685 (I2C, A4/A5), powered by XL4016 + 1000µF capacitor

**Use the HC-SR04 sensor's face as your reference point, and the arm's base rotation axis (the centre of the base servo shaft) as the arm's origin.**

dx - Front–back distance from the base axis to the sensor face - 
dy - Left–right distance (should be 0 if both are centred) -
dz - Height of the shoulder joint above the floor -

## Calibration
# Channels

| Ch | Joint |
|---|---|
| 0 | Gripper |
| 1 | Link 1 (shoulder) |
| 2 | Link 2 (elbow) |
| 3 | Base |

## Joint limits
| Joint | Ch | Min | Max | Direction |
|---|---|---|---|---|
| Gripper | 0 | 30 (close) | 80 (open) | |
| Link 1 | 1 | 40 | 160 | min = down, max = up |
| Link 2 | 2 | 50 | 150 | min = up, max = down |
| Base | 3 | 0 | 180 | min = right, max = left |

## Coupling (Link 1 ↔ Link 2)
Measured conditions:
- Link 1 at min (40) → Link 2: 100–150
- Link 1 at max (160) → Link 2: 50–100
- Link 2 at min (50) → Link 1: 100–160
- Link 2 at max (150) → Link 1: 40–120

## Method
I used `firmware/4dof_arm_part/calibration/calibration.ino` for check min max of each links.

1. Calibration sketch: type "ch angle" in Serial Monitor (9600, Newline)
2. Moved each servo in 10° steps until it touched or buzzed