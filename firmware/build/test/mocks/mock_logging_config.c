/* AUTOGENERATED FILE. DO NOT EDIT. */
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
#include "unity.h"
#include "cmock.h"
#include "mock_logging_config.h"

static const char* CMockString_log_init = "log_init";

typedef struct _CMOCK_log_init_CALL_INSTANCE
{
  UNITY_LINE_TYPE LineNumber;
  int CallOrder;

} CMOCK_log_init_CALL_INSTANCE;

static struct mock_logging_configInstance
{
  int log_init_IgnoreBool;
  CMOCK_log_init_CALLBACK log_init_CallbackFunctionPointer;
  int log_init_CallbackCalls;
  CMOCK_MEM_INDEX_TYPE log_init_CallInstance;
} Mock;

extern jmp_buf AbortFrame;
extern int GlobalExpectCount;
extern int GlobalVerifyOrder;

void mock_logging_config_Verify(void)
{
  UNITY_LINE_TYPE cmock_line = TEST_LINE_NUM;
  if (Mock.log_init_IgnoreBool)
    Mock.log_init_CallInstance = CMOCK_GUTS_NONE;
  UNITY_SET_DETAIL(CMockString_log_init);
  UNITY_TEST_ASSERT(CMOCK_GUTS_NONE == Mock.log_init_CallInstance, cmock_line, CMockStringCalledLess);
  if (Mock.log_init_CallbackFunctionPointer != NULL)
    Mock.log_init_CallInstance = CMOCK_GUTS_NONE;
}

void mock_logging_config_Init(void)
{
  mock_logging_config_Destroy();
}

void mock_logging_config_Destroy(void)
{
  CMock_Guts_MemFreeAll();
  memset(&Mock, 0, sizeof(Mock));
  Mock.log_init_CallbackFunctionPointer = NULL;
  Mock.log_init_CallbackCalls = 0;
  GlobalExpectCount = 0;
  GlobalVerifyOrder = 0;
}

void log_init(void)
{
  UNITY_LINE_TYPE cmock_line = TEST_LINE_NUM;
  CMOCK_log_init_CALL_INSTANCE* cmock_call_instance;
  UNITY_SET_DETAIL(CMockString_log_init);
  cmock_call_instance = (CMOCK_log_init_CALL_INSTANCE*)CMock_Guts_GetAddressFor(Mock.log_init_CallInstance);
  Mock.log_init_CallInstance = CMock_Guts_MemNext(Mock.log_init_CallInstance);
  if (Mock.log_init_IgnoreBool)
  {
    UNITY_CLR_DETAILS();
    return;
  }
  if (Mock.log_init_CallbackFunctionPointer != NULL)
  {
    Mock.log_init_CallbackFunctionPointer(Mock.log_init_CallbackCalls++);
    return;
  }
  UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringCalledMore);
  cmock_line = cmock_call_instance->LineNumber;
  if (cmock_call_instance->CallOrder > ++GlobalVerifyOrder)
    UNITY_TEST_FAIL(cmock_line, CMockStringCalledEarly);
  if (cmock_call_instance->CallOrder < GlobalVerifyOrder)
    UNITY_TEST_FAIL(cmock_line, CMockStringCalledLate);
  UNITY_CLR_DETAILS();
}

void log_init_CMockIgnore(void)
{
  Mock.log_init_IgnoreBool = (int)1;
}

void log_init_CMockExpect(UNITY_LINE_TYPE cmock_line)
{
  CMOCK_MEM_INDEX_TYPE cmock_guts_index = CMock_Guts_MemNew(sizeof(CMOCK_log_init_CALL_INSTANCE));
  CMOCK_log_init_CALL_INSTANCE* cmock_call_instance = (CMOCK_log_init_CALL_INSTANCE*)CMock_Guts_GetAddressFor(cmock_guts_index);
  UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringOutOfMemory);
  memset(cmock_call_instance, 0, sizeof(*cmock_call_instance));
  Mock.log_init_CallInstance = CMock_Guts_MemChain(Mock.log_init_CallInstance, cmock_guts_index);
  Mock.log_init_IgnoreBool = (int)0;
  cmock_call_instance->LineNumber = cmock_line;
  cmock_call_instance->CallOrder = ++GlobalExpectCount;
  UNITY_CLR_DETAILS();
}

void log_init_StubWithCallback(CMOCK_log_init_CALLBACK Callback)
{
  Mock.log_init_IgnoreBool = (int)0;
  Mock.log_init_CallbackFunctionPointer = Callback;
}
