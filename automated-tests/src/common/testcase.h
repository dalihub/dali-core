#ifndef _TESTCASE_H_
#define _TESTCASE_H_

/* pointer to startup/cleanup functions */
typedef void (*void_fun_ptr)(void);

/* pointer to testcase functions */
typedef int (*tc_fun_ptr)(void);

/* struct describing specific testcase */
typedef struct testcase_s {
    const char* name;
    tc_fun_ptr function;
    void_fun_ptr startup;
    void_fun_ptr cleanup;
} testcase;

#endif // _TESTCASE_H_
