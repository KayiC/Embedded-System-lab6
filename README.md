[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/p-H4XMpF)
# Ka Yi Cheng
# ECS643/ECS714
# Lab6: RTOS	Based	Systems

This lab project is working on controlling the flashing of green and red RGB LEDs base on the user input in the terminal.

- Start with green and red RGB LEDs are flashing
- Change the flashing time base on the user input
   - If the user input 'slower', then it will increase the delay time of flashing (0.5	s	→ 1s	→ 1.5s	→ …	→ 4s	→ 0.5	s)
   - If the user input 'faster', then it will decrease the delay time of flashing (4s	→ 3.5s	→ …	→ 0.5s	→ 4	s	→ 3.5	s	→ …)
   - If the user input invalid command, the error message is sent and the red RGB LED is on
