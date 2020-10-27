Purpose
In Lab09 you will learn about time delays, arrays and functional debugging. The data you collect will be physical proof that the system operates within specifications.

System Requirements
The Lab9 starter project is the same as C9_Debugging example but includes the connections to the Lab9 grader. You will make three changes.

First, make the LED flash at 10 Hz. In other words, invert the output every 0.05 seconds.

Second, make the LED flash if either switch SW1 or SW2 are pressed (this means either PF4 or PF0 is 0).  This means toggle if just SW1 is pressed, just SW2 is pressed or both switches are pressed.

Third, record PortF bits 4,1,0 every time the input changes or the output changes. For example, if your system detects a change in either PF4 or PF0 input, record PortF bits 4,1,0. If your system causes a change in PF1, record PortF bits 4,1,0. In order for the grading engine to see/grade your data, please leave the debugging array defined exactly as it is in the starter project.

Remember to reset the program before grading so the grader can watch you enter data into the buffers.

unsigned long Data[50];

Your system will be graded on its ability to satisfy the following requirements.

If both PF4 and PF0 switch are not pressed, the PF1 output should be low.
If either PF4 or PF0 switches is pressed, the output is inverted every 50 ms, making it oscillate at 10 Hz (±10%).
Information collected in the Data array matches the I/O on PortF.
50 data points are collected only on a change in input or a change in output. (i.e., no adjacent elements in the array are equal).

Most of the labs run at 80 MHz. This lab is an exception to this rule, where the grader does not activate the PLL, so the system runs with the internal oscillator at 16 MHz. You must not modify this rate.