

![LED_problem_with_resistor_value](LED_problem_with_resistor_value.png)

Standard LEDs only need 10–20mA, and most low-power devices cannot handle 200mA.

The LED toggle system fails with a 10-ohm (
) resistor because it allows too much current to flow, likely causing the power supply to trigger protection mechanisms (like thermal shutdown or voltage drop),