/**
  ******************************************************************************
  * @file    dfsm.h
  * @author  lizdDong (lizddong@126.com)
  * @version V1.0
  * @date    2020-3-10
  * @brief   None
  * @attention
  *
  ******************************************************************************
  */

#include <stdint.h>

#ifndef _DFSM_H_
#define _DFSM_H_

#define FSM_OK         0
#define FSM_ERROR      (-1)

#ifndef FSM_STAT_NULL
#define FSM_STAT_NULL  (-1)
#endif

typedef struct
{
    int state;
    void (*fsmEnter)();
    void (*fsmLoop)();
    void (*fsmExit)();
} fsmState_t;

typedef struct
{
    int stateCount;
    int lastState;
    int currState;
    int nextState;
    fsmState_t *asFsmState;
} fsm_t;

int fsm_init(fsm_t *fsm, fsmState_t fsmState[], int stateNum);
fsm_t *fsm_create(int stateNum);
int fsm_free(fsm_t *sFsm);
int fsm_regist(fsm_t *sFsm, int state, void (*fsmEnter)(), void (*fsmLoop)(), void (*fsmExit)());
int fsm_start(fsm_t *sFsm, int state);
int fsm_transfer(fsm_t *sFsm, int state);
void fsm_handle(fsm_t *sFsm);


#endif

/*********************************** End of file *****************************************/
