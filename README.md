# 🧠MindMatrix Challenge🧠

This repository contains MindMatrix Challenge, a project for the 3rd-year "Introduction to Robotics" course taken at the Faculty of Mathematics and Computer Science, University of Bucharest.

## 🕹️Game Information and Rules 🕹️
The MindMatrix Challenge is guess what? ...a memory game where you can test your brain and attention. It consists of 3 levels with progressing difficulty. At each level, some LEDs are turned on to form a random pattern that will be displayed for a couple of seconds. Then, there's an LED blinking ... that's the player. It can be moved on the matrix using the joystick. When clicking the switch button, the LED at the player's position is turned on or off, depending on its previous state.  
🏆To win the game, you should represent the correct pattern at each level.  
❌If an LED that is not in the pattern is turned on, the game is over.

## 📜Menu Structure📜
To scroll through the menu, move the joystick. To select an option, click on the joystick.
#### 📄Main Menu📄
- Start game
- Settings
- About (info about the creator)
- How to play

#### ⚙️Settings Menu⚙️
- LCD Brightness Control
- Matrix Brightness Control
- Sound ON or OFF
- Back (to main menu)

## 🔋Electronic Setup🪫
The setup for this project involves the use of specific electronic components, including:
1. _An 8x8 LED Matrix_: An 8x8 LED matrix for Arduino is a compact display module featuring 64 individually controllable LEDs arranged in an 8 by 8 grid. Designed for use with Arduino microcontrollers, this matrix provides a versatile platform for creating dynamic visual patterns, scrolling text, simple animations, or custom pixel art.
2. _MAX7219 Driver_: The MAX7219 Driver for Arduino Matrix is a powerful display driver module that simplifies the control of LED matrix displays. Specifically designed for use with Arduino, this driver seamlessly interfaces with 8x8 LED matrices, providing an efficient means to control individual LEDs and create captivating visual effects.
3. _16x2 LCD_: A 16x2 LCD for Arduino is a standard character display module with 16 columns and 2 rows, capable of showing alphanumeric characters and symbols. This versatile module integrates seamlessly with Arduino microcontrollers, offering a user-friendly interface for displaying information such as sensor readings, messages, or system status.
4. _Joystick_: It is a handheld input device commonly used for controlling the movement or position of the object in a digital environment. It typically consists of a stick-like handle that can be pushed or tilted in various directions.
5. _10uF Electrolytic Capacitor_: It is a compact electronic component with a capacitance of 10 microfarads. This capacitor is of the electrolytic type, featuring a positive and a negative lead. Known for its ability to store and release electrical charge efficiently, the 10 μF electrolytic capacitor is commonly used in electronic circuits to filter and stabilize voltage, smooth power supplies, and assist in timing applications.
6. _104pF Ceramic Capacitor_: It is a small electronic component with a capacitance of 104 picofarads. This capacitor is of the ceramic type and is marked with the code "104" to indicate its capacitance value. It plays a crucial role in fine-tuning resonant frequencies and ensuring signal integrity in various electronic devices and applications.
7. _Resistors and wires_: These components are used to connect the matrix, LCD, joystick, and the driver to the Arduino. I have used a 100-Ohm resistor for the buzzer, a 10k-Ohm resistor for the MAX7219 driver, and a 220-Ohm resistor for the LCD.

## 📸Picture of the setup📸
