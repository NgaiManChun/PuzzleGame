#ifndef _INPUT_H
#define _INPUT_H

#include "keyboard.h"

#define INPUT_IS_DOWN(key) Keyboard_IsKeyDown(key)
#define INPUT_IS_DOWN_TRIGGER(key) Keyboard_IsKeyDownTrigger(key)
#define INPUT_IS_UP(key) Keyboard_IsKeyUp(key)

#define INPUT_START KK_A
#define INPUT_BACK KK_BACK

#define INPUT_A KK_A
#define INPUT_PLUS KK_W
#define INPUT_MINUS KK_Q
#define INPUT_ACC_0 KK_Z
#define INPUT_ACC_1 KK_X
#define INPUT_JUMP KK_SPACE
#define INPUT_OK KK_A

#define INPUT_0 KK_D0
#define INPUT_1 KK_D1
#define INPUT_2 KK_D2
#define INPUT_3 KK_D3
#define INPUT_4 KK_D4
#define INPUT_5 KK_D5
#define INPUT_6 KK_D6
#define INPUT_7 KK_D7
#define INPUT_8 KK_D8
#define INPUT_9 KK_D9

#define INPUT_ENTER KK_ENTER
#define INPUT_LEFT KK_LEFT
#define INPUT_UP KK_UP
#define INPUT_RIGHT KK_RIGHT
#define INPUT_DOWN KK_DOWN
#define INPUT_HOME KK_HOME
#define INPUT_SPACE KK_SPACE


#endif