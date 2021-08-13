/**
  ******************************************************************************
  * @file    dfsm.c
  * @author  lizdDong (lizddong@126.com)
  * @version V1.0
  * @date    2020-3-10
  * @brief   None
  * @attention
  *
  ******************************************************************************
**/

#include "dcom.h"
#include "dfsm.h"


/**
 ****************************************************************************
 * @brief    静态的创建一个状态机
 * @author lizdDong
 * @note   None
 * @param  stateNum: 状态数量，必须大于0
 * @retval  成功(FSM_OK)，失败(FSM_ERROR)
 ****************************************************************************
*/
int fsm_init(fsm_t *fsm, fsmState_t fsmState[], int stateNum)
{
    int i;

    if(stateNum < 1)
    {
        return FSM_ERROR;
    }

    //初始化状态机
    fsm->stateCount = stateNum;
    fsm->lastState = FSM_STAT_NULL;
    fsm->currState = FSM_STAT_NULL;
    fsm->nextState = FSM_STAT_NULL;
    fsm->asFsmState = fsmState;

    //初始化每一个状态结构
    for(i = 0; i < stateNum; ++i)
    {
        fsm->asFsmState[i].state = FSM_STAT_NULL;
        fsm->asFsmState[i].fsmEnter = NULL;
        fsm->asFsmState[i].fsmLoop = NULL;
        fsm->asFsmState[i].fsmExit = NULL;
    }
    return FSM_OK;
}

/**
 ****************************************************************************
 * @brief    动态的创建一个状态机
 * @author lizdDong
 * @note   None
 * @param  stateNum: 状态数量，必须大于0
 * @retval  成功(状态机指针)，失败(NULL)
 ****************************************************************************
*/
fsm_t *fsm_create(int stateNum)
{
    fsm_t *fsm = NULL;
    fsmState_t *fsmState;
    int i;

    if(stateNum < 1)
    {
        return NULL;
    }

    // 申请状态机内存
    fsm = (fsm_t *)malloc(sizeof(fsm_t));
    if(fsm == NULL)
        return NULL;

    fsmState = (fsmState_t *)malloc(sizeof(fsm_t) * stateNum);
    if(fsmState == NULL)
        return NULL;

    //初始化状态机
    fsm->stateCount = stateNum;
    fsm->lastState = FSM_STAT_NULL;
    fsm->currState = FSM_STAT_NULL;
    fsm->nextState = FSM_STAT_NULL;
    fsm->asFsmState = fsmState;

    //初始化每一个状态结构
    for(i = 0; i < stateNum; ++i)
    {
        fsm->asFsmState[i].state = FSM_STAT_NULL;
        fsm->asFsmState[i].fsmEnter = NULL;
        fsm->asFsmState[i].fsmLoop = NULL;
        fsm->asFsmState[i].fsmExit = NULL;
    }
    return fsm;
}

/**
 ****************************************************************************
 * @brief    释放状态机
 * @author lizdDong
 * @note   None
 * @param  sFsm: 状态机指针
 * @retval  成功（FSM_OK），失败（FSM_ERROR）
 ****************************************************************************
*/
int fsm_free(fsm_t *sFsm)
{
    if(sFsm == NULL)
    {
        return FSM_ERROR;
    }

    //释放状态机内存
    free(sFsm->asFsmState);
    free(sFsm);
    return FSM_OK;
}

/**
 ****************************************************************************
 * @brief    注册状态
 * @author lizdDong
 * @note   None
 * @param  sFsm: 状态机指针, state: 要注册的状态, fsmEnter: 入状态回调函数, fsmLoop: 状态中回调函数, fsmExit: 出状态回调函数
 * @retval  成功（FSM_OK），失败（FSM_ERROR）
 ****************************************************************************
*/
int fsm_regist(fsm_t *sFsm, int state, void (*fsmEnter)(), void (*fsmLoop)(), void (*fsmExit)())
{
    int i;
    for(i = 0; i < sFsm->stateCount; ++i)
    {
        if(sFsm->asFsmState[i].state == FSM_STAT_NULL)
        {
            break;
        }
    }
    if(i == sFsm->stateCount)
    {
        return FSM_ERROR;
    }
    sFsm->asFsmState[i].state = state;
    sFsm->asFsmState[i].fsmEnter = fsmEnter;
    sFsm->asFsmState[i].fsmLoop = fsmLoop;
    sFsm->asFsmState[i].fsmExit = fsmExit;
    return FSM_OK;
}

/**
 ****************************************************************************
 * @brief    启动状态机
 * @author lizdDong
 * @note   None
 * @param  sFsm: 状态机指针, state: 启动时的状态
 * @retval  成功（FSM_OK），失败（FSM_ERROR）
 ****************************************************************************
*/
int fsm_start(fsm_t *sFsm, int state)
{
    if(sFsm == NULL)
    {
        return FSM_ERROR;
    }
    sFsm->currState = state;
    sFsm->nextState = state;
    return FSM_OK;
}

/**
 ****************************************************************************
 * @brief    状态迁移
 * @author lizdDong
 * @note   None
 * @param  sFsm: 状态机指针, state: 要迁移到的状态
 * @retval  成功（FSM_OK），失败（FSM_ERROR）
 ****************************************************************************
*/
int fsm_transfer(fsm_t *sFsm, int state)
{
    if(sFsm == NULL)
    {
        return FSM_ERROR;
    }
    if(sFsm->currState == FSM_STAT_NULL)
    {
        sFsm->currState = state;
    }
    sFsm->nextState = state;
    return FSM_OK;
}

/**
 ****************************************************************************
 * @brief  状态机处理程序，需要被循环调用
 * @note   None
 * @param  sFsm: 状态机指针
 * @retval None
 ****************************************************************************
*/
void fsm_handle(fsm_t *sFsm)
{
    int i;

    if(sFsm == NULL)
    {
        return;
    }
    for(i = 0; i < sFsm->stateCount; ++i)
    {
        if(sFsm->asFsmState[i].state != FSM_STAT_NULL && sFsm->asFsmState[i].state == sFsm->currState)
        {
            //执行入状态回调函数
            if(sFsm->lastState != sFsm->currState)
            {
                sFsm->lastState = sFsm->currState;
                if(sFsm->asFsmState[i].fsmEnter != NULL)
                {
                    sFsm->asFsmState[i].fsmEnter();
                }
            }

            //执行状态中回调函数
            if(sFsm->asFsmState[i].fsmLoop != NULL)
            {
                sFsm->asFsmState[i].fsmLoop();
            }

            //执行出状态回调函数
            if(sFsm->nextState != sFsm->currState)
            {
                sFsm->currState = sFsm->nextState;
                sFsm->nextState = sFsm->currState;
                if(sFsm->asFsmState[i].fsmExit != NULL)
                {
                    sFsm->asFsmState[i].fsmExit();
                }
            }
        }
    }
}


/*********************************** End of file *****************************************/


