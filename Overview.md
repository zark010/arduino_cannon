# Arduino Cannon Project
This repository showcases the code and build of an Aruduino cannon.

The project covers:
- Movement
- Distance detection
- Servo manipulation (angle adjustments based on distance)
- Shooting (via flywheels)

Due to the complexity of the ciruitry, two Arduinos were used. The master is in charge of movement and
distance detection via sensor. The slave (receiving the distance from the master) then calculates
the angle necessary for the shot, adjusts the necessary servos, and turns on the motors that spin
the flywheel.
