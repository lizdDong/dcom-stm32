/**
  ******************************************************************************
  * @file    dio.h
  * @author  lizdDong (lizddong@126.com)
  * @version V1.0
  * @date    2021-4-12
  * @brief   None
  * @attention
  *
  ******************************************************************************
  */

#ifndef _DIO_H_
#define _DIO_H_

#include <stdint.h>


/* IO״̬���� */
enum io_stat_e
{
    IO_STAT_RESET = 0,
    IO_STAT_SET,
    IO_STAT_UNKNOWN
};


/* IO������� */
enum io_output_e
{
    IO_CTRL_MAIN_PRIO = 0,
    IO_CTRL_MANUAL_SWITCH
};

/* IO�����˲����� ��1ms/�Σ� */
#define IO_INPUT_FILTER_TIMES     10

/* IO���붨�� */
#define IO_INPUT_COUNTS           7
enum io_input_e
{
    IO_MAIN_WORKING = 0,    // ��·������
    IO_STDBY_WORKING,       // ��·������
    IO_STDBY_PRIO,          // �Զ�ģʽ�£���·����
    IO_MAIN_PRIO,           // �Զ�ģʽ�£���·����
    IO_SWITCH_ONCE,         // �л�һ��
    IO_MODE_AUTO,           // �Զ�ģʽ
    IO_MODE_MANUAL          // �ֶ�ģʽ
};

#define IO_IN_EFFECT   IO_STAT_SET

void io_init(void);
void io_inputFilterHandle(void);
void io_setOut(uint8_t n, uint8_t value);
uint8_t io_getIn(uint8_t n);

#endif


