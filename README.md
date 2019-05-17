# LDR
Arduino sketch that approximates LDR lux equation.
You will need BH1750FVI sensor connected to Arduino via I2C, an LDR and a regular resistor. Connect the LDR/resistor to the A0 analog pin in a way in which the analogRead() gives values proportional to the LDR illumination (more light = higher value).
