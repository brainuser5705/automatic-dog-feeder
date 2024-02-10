/*
  This file contains configuration variables used to adjust the functionality of the door.
*/

#ifndef CONFIGURATIONS_H
#define CONFIGURATIONS_H

const int MIN_POS = 0;    // final position when door is open
const int MAX_POS = 145;  // final position when door is close
const int SPEED = 15;     // how fast the door opens (smaller -> faster)
const int DELAY = 3;      // time to wait between opening and closing (secs)

const int ALARM_TIMES_SIZE = 2;
const int ALARM_TIMES[][3] = {{7, 00, 00}, {19,00,00}}; // must enter second dimension

#endif