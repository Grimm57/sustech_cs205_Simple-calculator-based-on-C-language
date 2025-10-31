/*
    *编译指令 gcc -o calculator main.c
    *Last update: 2025/03/15
*/



#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

/// 宏

#define STACK_MAX 256             // 表达式栈的最大容量
#define MAX_VARIABLES 50          // 变量表的最大容量
#define MAX_NAME_LEN 50           // 变量名的最大长度
#define MAX_FUNCTIONS 20          // 函数表的最大容量
#define MAX_FUNCTION_PARAM_LEN 50 // 函数参数的最大数量
#ifndef DEFAULT_SCALE
#define DEFAULT_SCALE 20          // 默认除法小数位数
#endif
int scale = DEFAULT_SCALE; // 使用编译时定义的默认值

// 枚举
// 节点类型
typedef enum
{
    OPERATOR,
    OPERAND,
    VARIABLE,
    // FUNCTION命名要求与变量一致，但是必须包含“{”和“}”
} NodeType;

/// 结构体
typedef struct
{
    void *data[STACK_MAX];
    int top;
} Stack;

// 节点结构
typedef struct ExprNode
{
    char *value;
    NodeType type;
    struct ExprNode *left;
    struct ExprNode *right;
} ExprNode;
// 变量结构
typedef struct
{
    char name[MAX_NAME_LEN];
    char *value;
} Variable;
// 函数结构
typedef struct
{
    char name[MAX_NAME_LEN];
    char *params[MAX_FUNCTION_PARAM_LEN]; // 函数参数名称数组
    int param_count;                      // 参数数量
    ExprNode *body;                       // 函数表达式树
} Function;

// 存储被临时覆盖的变量
typedef struct
{
    char name[MAX_NAME_LEN];
    char *value;
} TempVariable;

////////////////////////////////////////////////////////////////定义数据结构////////////////////////////////////////////////////////////////

//////////////////////////////////表达式树相关//////////////////////////////////

// 节点相关方法
ExprNode *createExprNode(const char *value, NodeType type);
void freeExprTree(ExprNode *root);

////////////////////表达式树辅助函数////////////////////

int getPriority(char op);

// 检查传入是否合法
int isValidIdentifier(const char *str);
int isValidNumber(const char *str);
int isOperator(char c);
int isValidMethodCall(const char *str);
int isValidMethodName(const char *str);
int isValidMethodDeclaration(const char *str);
int isValidVariableName(const char *str);
int isValidVariableDeclaration(const char *str);

// 将中缀表达式转化为表达式树
ExprNode *tokenize(const char *input);

/*
//测试用方法
char* expressionToString(ExprNode *root);
void inorderTraversal(ExprNode* root, char* result, int* position, int maxLength);
*/

//////////////////////////////////栈相关//////////////////////////////////
Stack *createStack();
void push(Stack *stack, void *data);
void *pop(Stack *stack);
void *peek(Stack *stack);
int isStackEmpty(Stack *stack);
void freeStack(Stack *stack);

////////////////////////////////////////////////////////////////基本计算函数////////////////////////////////////////////////////////////////
/////基础加减乘除/////
////////////////////////////////////////////////////////////////基本计算函数////////////////////////////////////////////////////////////////
/////基础加减乘除/////


// 辅助函数
int compare_numbers_abs(const char *num1, const char *num2);

// 正整数加减乘
char *add_big_abs(const char *num1, const char *num2);
char *subtract_big_abs(const char *num1, const char *num2);
char *multiply_big_abs(const char *num1, const char *num2);
/*
// Karatsuba算法实现 未完全测试,所以弃用
char *mul_kara(const char *num1, const char *num2);
char *shift_left(const char *num, int n);
*/
// 正小数加减乘除函数
char *add_big_decimal(const char *num1, const char *num2);
char *subtract_big_decimal(const char *num1, const char *num2);
char *multiply_big_decimal(const char *num1, const char *num2);
char *divide_big_abs(const char *num1, const char *num2, int scale);
// 自然数加减乘除
char *complete_add(const char *num1, const char *num2);
char *complete_sub(const char *num1, const char *num2);
char *complete_mul(const char *num1, const char *num2);
char *complete_div(const char *num1, const char *num2, int scale);


////////////////////////////////////////////////////////////////函数与变量的定义////////////////////////////////////////////////////////////////

////////变量相关////////

// 全局变量表
Variable variables[MAX_VARIABLES];
int varCount = 0;

char *findVariable(const char *name);
int setVariable(const char *name, const char *value);
void freeVariables();

////////函数相关////////

// 函数表
Function functions[MAX_FUNCTIONS];
int funcCount = 0;

// 查找函数
Function *findFunction(const char *name);

// 函数声明的解析和保存
int parseAndSaveFunction(const char *declaration);

// 清理函数表的辅助函数
void freeFunctions();

// 显示当前变量和方程
void displayState();

///////////////////////////////////// 计算器核心////////////////////////////////////////
// 计算表达式树
char *calculate_core(ExprNode *root);
// 执行函数调用
char *executeFunction(const char *methodCall);
// 打印帮助信息
void printHelp();
// 打印版本信息
void printVersion();
// 交互模式主函数
int interactiveMode(int scale);
// 命令行模式主函数
int commandLineMode(const char *num1, const char *operator, const char * num2, int scale);

////////////////////////////////////////////////////////////////主函数////////////////////////////////////////////////////////////////
/*
测试
int main()
{
    char *num1 = "113212312";
    char *num2 = "712897389";
    int scale = 30;
    char *result = divide_big_abs(num1, num2, scale);
    printf("result: %s\n", result);
    return 0;
}
*/
int main(int argc, char **argv)
{
    int optionCount = 0;
    // 命令行参数解析
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
        {
            optionCount++;
            printHelp();
        }
        else if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0)
        {
            optionCount++;
            printVersion();
        }
        else if (strncmp(argv[i], "-s", 2) == 0 || strncmp(argv[i], "--scale", 7) == 0)
        {
            optionCount++;
            int tempScale = atoi(argv[i] + 2);
            if (tempScale >= 0 && tempScale <= 50)
            {
            scale = tempScale;
            }
            else
            {
            printf("TokenizeError: Invalid scale value. Scale must be between 0 and 50.\n");
            return EXIT_FAILURE;
            }
        }
    }
    // 分别对应help,version,quiet命令
    switch (argc - optionCount)
    {
    case 1:
        interactiveMode(scale);
        break;
    case 4:
        commandLineMode(argv[1], argv[2], argv[3], scale);
        break;
    default:
        printf("TokenizeError: Can't resolve the input symbol, use -h or --help for help\n");
        break;
    }
    freeVariables();
    freeFunctions();
    return EXIT_SUCCESS;
}
////////////////////////////////////////////////////////////////函数实现////////////////////////////////////////////////////////////////
//内存错误报错1
void memory_error()
{
    printf("MemoryError: Memory allocation failed\n");
}

// 正整数比较
int compare_numbers_abs(const char *num1, const char *num2)
{
    // 跳过符号
    if (num1[0] == '+' || num1[0] == '-')
        num1++;
    if (num2[0] == '+' || num2[0] == '-')
        num2++;

    size_t len1 = strlen(num1);
    size_t len2 = strlen(num2);

    if (len1 != len2)
    {
        return (len1 > len2) ? 1 : -1;
    }

    // 长度相同时逐位比较
    for (size_t i = 0; i < len1; i++)
    {
        if (num1[i] != num2[i])
        {
            return (num1[i] > num2[i]) ? 1 : -1;
        }
    }

    return 0; // 相等
}
// 正实数比较
int compare_decimal_numbers_abs(const char *num1, const char *num2)
{
    // 跳过符号
    if (num1[0] == '+' || num1[0] == '-')
        num1++;
    if (num2[0] == '+' || num2[0] == '-')
        num2++;

    // 找到小数点位置
    const char *dot1 = strchr(num1, '.');
    const char *dot2 = strchr(num2, '.');

    // 计算整数部分长度
    size_t int_len1 = dot1 ? (dot1 - num1) : strlen(num1);
    size_t int_len2 = dot2 ? (dot2 - num2) : strlen(num2);

    // 先比较整数部分长度
    if (int_len1 != int_len2)
    {
        return (int_len1 > int_len2) ? 1 : -1;
    }

    // 比较整数部分
    for (size_t i = 0; i < int_len1; i++)
    {
        if (num1[i] != num2[i])
        {
            return (num1[i] > num2[i]) ? 1 : -1;
        }
    }

    // 如果整数部分相等，比较小数部分
    const char *dec1 = dot1 ? (dot1 + 1) : "";
    const char *dec2 = dot2 ? (dot2 + 1) : "";

    // 逐位比较小数部分
    while (*dec1 || *dec2)
    {
        char d1 = *dec1 ? *dec1 : '0';
        char d2 = *dec2 ? *dec2 : '0';

        if (d1 != d2)
        {
            return (d1 > d2) ? 1 : -1;
        }

        if (*dec1)
            dec1++;
        if (*dec2)
            dec2++;
    }

    return 0; // 完全相等
}

// 加法的实现

// 前置方法,计算加法的raw_result
char *add_big_abs(const char *num1, const char *num2)
{
    int len1 = strlen(num1);
    int len2 = strlen(num2);
    // 两个正整数相加,结果最多比较长的那个数多一位
    int maxLen = (len1 > len2 ? len1 : len2) + 1;

    char *result = (char *)malloc((maxLen + 1) * sizeof(char));
    if (!result)
    {
        memory_error();
        return NULL;
    }
    memset(result, '0', maxLen);
    result[maxLen] = '\0';

    int carry = 0, i = len1 - 1, j = len2 - 1, k = maxLen - 1;

    // 倒序模拟竖式加法运算
    while (i >= 0 || j >= 0 || carry)
    {
        int digit1 = (i >= 0) ? num1[i] - '0' : 0;
        int digit2 = (j >= 0) ? num2[j] - '0' : 0;
        int sum = digit1 + digit2 + carry;
        // k不可能变成负数
        result[k] = (sum % 10) + '0';
        carry = sum / 10;

        // 处理完这一位后，指针前移
        i--;
        j--;
        k--;
    }
    /*
        // 消除前导零
        char *start = result; // start与result指向同一块内存
        while (*start == '0' && *(start + 1) != '\0')
        {
            start++;
        }

        // 如果 start 指向的位置比 result 靠后，说明结果字符串中存在前导零。
        if (start > result)
        {
            size_t newLen = strlen(start);      // 计算从start一直到字符串末尾的长度,但是不包含结束符!
            memmove(result, start, newLen + 1); // +1 为了包含结束符
        }
    */

    return result;
}
// 正实数相加
char *add_big_decimal(const char *num1, const char *num2)
{
    // 找到小数点位置
    const char *dot1 = strchr(num1, '.');
    const char *dot2 = strchr(num2, '.');
    // 如果均为整数就直接调用add_big_abs
    // 计算整数部分和小数部分的长度

    // 指针相减表示指针指向的位置之间有多少个元素
    // 不同类型的指针不能直接相减
    size_t int_len1 = dot1 ? (dot1 - num1) : strlen(num1);
    size_t int_len2 = dot2 ? (dot2 - num2) : strlen(num2);

    size_t dec_len1 = dot1 ? strlen(dot1 + 1) : 0;
    size_t dec_len2 = dot2 ? strlen(dot2 + 1) : 0;

    // 找出最长的小数位数, 用于对齐
    // 0.01 + 0.001
    // max_dec_len = 3
    size_t max_dec_len = (dec_len1 > dec_len2) ? dec_len1 : dec_len2;
    size_t max_int_len = (int_len1 > int_len2 ? int_len1 : int_len2) + 1;
    // 创建临时字符串来存储对齐后的数字
    // temp_size = 1 + 1 + 3 + 2 = 7
    // 0010
    // 0001
    size_t temp_size = max_int_len + max_dec_len + 2;
    // 对齐后的数字长度为整数部分最大长度+1+最大小数部分长度+小数点+结束符
    // +2 为小数点和\0

    // 0000000 七个\0
    char *temp1 = (char *)calloc(temp_size, sizeof(char));
    char *temp2 = (char *)calloc(temp_size, sizeof(char));

    if (!temp1 || !temp2)
    {
        memory_error();
        return NULL;
    }

    // '0'000000 字符0加上6个/0
    // '0'000000 字符0加上6个/0
    // 复制整数部分
    strncpy(temp1, num1, int_len1);
    strncpy(temp2, num2, int_len2);

    // 复制小数部分(如果有)

    //'001'0000
    if (dot1)
    {
        strcat(temp1, dot1 + 1);
        // 从目标字符串的第一个\0开始向后拼接源字符串
    }
    //'0001'000 字符0加上6个/0
    if (dot2)
    {
        strcat(temp2, dot2 + 1);
    }

    // 补零对齐小数部分
    // 在较短的小数后边补零,使得加法可以对齐
    // '001'0000
    // strlen返回到第一个结束符的长度
    // 3 - 1 < 3
    // '0010'000
    while (strlen(temp1) - int_len1 < max_dec_len)
    {
        strcat(temp1, "0");
    }
    // '0001'000
    while (strlen(temp2) - int_len2 < max_dec_len)
    {
        strcat(temp2, "0");
    }

    // 使用原有的add_big_abs进行计算
    char *raw_result = add_big_abs(temp1, temp2);
    // printf("raw_result: %s\n", raw_result);
    if (!raw_result)
    {
        memory_error();
        free(temp1);
        free(temp2);
        return NULL;
    }
    // 在结果中插入小数点
    size_t result_len = strlen(raw_result);              // 2
    char *final_result = (char *)malloc(result_len + 2); // 7
    // 结果的最大长度就是之前计算的temp_size
    if (!final_result)
    {
        memory_error();
        free(temp1);
        free(temp2);
        free(raw_result);
        return NULL;
    }
    // 计算结果的小数点位置
    // 原结果长度 - 最大小数位数
    // 4 - 3 = 1
    size_t dot_pos = result_len - max_dec_len;
    strncpy(final_result, raw_result, dot_pos); // 把源字符串中到小数点之前的字符复制到final_result中
    if (max_dec_len > 0)
    {
        final_result[dot_pos] = '.'; // 在小数点的位置插入小数点
        strcpy(final_result + dot_pos + 1, raw_result + dot_pos);
    }
    else
    {
        strcpy(final_result + dot_pos, raw_result + dot_pos);
    }
    // final_result + dot_pos + 1指向最终结果字符串中小数点之后的第一个位置
    // raw_result + dot_pos指向原始结果字符串中小数部分开始的位置
    final_result[result_len + 1] = '\0';
    // 检查是否有多余的前导零（首位是0且下一位不是小数点）
    if (final_result[0] == '0' && final_result[1] != '.' && final_result[1] != '\0')
    {
        memmove(final_result, final_result + 1, result_len + 1);
    }
    // 清理
    free(temp1);
    free(temp2);
    free(raw_result);
    return final_result;
}
// 所有实数相加
char *complete_add(const char *num1, const char *num2)
{
    // 检查输入是否带符号
    int is_negative1 = (num1[0] == '-');
    int is_signed1 = (num1[0] == '+' || is_negative1);
    int is_negative2 = (num2[0] == '-');
    int is_signed2 = (num2[0] == '+' || is_negative2);

    // 去掉符号
    if (is_signed1)
        num1++;
    if (is_signed2)
        num2++;

    char *result = NULL;

    // 如果两个数都是负数，结果也是负数
    if (is_negative1 && is_negative2)
    {
        result = add_big_decimal(num1, num2);
        if (!result)
        {
            return NULL;
        }
        size_t len = strlen(result);
        char *final_result = (char *)malloc(len + 2); // +2 为负号和\0
        if (!final_result)
        {
            free(result);
            return NULL;
        }
        final_result[0] = '-';
        strcpy(final_result + 1, result);
        free(result);
        return final_result;
    }

    // 如果一个数是负数，另一个数是正数，进行减法运算
    if (is_negative1 && !is_negative2)
    {
        result = subtract_big_decimal(num2, num1); // num2 - num1
        return result;
    }
    if (!is_negative1 && is_negative2)
    {
        result = subtract_big_decimal(num1, num2); // num1 - num2
        return result;
    }

    // 如果两个数都是正数，直接调用add_big_decimal
    result = add_big_decimal(num1, num2);
    return result;
}

// 减法的实现

// 前置方法,计算减法的raw_result
char *subtract_big_abs(const char *num1, const char *num2)
{
    // 比较两数大小
    int comparison = compare_numbers_abs(num1, num2);
    const char *larger = num1;
    const char *smaller = num2;
    // printf("larger: %s\n", larger);
    // printf("smaller: %s\n", smaller);
    // int is_negative = 0;
    /*
    //保证结果为正数
    if (comparison < 0)
    {
        // 如果 num1 < num2，交换操作数并标记结果为负
        larger = num2;
        smaller = num1;
        //is_negative = 1;
    }
    */

    int len1 = strlen(larger);
    int len2 = strlen(smaller);

    // 最大长度为被减数的长度 (+1为\0)
    char *result = (char *)malloc((len1 + 1));
    if (!result)
    {
        memory_error();
        return NULL;
    }

    // 初始化结果数组
    memset(result, '0', len1);
    result[len1] = '\0';

    // 从右向左模拟竖式做减法
    int borrow = 0;
    int i = len1 - 1, j = len2 - 1, k = len1 - 1;

    while (i >= 0)
    {
        // 当前位数字
        int digit1 = larger[i] - '0';
        int digit2 = (j >= 0) ? (smaller[j] - '0') : 0;

        // 处理来自上一位的借位
        digit1 -= borrow;

        // 如果当前位不够减
        if (digit1 < digit2)
        {
            digit1 += 10;
            borrow = 1;
        }
        else
        {
            borrow = 0;
        }

        // 计算当前位的结果
        result[k] = (digit1 - digit2) + '0';

        i--;
        j--;
        k--;
    }
    /*
    // 消除前导零
    char *start = result; // start与result指向同一块内存
    while (*start == '0' && *(start + 1) != '\0')
    {
        start++;
    }

    // 如果 start 指向的位置比 result 靠后，说明结果字符串中存在前导零。
    if (start > result)
    {
        size_t newLen = strlen(start);      // 计算从start一直到字符串末尾的长度,但是不包含结束符!
        memmove(result, start, newLen + 1); // +1 为了包含结束符
    }
    // 检查是否为负数并且结果不是0
    if (is_negative && strcmp(result, "0") != 0)
    {
        size_t len = strlen(result);
        char *final_result = (char *)malloc(len + 2); // +2 为负号和\0
        if (!final_result)
        {
            memory_error();
            free(result);
            return NULL;
        }
        final_result[0] = '-';
        strcpy(final_result + 1, result);
        free(result);
        return final_result;
    }
    */
    return result;
}
// 正实数相减
char *subtract_big_decimal(const char *num1, const char *num2)
{
    // 0.01
    // 0.001
    //  找到小数点位置
    const char *dot1 = strchr(num1, '.');
    const char *dot2 = strchr(num2, '.');
    // 计算整数部分和小数部分的长度
    // 指针相减表示指针指向的位置之间有多少个元素
    // 不同类型的指针不能直接相减
    size_t int_len1 = dot1 ? (dot1 - num1) : strlen(num1);
    size_t int_len2 = dot2 ? (dot2 - num2) : strlen(num2);

    size_t dec_len1 = dot1 ? strlen(dot1 + 1) : 0;
    size_t dec_len2 = dot2 ? strlen(dot2 + 1) : 0;

    // 找出最长的小数位数, 用于对齐
    // 0.01 - 0.001 = 0.009
    // max_dec_len = 3
    size_t max_dec_len = (dec_len1 > dec_len2) ? dec_len1 : dec_len2;
    size_t max_int_len = (int_len1 > int_len2 ? int_len1 : int_len2) + 2;
    // 创建临时字符串来存储对齐后的数字
    // temp_size = 1 + 2 + 3 + 2 = 8
    // 001
    // 0001
    size_t temp_size = max_int_len + max_dec_len + 2;
    // 对齐后的数字长度为整数部分最大长度+1+最大小数部分长度+小数点+结束符
    // +2 为小数点和\0

    // 0000000 七个\0
    char *temp1 = (char *)calloc(temp_size, sizeof(char));
    char *temp2 = (char *)calloc(temp_size, sizeof(char));

    if (!temp1 || !temp2)
    {
        memory_error();
        return NULL;
    }

    // '0'000000 字符0加上6个/0
    // '0'000000 字符0加上6个/0
    // 复制整数部分
    strncpy(temp1, num1, int_len1);
    strncpy(temp2, num2, int_len2);

    // 复制小数部分(如果有)

    //'001'0000
    if (dot1)
    {
        strcat(temp1, dot1 + 1);
        // 从目标字符串的第一个\0开始向后拼接源字符串
    }
    //'0001'000 字符0加上6个/0
    if (dot2)
    {
        strcat(temp2, dot2 + 1);
    }

    // 补零对齐小数部分
    // 在较短的小数后边补零,使得加法可以对齐
    // '001'0000
    // strlen返回到第一个结束符的长度
    // 3 - 1 < 3
    // '0010'000
    while (strlen(temp1) - int_len1 < max_dec_len)
    {
        strcat(temp1, "0");
    }
    // '0001'000
    while (strlen(temp2) - int_len2 < max_dec_len)
    {
        strcat(temp2, "0");
    }
    int comparison = compare_numbers_abs(temp1, temp2);
    const char *larger = temp1;
    const char *smaller = temp2;
    // printf("larger: %s\n", larger);
    // printf("smaller: %s\n", smaller);
    int is_negative = 0;
    // 保证结果为正数
    if (comparison < 0)
    {
        // 如果 num1 < num2，交换操作数并标记结果为负
        larger = temp2;
        smaller = temp1;
        is_negative = 1;
    }
    // 使用subtract_big_abs进行计算
    char *raw_result = subtract_big_abs(larger, smaller);
    // printf("raw_result: %s\n", raw_result);
    if (!raw_result)
    {
        memory_error();
        free(temp1);
        free(temp2);
        return NULL;
    }
    // 在结果中插入小数点
    size_t result_len = strlen(raw_result);              // 2
    char *final_result = (char *)malloc(result_len + 3); // 7
    // 结果的最大长度就是之前计算的temp_size
    if (!final_result)
    {
        memory_error();
        free(temp1);
        free(temp2);
        // free(larger);
        // free(smaller);
        free(raw_result);
        return NULL;
    }
    // 计算结果的小数点位置
    // 原结果长度 - 最大小数位数
    // 4 - 3 = 1
    size_t dot_pos = result_len - max_dec_len;
    strncpy(final_result, raw_result, dot_pos); // 把源字符串中到小数点之前的字符复制到final_result中
    if (max_dec_len > 0)
    {
        final_result[dot_pos] = '.'; // 在小数点的位置插入小数点
        strcpy(final_result + dot_pos + 1, raw_result + dot_pos);
    }
    else
    {
        strcpy(final_result + dot_pos, raw_result + dot_pos);
    }
    // final_result + dot_pos + 1指向最终结果字符串中小数点之后的第一个位置
    // raw_result + dot_pos指向原始结果字符串中小数部分开始的位置
    final_result[result_len + 1] = '\0';
    // 检查是否有多余的前导零（首位是0且下一位不是小数点）
    if (final_result[0] == '0' && final_result[1] != '.' && final_result[1] != '\0')
    {
        memmove(final_result, final_result + 1, result_len + 1);
    }
    /*
     */
    // 如果第一位是小数点，前面加0
    if (final_result[0] == '.')
    {
        memmove(final_result + 1, final_result, strlen(final_result) + 1);
        final_result[0] = '0';
    }
    // 如果结果为负数，添加负号
    if (is_negative)
    {
        memmove(final_result + 1, final_result, strlen(final_result) + 1);
        final_result[0] = '-';
    }

    // 清理
    free(temp1);
    free(temp2);
    free(raw_result);
    return final_result;
}
// 所有实数相减
char *complete_sub(const char *num1, const char *num2)
{
    // 检查输入是否带符号
    int is_negative1 = (num1[0] == '-');
    int is_signed1 = (num1[0] == '+' || is_negative1);
    int is_negative2 = (num2[0] == '-');
    int is_signed2 = (num2[0] == '+' || is_negative2);

    // 去掉符号
    if (is_signed1)
        num1++;
    if (is_signed2)
        num2++;

    char *result = NULL;

    // 如果两个数都是负数，2 - 1
    if (is_negative1 && is_negative2)
    {
        result = subtract_big_decimal(num2, num1);
        if (!result)
        {
            return NULL;
        }
        return result;
    }

    // 如果一个数是负数，另一个数是正数，进行加法运算
    if (is_negative1 && !is_negative2)
    {
        result = add_big_decimal(num1, num2); // -num1 - num2
        if (result)
        {
            size_t len = strlen(result);
            char *final_result = (char *)malloc(len + 2); // +2 为负号和\0
            if (!final_result)
            {
                memory_error();
                free(result);
                return NULL;
            }
            final_result[0] = '-';
            strcpy(final_result + 1, result);
            free(result);
            return final_result;
        }
        return NULL;
    }
    if (!is_negative1 && is_negative2)
    {
        result = add_big_decimal(num1, num2); // num1 + num2
        return result;
    }

    // 如果两个数都是正数，直接调用subtract_big_decimal
    result = subtract_big_decimal(num1, num2);
    return result;
}
/*
// 辅助函数：将字符串表示的数字左移n位
char *shift_left(const char *num, int n)
{
    int len = strlen(num);
    char *result = (char *)calloc(len + n + 1, sizeof(char));
    if (!result)
    {
        memory_error();
        return NULL;
    }
    strcpy(result, num);
    memset(result + len, '0', n);
    result[len + n] = '\0';
    return result;
}
char *mul_kara(const char *num1, const char *num2)
{
    int len1 = strlen(num1);
    int len2 = strlen(num2);

    if (len1 < 2 || len2 < 2)
    {
        return multiply_big_abs(num1, num2);
    }

    int m = (len1 > len2 ? len1 : len2) / 2;

    // 声明分割后的高低位
    char *high1 = NULL, *low1 = NULL, *high2 = NULL, *low2 = NULL;

    // 安全分割数字
    if (len1 > m)
    {
        high1 = strndup(num1, len1 - m);
        low1 = strdup(num1 + len1 - m);
    }
    else
    {
        high1 = strdup("0");
        low1 = strdup(num1);
    }

    if (len2 > m)
    {
        high2 = strndup(num2, len2 - m);
        low2 = strdup(num2 + len2 - m);
    }
    else
    {
        high2 = strdup("0");
        low2 = strdup(num2);
    }

    // 计算z0, z2, z1
    char *z0 = mul_kara(low1, low2);
    char *z2 = mul_kara(high1, high2);
    char *high1_plus_low1 = add_big_abs(high1, low1);
    char *high2_plus_low2 = add_big_abs(high2, low2);
    char *z1 = mul_kara(high1_plus_low1, high2_plus_low2);

    // z1 - z2 - z0
    char *temp1 = subtract_big_abs(z1, z2);
    char *middle_term = subtract_big_abs(temp1, z0);

    // 计算最终结果
    char *shifted_z2 = shift_left(z2, 2 * m);
    char *shifted_middle = shift_left(middle_term, m);
    char *temp_result = add_big_abs(shifted_z2, shifted_middle);
    char *result = add_big_abs(temp_result, z0);

    // 清理资源
    free(high1);
    free(low1);
    free(high2);
    free(low2);
    free(high1_plus_low1);
    free(high2_plus_low2);
    free(z0);
    free(z2);
    free(z1);
    free(temp1);
    free(middle_term);
    free(shifted_z2);
    free(shifted_middle);
    free(temp_result);

    return result;
}

*/
// 正整数相乘
char *multiply_big_abs(const char *num1, const char *num2)
{
    int len1 = strlen(num1);
    int len2 = strlen(num2);
    int result_len = len1 + len2; // 结果最大长度为两乘数长度之和

    // 分配结果空间并初始化为0
    char *result = (char *)malloc((result_len + 1) * sizeof(char));
    if (!result)
    {
        memory_error();
        return NULL;
    }
    memset(result, '0', result_len);
    result[result_len] = '\0';

    // 从右向左遍历乘数的每一位
    for (int i = len2 - 1; i >= 0; i--)
    {
        int carry = 0;
        int n2 = num2[i] - '0';

        // 当前位与被乘数相乘
        for (int j = len1 - 1; j >= 0; j--)
        {
            int n1 = num1[j] - '0';
            int sum = (result[i + j + 1] - '0') + n1 * n2 + carry;

            result[i + j + 1] = (sum % 10) + '0';
            carry = sum / 10;
        }

        // 处理最后的进位
        result[i] += carry;
    }

    // 去除前导零
    char *start = result;
    while (*start == '0' && *(start + 1) != '\0')
    {
        start++;
    }

    if (start > result)
    {
        size_t newLen = strlen(start);
        memmove(result, start, newLen + 1);
    }

    return result;
}
// 正实数相乘
char *multiply_big_decimal(const char *num1, const char *num2)
{
    // 找到小数点位置
    const char *dot1 = strchr(num1, '.');
    const char *dot2 = strchr(num2, '.');

    if (!dot1 && !dot2)
    {
        return multiply_big_abs(num1, num2);
    }

    // 计算整数部分和小数部分的长度
    size_t int_len1 = dot1 ? (dot1 - num1) : strlen(num1);
    size_t int_len2 = dot2 ? (dot2 - num2) : strlen(num2);
    size_t dec_len1 = dot1 ? strlen(dot1 + 1) : 0;
    size_t dec_len2 = dot2 ? strlen(dot2 + 1) : 0;

    // 创建临时字符串来存储去掉小数点的数字
    size_t temp_size1 = int_len1 + dec_len1 + 1;
    size_t temp_size2 = int_len2 + dec_len2 + 1;
    char *temp1 = (char *)calloc(temp_size1, sizeof(char));
    char *temp2 = (char *)calloc(temp_size2, sizeof(char));

    if (!temp1 || !temp2)
    {
        free(temp1); // 安全释放
        free(temp2);
        return NULL;
    }

    // 复制整数部分
    strncpy(temp1, num1, int_len1);
    strncpy(temp2, num2, int_len2);

    // 复制小数部分(如果有)
    if (dot1)
    {
        strcat(temp1, dot1 + 1);
    }
    if (dot2)
    {
        strcat(temp2, dot2 + 1);
    }

    // 使用 multiply_big_abs 进行计算
    char *raw_result = multiply_big_abs(temp1, temp2);
    if (!raw_result)
    {
        free(temp1);
        free(temp2);
        return NULL;
    }

    // 计算结果中小数点的位置
    size_t total_dec_len = dec_len1 + dec_len2;
    size_t result_len = strlen(raw_result);

    // 分配最终结果的空间
    char *final_result = (char *)malloc(result_len + 2 + total_dec_len); // 确保空间足够
    if (!final_result)
    {
        free(temp1);
        free(temp2);
        free(raw_result);
        return NULL;
    }

    // 插入小数点
    if (total_dec_len > 0)
    {
        // 处理结果长度小于小数位数的情况
        if (result_len <= total_dec_len)
        {
            final_result[0] = '0';
            final_result[1] = '.';

            // 计算需要补充的前导零数量并确保正确
            size_t zeros_needed = total_dec_len - result_len;

            // 补充前导零
            for (size_t i = 0; i < zeros_needed; i++)
            {
                final_result[2 + i] = '0';
            }

            // 复制实际数字
            strcpy(final_result + 2 + zeros_needed, raw_result);
        }
        else
        {
            size_t dot_pos = result_len - total_dec_len;
            strncpy(final_result, raw_result, dot_pos);
            final_result[dot_pos] = '.';
            strcpy(final_result + dot_pos + 1, raw_result + dot_pos);
        }

        // 移除末尾的多余零
        char *end = final_result + strlen(final_result) - 1;
        while (end > final_result && *end == '0' && *(end - 1) != '.')
        {
            end--;
        }
        *(end + 1) = '\0';

        // 如果小数点在最后，移除它
        if (*end == '.')
        {
            *end = '\0';
        }
    }
    else
    {
        strcpy(final_result, raw_result);
    }

    // 清理
    free(temp1);
    free(temp2);
    free(raw_result);

    return final_result;
}
// 所有数相乘
char *complete_mul(const char *num1, const char *num2)
{
    // 检查输入是否带符号
    int is_negative1 = (num1[0] == '-');
    int is_signed1 = (num1[0] == '+' || is_negative1);
    int is_negative2 = (num2[0] == '-');
    int is_signed2 = (num2[0] == '+' || is_negative2);

    // 去掉符号
    if (is_signed1)
        num1++;
    if (is_signed2)
        num2++;

    char *result = multiply_big_decimal(num1, num2);

    // 如果两个数中只有一个是负数，结果为负
    if ((is_negative1 && !is_negative2) || (!is_negative1 && is_negative2))
    {
        if (result)
        {
            size_t len = strlen(result);
            char *final_result = (char *)malloc(len + 2); // +2 为负号和\0
            if (!final_result)
            {
                free(result);
                return NULL;
            }
            final_result[0] = '-';
            strcpy(final_result + 1, result);
            free(result);
            return final_result;
        }
        return NULL;
    }

    return result;
}

/*
// 除法辅助函数
//  计算 10^exp (返回字符串表示)
char *power_of_ten(int exp)
{
    if (exp < 0)
    {
        char *result = (char *)malloc(-exp + 3); // +3为"0."和'\0'
        if (!result)
        {
            memory_error();
            return NULL;
        }
        result[0] = '0';
        result[1] = '.';
        memset(result + 2, '0', -exp);
        result[-exp + 1] = '1';
        result[-exp + 2] = '\0';
        return result;
    }
    char *res = (char *)malloc(exp + 2);
    if (!res)
    {
        memory_error();
        return NULL;
    }
    res[0] = '1';
    memset(res + 1, '0', exp);
    res[exp + 1] = '\0';
    return res;
}
*/
// 使用牛顿迭代法计算一个数的逆元
// num2: 被除数
// precision: 精度
// len: num2的长度
// 返回值: num2的逆元
char *newton_inverse(const char *num2, int precision, int len)
{
    // 计算初始值 x = 10^(-n)，其中n是num2的长度
    int n = strlen(num2);

    // 创建初始值字符串 "0.0...01" (n-1个0)
    char *x = (char *)malloc(n + 3); // "0." + n-1个0 + "1" + "\0"
    if (!x)
    {
        memory_error();
        return NULL;
    }
    x[0] = '0';
    x[1] = '.';
    memset(x + 2, '0', n - 1);
    x[n + 1] = '1';
    x[n + 2] = '\0';

    // 开始迭代
    for (int i = 0; i < precision / 2; i++)
    {
        // 计算 num2 * x
        char *num2_x = complete_mul(num2, x);
        //printf("num2_x: %s\n", num2_x);
        if (!num2_x)
        {
            free(x);
            return NULL;
        }

        // 计算 2 - num2 * x
        char *two_minus_num2_x = complete_sub("2", num2_x);
        //printf("two_minus_num2_x: %s\n", two_minus_num2_x);
        free(num2_x);
        if (!two_minus_num2_x)
        {
            free(x);
            return NULL;
        }

        // 计算 x * (2 - num2 * x)
        char *new_x = complete_mul(x, two_minus_num2_x);
        free(two_minus_num2_x);
        free(x);
        if (!new_x)
        {
            return NULL;
        }
        // 限制精度到 len + precision 位
        const char *dot = strchr(new_x, '.');
        if (dot)
        {
            size_t dec_len = strlen(dot + 1);
            if (dec_len > len + precision)
            {
                // 截断小数部分
                new_x[dot - new_x + 1 + len + precision] = '\0';
            }
        }

        x = new_x;
        //printf("x: %s\n", x);
    }

    return x;
}


char *divide_big_abs(const char *num1, const char *num2, int scale)
{
    // 处理除以0的情况
    /*
    if (compare_numbers_abs(num2, "0") == 0)
    {
        return NULL;
    }
    */

    // 处理被除数为0的情况
    if (compare_numbers_abs(num1, "0") == 0)
    {
        return strdup("0");
    }
    int n1 = strlen(num1);
    int precision = n1 + scale; // 确保精度足够

    // 计算 num2 的逆元
    char *inv_num2 = newton_inverse(num2, precision, n1);
    if (!inv_num2)
    {
        return NULL;
    }

    // 计算 num1 * (1 / num2)
    char *result = complete_mul(num1, inv_num2);
    free(inv_num2); // 及时释放不再需要的内存

    if (!result)
    {
        return NULL;
    }

    // 检查结果是否已包含小数点
    const char *dot = strchr(result, '.');
    if (dot)
    {
        // 结果已经包含小数点，只需调整精度
        int dec_len = strlen(dot + 1);

        if (dec_len > scale)
        {
            // 截断超出指定精度的小数位 - 安全方式
            size_t truncate_pos = dot - result + 1 + scale;
            size_t total_len = strlen(result);

            // 确保不会越界访问
            if (truncate_pos < total_len)
            {
                char *truncated_result = (char *)malloc(truncate_pos + 1);
                if (!truncated_result)
                {
                    free(result);
                    return NULL;
                }
                strncpy(truncated_result, result, truncate_pos);
                truncated_result[truncate_pos] = '\0';
                free(result);
                result = truncated_result;
            }
        }
        else if (dec_len < scale)
        {
            // 需要补零
            int zeros_needed = scale - dec_len;
            char *final_result = (char *)malloc(strlen(result) + zeros_needed + 1);
            if (!final_result)
            {
                free(result);
                return NULL;
            }

            strcpy(final_result, result);
            memset(final_result + strlen(result), '0', zeros_needed);
            final_result[strlen(result) + zeros_needed] = '\0';

            free(result);
            result = final_result;
        }
    }
    else
    {
        // 原始结果没有小数点，按原来逻辑处理
        if (scale > 0)
        {
            int len = strlen(result);
            char *final_result = (char *)malloc(len + 2); // +2 为小数点和'\0'
            if (!final_result)
            {
                free(result);
                return NULL;
            }

            if (len > scale)
            {
                strncpy(final_result, result, len - scale);
                final_result[len - scale] = '.';
                strncpy(final_result + len - scale + 1, result + len - scale, scale);
                final_result[len + 1] = '\0';
            }
            else
            {
                final_result[0] = '0';
                final_result[1] = '.';
                memset(final_result + 2, '0', scale - len);
                strcpy(final_result + 2 + (scale - len), result);
                final_result[2 + (scale - len) + len] = '\0';
            }

            free(result);
            result = final_result;
        }
    }

    // 移除末尾多余的0
    char *end = result + strlen(result) - 1;
    while (end > result && *end == '0' && *(end - 1) != '.')
    {
        end--;
    }
    *(end + 1) = '\0';

    // 如果小数点在最后，移除它
    if (*end == '.')
    {
        *end = '\0';
    }

    return result;
}
char *complete_div(const char *num1, const char *num2, int scale)
{
    // 检查输入是否带符号
    int is_negative1 = (num1[0] == '-');
    int is_signed1 = (num1[0] == '+' || is_negative1);
    int is_negative2 = (num2[0] == '-');
    int is_signed2 = (num2[0] == '+' || is_negative2);

    // 去掉符号
    if (is_signed1)
        num1++;
    if (is_signed2)
        num2++;

    char *result = divide_big_abs(num1, num2, scale);

    // 如果两个数中只有一个是负数，结果为负
    if ((is_negative1 && !is_negative2) || (!is_negative1 && is_negative2))
    {
        if (result)
        {
            size_t len = strlen(result);
            char *final_result = (char *)malloc(len + 2); // +2 为负号和\0
            if (!final_result)
            {
                free(result);
                return NULL;
            }
            final_result[0] = '-';
            strcpy(final_result + 1, result);
            free(result);
            return final_result;
        }
        return NULL;
    }

    return result;
}


///////////////////////////////////// 计算器核心////////////////////////////////////////
char *calculate_core(ExprNode *root)
{
    if (!root)
    {
        return NULL;
    }

    // 如果是叶子节点，直接返回其值
    if (!root->left && !root->right)
    {
        // 检查节点类型
        if (root->type == VARIABLE)
        {
            // 查找变量值
            char *var_value = findVariable(root->value);
            if (var_value)
            {
                return strdup(var_value);
            }
            else
            {
                printf("RunError: Variable '%s' not defined\n", root->value);
                return NULL;
            }
        }
        else
        {
            // 对于普通操作数，直接返回其值
            return strdup(root->value);
        }
    }

    // 递归计算左子树和右子树的值
    char *leftValue = calculate_core(root->left);
    char *rightValue = calculate_core(root->right);

    if (!leftValue || !rightValue)
    {
        free(leftValue);
        free(rightValue);
        return NULL;
    }

    char *result = NULL;

    // 根据操作符进行相应的运算
    if (strcmp(root->value, "+") == 0)
    {
        result = complete_add(leftValue, rightValue);
    }
    else if (strcmp(root->value, "-") == 0)
    {
        result = complete_sub(leftValue, rightValue);
    }
    else if (strcmp(root->value, "x") == 0)
    {
        result = complete_mul(leftValue, rightValue);
    }
    else if (strcmp(root->value, "/") == 0)
    {
        if (strcmp(rightValue, "0") == 0)
        {
            printf("RunError: Division by zero\n");
            free(leftValue);
            free(rightValue);
            return NULL;
        }
        result = complete_div(leftValue, rightValue, scale);
    }

    free(leftValue);
    free(rightValue);

    return result;
}

// 执行函数调用
char *executeFunction(const char *methodCall)
{
    methodCall++; // 跳过'/'开头
    if (!isValidMethodCall(methodCall))
    {
        printf("TokenizeError:Invalid function call format\n");
        return NULL;
    }

    // 1. 提取函数名
    int i = 1; // 跳过'_'
    while (methodCall[i] && methodCall[i] != '[')
        i++;
    char *funcName = (char *)malloc(i + 1);
    strncpy(funcName, methodCall, i);
    funcName[i] = '\0';

    // 2. 查找函数
    Function *func = findFunction(funcName);
    free(funcName);

    if (!func)
    {
        printf("RunError:Function not found\n");
        return NULL;
    }

    // 3. 提取参数值并检查参数数量
    char *params_start = strchr(methodCall, '[') + 1;
    char *params_end = strchr(params_start, ']');
    int params_len = params_end - params_start;
    char *params = (char *)malloc(params_len + 1);
    strncpy(params, params_start, params_len);
    params[params_len] = '\0';

    // 计算传入的参数数量
    int input_param_count = 1; // 至少有一个参数
    for (int i = 0; i < params_len; i++)
    {
        if (params[i] == ';')
        {
            input_param_count++;
        }
    }

    // 检查参数数量是否匹配
    if (input_param_count != func->param_count)
    {
        printf("RunError: Function expects %d parameters but got %d\n",
               func->param_count, input_param_count);
        free(params);
        return NULL;
    }

    // 4. 保存可能被覆盖的变量并检查变量表容量
    TempVariable *tempVars = (TempVariable *)malloc(sizeof(TempVariable) * func->param_count);
    int tempVarCount = 0;

    // 计算需要添加的新变量数量
    int newVarsNeeded = 0;
    for (int i = 0; i < func->param_count; i++)
    {
        if (!findVariable(func->params[i]))
        {
            newVarsNeeded++;
        }
    }

    // 检查变量表是否有足够空间
    if (varCount + newVarsNeeded > MAX_VARIABLES)
    {
        printf("MemoryError: Not enough space in variable table. Need %d more slots\n",
               newVarsNeeded - (MAX_VARIABLES - varCount));
        free(params);
        free(tempVars);
        return NULL;
    }

    // 分割参数值
    char *param_value = strtok(params, ";");
    for (int i = 0; i < func->param_count; i++)
    {
        if (!param_value)
        {
            printf("RunError:Insufficient parameters\n");
            free(params);
            // 恢复已保存的变量
            for (int j = 0; j < tempVarCount; j++)
            {
                setVariable(tempVars[j].name, tempVars[j].value);
                free(tempVars[j].value);
            }
            free(tempVars);
            return NULL;
        }

        // 检查是否存在同名变量并保存
        char *existing_value = findVariable(func->params[i]);
        if (existing_value)
        {
            strcpy(tempVars[tempVarCount].name, func->params[i]);
            tempVars[tempVarCount].value = strdup(existing_value);
            tempVarCount++;
        }

        // 设置参数值
        if (!setVariable(func->params[i], param_value))
        {
            printf("RunError: Failed to set parameter '%s'\n", func->params[i]);
            // 清理已保存的临时变量
            for (int j = 0; j < tempVarCount; j++)
            {
                setVariable(tempVars[j].name, tempVars[j].value);
                free(tempVars[j].value);
            }
            free(tempVars);
            free(params);
            return NULL;
        }
        param_value = strtok(NULL, ";");
    }

    // 5. 执行函数体
    char *result = calculate_core(func->body);

    // 6. 恢复被覆盖的变量
    for (int i = 0; i < tempVarCount; i++)
    {
        setVariable(tempVars[i].name, tempVars[i].value);
        free(tempVars[i].value);
    }

    // 7. 清理临时变量
    for (int i = 0; i < func->param_count; i++)
    {
        if (tempVarCount == 0 || strcmp(tempVars[0].name, func->params[i]) != 0)
        {
            char *var_value = findVariable(func->params[i]);
            if (var_value)
            {
                // 删除临时变量
                for (int j = 0; j < varCount; j++)
                {
                    if (strcmp(variables[j].name, func->params[i]) == 0)
                    {
                        free(variables[j].value);
                        // 移动后面的变量
                        for (int k = j; k < varCount - 1; k++)
                        {
                            strcpy(variables[k].name, variables[k + 1].name);
                            variables[k].value = variables[k + 1].value;
                        }
                        varCount--;
                        break;
                    }
                }
            }
        }
    }

    free(tempVars);
    free(params);
    return result;
}
void printHelp()
{
    printf("Simple Calculator\n");
    printf("Usage: ./calculator [expression] [options]\n");

    printf("Options:\n");
    printf("  -h, --help\t\tDisplay this information\n");
    printf("  -v, --version\t\tDisplay version information\n");
    printf("  -s, --scale\t\tSet the scale for division (default: 20)\n");

    printf("\nCommands:\n");
    printf("  state\t\t\tDisplay current variables and functions\n");
    printf("  quit\t\t\tExit the interactive mode\n");

    printf("\nExpressions:\n");
    printf("  You can enter mathematical expressions using +, -, x, / operators.\n");
    printf("  Example: 123 + 456\n");

    printf("\nVariables:\n");
    printf("  You can assign values to variables using the '=' operator.\n");
    printf("  Example: x = 123 + 456\n");

    printf("\nFunctions:\n");
    printf("  You can define functions using the following format:\n");
    printf("  _functionName[param1;param2]{expression}\n");
    printf("  Example: _add[a;b]{a + b}\n");
    printf("  You can call functions using the following format:\n");
    printf("  /functionName[param1;param2]\n");
    printf("  Example: /add[123;456]\n");
}

void printVersion()
{
    printf("Simple Calculator v1.2\n");
}

// 交互模式主函数
int interactiveMode(int scale)
{
    char input[1024];
    char *result = NULL;

    printf("Simple Calculator (type 'quit' to exit)\n");

    while (1)
    {
        printf("> ");
        if (!fgets(input, sizeof(input), stdin))
        {
            break;
        }

        // 删除换行符
        input[strcspn(input, "\n")] = 0;
        if (strcmp(input, "state") == 0)
        {
            displayState();
            continue;
        }
        // 检查是否退出
        if (strcmp(input, "quit") == 0)
        {
            break;
        }
        // 检查是否为空输入
        if (strlen(input) == 0)
        {
            continue;
        }
        // 检查是否为函数定义 (以_开头)
        if (input[0] == '_')
        {
            if (!parseAndSaveFunction(input))
            {
                //printf("TokenizeError:Failed to define function\n");
            }
            else
            {
                printf("Function defined successfully\n");
            }
            continue;
        }
        // 检查是否为变量赋值
        char *equals_pos = strchr(input, '=');         // 寻找等号的位置
        if (equals_pos != NULL && equals_pos != input) // 确保等号不是输入的第一个字符
        {
            // 提取变量名
            char var_name[MAX_NAME_LEN + 1];
            int name_len = equals_pos - input;

            // 复制变量名部分
            strncpy(var_name, input, name_len);
            var_name[name_len] = '\0';

            // 去除变量名前后空格
            char *var_start = var_name;
            char *var_end = var_name + name_len - 1;

            while (*var_start && isspace(*var_start))
                var_start++;
            while (var_end > var_start && isspace(*var_end))
                *var_end-- = '\0';

            // 获取表达式部分
            char *expr_start = equals_pos + 1;
            while (*expr_start && isspace(*expr_start))
                expr_start++;

            // 解析表达式并计算
            ExprNode *expr_root = tokenize(expr_start);
            if (expr_root)
            {
                char *expr_result = calculate_core(expr_root);
                if (expr_result)
                {
                    // 设置变量
                    if (setVariable(var_start, expr_result))
                    {
                        printf("Variable %s set to %s\n", var_start, expr_result);
                    }
                    free(expr_result);
                }
                else
                {
                    printf("Error evaluating variable's expression.\n");
                }
                freeExprTree(expr_root);
            }
            else
            {
                printf("Invalid expression\n");
            }
            continue;
        }
        // 检查是否为函数调用
        if (input[0] == '/')
        {
            char *result = executeFunction(input);
            if (result)
            {
                printf("Result: %s\n", result);
                free(result);
            }
            continue;
        }
        // 解析输入并生成表达式树
        ExprNode *root = tokenize(input);

        // 计算表达式树的结果
        result = calculate_core(root);

        // 输出结果
        if (result)
        {
            printf("Result: %s\n", result);
            free(result);
        }

        // 清理表达式树
        freeExprTree(root);
    }

    printf("Goodbye!\n");
    return EXIT_SUCCESS;
}

// 命令行模式主函数
int commandLineMode(const char *num1, const char *operator, const char * num2, int scale)
{
    char *result = NULL;

    // 验证操作符
    if (strlen(operator) != 1 || !isOperator(operator[0]))
    {
        fprintf(stderr, "TokenizeError: Invalid operator. Use +, -, x, or /\n");
        return EXIT_FAILURE;
    }

    if (!isValidNumber(num1) || !isValidNumber(num2))
    {
        printf("TokenizeError: Number can not be interpreted\n");
        return EXIT_FAILURE;
    }

    // 执行计算
    switch (operator[0])
    {
    case '+':
        result = complete_add(num1, num2);
        break;
    case '-':
        result = complete_sub(num1, num2);
        break;
    case 'x':
        result = complete_mul(num1, num2);
        break;
    case '/':
        if (strcmp(num2, "0") == 0) {
            printf("RunError: Division by zero\n");
            return EXIT_FAILURE;
        }
        result = complete_div(num1, num2, scale);
        break;
    default:
        printf("Error: Invalid operator '%c'. Use +, -, x, or /\n", operator[0]);
        return EXIT_FAILURE;
    }

    // 检查计算结果
    if (!result)
    {
        printf("RunError: Badthings occurred during calculation\n");
        return EXIT_FAILURE;
    }

    // 打印结果
    printf("%s\n", result);
    free(result);
    return EXIT_SUCCESS;
}
// 辅助函数：检查输入是否是操作符/变量/数字/函数
int isOperator(char c)
{
    return (c == '+' || c == '-' || c == 'x' || c == '/');
}

int isValidIdentifier(const char *str)
{
    // 首字符必须是字母或下划线
    if (!isalpha(str[0]))
    {
        return 0;
    }

    if (str == NULL || *str == '\0')
    {
        return 0; // 空字符串无效
    }

    for (int i = 0; str[i] != '\0'; i++)
    {
        if (!isalnum(str[i]))
        {
            return 0; // 包含非法字符
        }
        if (isspace(str[i]))
        {
            return 0; // 包含空格
        }
    }

    return 1; // 所有字符都有效
}

int isValidNumber(const char *str)
{
    if (str == NULL || *str == '\0')
    {
        return 0; // 空字符串无效
    }

    int i = 0;

    // 跳过可选的正负号
    if (str[0] == '+' || str[0] == '-')
    {
        i++;
    }

    int hasDecimal = 0; // 标记是否已经有小数点

    // 检查剩余的字符
    for (; str[i] != '\0'; i++)
    {
        if (isdigit(str[i]))
        {
            continue; // 是数字，继续
        }
        else if (str[i] == '.' && !hasDecimal)
        {
            hasDecimal = 1; // 第一次遇到小数点
        }
        else
        {
            return 0; // 包含非法字符
        }
    }

    // 确保至少有一个数字
    if (i == (str[0] == '+' || str[0] == '-' ? 1 : 0))
    {
        return 0;
    }

    return 1; // 所有字符都有效
}

int isValidMethodCall(const char *str)
{
    if (str == NULL || *str == '\0')
    {
        return 0; // 空字符串无效
    }

    int len = strlen(str);
    // 创建可修改的字符串副本
    char *temp = (char *)malloc(len + 1);
    if (!temp)
    {
        return 0;
    }
    strncpy(temp, str, len + 1);
    int result = 0;
    int i = 0;

    // 1. 必须以 '_' 开头
    if (temp[i++] != '_')
    {
        // 一坨没用的代码的时候使用goto语句
        goto cleanup;
    }

    // 2. 方法名称 (必须是字母或数字)
    while (i < len && isalnum(temp[i]))
    {
        i++;
    }

    // 3. 必须紧接 '['
    if (i >= len || temp[i++] != '[')
    {
        goto cleanup;
    }

    // 4. 变量列表 (以 ';' 分隔)
    int varStart = i;
    while (i < len && temp[i] != ']')
    {
        if (temp[i] == ';')
        {
            // 先转化为字符串终止符进行判断,之后再转化回来
            temp[i] = '\0';
            if (!isValidNumber(temp + varStart))
            {
                goto cleanup;
            }
            temp[i] = ';';
            varStart = i + 1;
            i++;
            continue;
        }
        i++;
    }
    if (varStart < i)
    {
        temp[i] = '\0';
        if (!isValidNumber(temp + varStart))
        {
            goto cleanup;
        }
        temp[i] = ']';
    }

    // 5. 必须以 ']' 结尾
    if (i >= len || temp[i++] != ']')
    {
        goto cleanup;
    }

    // 6. 整个字符串中不能包含空格
    for (int j = 0; j < len; j++)
    {
        if (isspace(temp[j]))
        {
            goto cleanup;
        }
    }

    // 7. 必须是字符串的结尾
    if (i != len)
    {
        goto cleanup;
    }

    result = 1; // 所有检查都通过

cleanup:
    free(temp);
    return result;
}

int isValidMethodName(const char *str)
{
    // 检查空指针和空字符串
    if (str == NULL || *str == '\0')
    {
        return 0;
    }

    // 检查第一个字符必须是下划线
    if (str[0] != '_')
    {
        return 0;
    }

    // 检查下划线后面必须至少有一个字符
    if (str[1] == '\0')
    {
        return 0;
    }

    // 检查剩余字符必须是字母或数字
    for (int i = 1; str[i] != '\0'; i++)
    {
        if (!isalnum(str[i]))
        {
            return 0;
        }
        // 检查是否包含空格
        if (isspace(str[i]))
        {
            return 0;
        }
    }

    return 1;
}
// 用于检查函数声明中变量是否合法的辅助函数

// 检查表达式中的变量是否都在参数列表中
int isVariableInList(const char *var, const char *paramList)
{
    if (!var || !paramList)
        return 0;

    char *temp = strdup(paramList);
    if (!temp)
        return 0;

    int result = 0;
    char *token = strtok(temp, ";");
    while (token)
    {
        if (strcmp(token, var) == 0)
        {
            result = 1;
            break;
        }
        token = strtok(NULL, ";");
    }

    free(temp);
    return result;
}

// 修改 validateExpressionVariables 函数
int validateExpressionVariables(const char *expr, const char *paramList)
{
    if (!expr || !paramList)
        return 0;

    // 创建表达式的副本用于分割
    char *exprCopy = strdup(expr);
    if (!exprCopy)
        return 0;

    int result = 1;
    char *token = strtok(exprCopy, " ");

    while (token && result)
    {
        // 如果不是操作符、括号或数字，就认为是变量，需要检查
        if (!isOperator(token[0]) &&
            token[0] != '(' &&
            token[0] != ')' &&
            !isValidNumber(token))
        {

            // 检查变量名是否完全匹配参数列表中的某个参数
            int found = 0;
            char *params_copy = strdup(paramList);
            if (params_copy)
            {
                char *param = strtok(params_copy, ";");
                while (param)
                {
                    if (strcmp(token, param) == 0)
                    {
                        found = 1;
                        break;
                    }
                    param = strtok(NULL, ";");
                }
                free(params_copy);
            }

            if (!found)
            {
                printf("RunError: Variable '%s' not found in parameter list: [%s]\n",
                       token, paramList);
                result = 0;
            }
        }
        token = strtok(NULL, " ");
    }

    free(exprCopy);
    return result;
}

int isValidMethodDeclaration(const char *str)
{
    if (str == NULL || *str == '\0')
    {
        return 0;
    }

    int len = strlen(str);
    char *temp = (char *)malloc(len + 1);
    if (!temp)
    {
        return 0;
    }
    strncpy(temp, str, len + 1);
    int result = 0;
    int i = 0;

    // 1. 检查方法名部分 (以 '_' 开头)
    if (temp[i++] != '_')
    {
        printf("TokenizeError: Method name must start with '_'\n");
        goto cleanup;
    }

    // 2. 检查方法名其余部分
    while (i < len && isalnum(temp[i]))
    {
        i++;
    }

    // 3. 检查 '['
    if (i >= len || temp[i++] != '[')
    {
        printf("TokenizeError: Expected '['\n");
        goto cleanup;
    }

    // 4. 检查变量列表 (以 ';' 分隔的标识符)
    int varStart = i;
    while (i < len && temp[i] != ']')
    {
        if (temp[i] == ';')
        {
            temp[i] = '\0';
            if (!isValidIdentifier(temp + varStart))
            {
                printf("TokenizeError: Invalid variable name: %s\n", temp + varStart);
                goto cleanup;
            }
            temp[i] = ';';
            varStart = i + 1;
            i++;
            continue;
        }
        i++;
    }

    // 检查最后一个变量
    if (varStart < i)
    {
        temp[i] = '\0';
        if (!isValidIdentifier(temp + varStart))
        {
            printf("TokenizeError: Invalid variable name: %s\n", temp + varStart);
            goto cleanup;
        }
        temp[i] = ']';
    }

    // 5. 检查 ']'
    if (i >= len || temp[i++] != ']')
    {
        printf("TokenizeError: Expected ']'\n");
        goto cleanup;
    }

    // 6. 检查 '{'
    if (i >= len || temp[i++] != '{')
    {
        printf("TokenizeError: Expected '{'\n");
        goto cleanup;
    }

    // 7. 查找匹配的 '}'
    int exprStart = i;
    int braceCount = 1;
    while (i < len && braceCount > 0)
    {
        if (temp[i] == '{')
            braceCount++;
        if (temp[i] == '}')
            braceCount--;
        i++;
    }

    // 确保找到匹配的 '}'
    if (braceCount != 0 || i > len)
    {
        printf("TokenizeError: Missing '}'\n");
        goto cleanup;
    }
    // 提取参数列表
    int paramStart = strchr(temp, '[') - temp + 1;
    int paramEnd = strchr(temp, ']') - temp;
    char *paramList = (char *)malloc(paramEnd - paramStart + 1);
    if (!paramList)
    {
        printf("MemoryError: Memory allocation failed\n");
        goto cleanup;
    }
    strncpy(paramList, temp + paramStart, paramEnd - paramStart);
    paramList[paramEnd - paramStart] = '\0';
    // 先验证表达式中的变量
    if (!validateExpressionVariables(temp + exprStart, paramList))
    {
        printf("TokenizeError: Invalid variable in expression\n");
        free(paramList);
        goto cleanup;
    }
    // 8. 提取并验证表达式
    temp[i - 1] = '\0'; // 临时移除 '}'
    ExprNode *exprTree = tokenize(temp + exprStart);
    if (!exprTree)
    {
        printf("TokenizeError: Invalid expression\n");
        goto cleanup;
    }
    freeExprTree(exprTree);
    free(paramList);

    // 9. 确保这是字符串的结尾
    if (i != len)
    {
        printf("TokenizeError: Unexpected characters after '}'\n");
        goto cleanup;
    }

    result = 1;

cleanup:
    free(temp);
    return result;
}

// 辅助函数：返回操作符的优先级
int getPriority(char op)
{
    switch (op)
    {
    case '+':
    case '-':
        return 1;
    case 'x':
    case '/':
        return 2;
    default:
        return 0; // '(' 的优先级设为最低，方便处理
    }
}

// 创建一个表达式节点,并返回指向该节点的指针
ExprNode *createExprNode(const char *value, NodeType type)
{
    ExprNode *node = (ExprNode *)malloc(sizeof(ExprNode));
    if (!node)
    {
        return NULL;
    }
    node->value = strdup(value);
    // 避免直接复制地址
    // 将传入的字符串复制一份并把这个副本的内存地址赋给节点的value成员
    node->type = type;
    // 左右节点都初始化为NULL
    node->left = NULL;
    node->right = NULL;
    return node;
}

// 递归释放表达式树
void freeExprTree(ExprNode *root)
{
    if (!root)
    {
        return;
    }
    freeExprTree(root->left);
    freeExprTree(root->right);
    free(root->value);
    free(root);
}

// 创建表达式树
ExprNode *tokenize(const char *input)
{
    if (!input)
        return NULL;

    // 创建输入字符串的副本
    char *inputCopy = strdup(input);
    if (!inputCopy)
        return NULL;

    Stack *operatorStack = createStack();
    Stack *operandStack = createStack();

    char *token = strtok(inputCopy, " ");

    while (token != NULL)
    {
        if (strlen(token) == 1 && isOperator(token[0]))
        {
            // 处理运算符
            char *topOp;
            while (!isStackEmpty(operatorStack) &&
                   (topOp = (char *)peek(operatorStack)) != NULL &&
                   *topOp != '(' &&
                   getPriority(*topOp) >= getPriority(token[0]))
            {

                // 弹出操作符和操作数
                void *opPtr = pop(operatorStack);
                ExprNode *right = (ExprNode *)pop(operandStack);
                ExprNode *left = (ExprNode *)pop(operandStack);

                // 创建新节点
                ExprNode *node = createExprNode((char *)opPtr, OPERATOR);
                free(opPtr); // 释放操作符内存
                node->left = left;
                node->right = right;

                push(operandStack, (void *)node);
            }
            // 将当前操作符入栈
            char *op = strdup(token);
            push(operatorStack, (void *)op);
        }
        else if (token[0] == '(')
        {
            char *paren = strdup(token);
            push(operatorStack, (void *)paren);
        }
        else if (token[0] == ')')
        {
            char *topOp;
            while (!isStackEmpty(operatorStack) &&
                   (topOp = (char *)peek(operatorStack)) != NULL &&
                   *topOp != '(')
            {
                void *opPtr = pop(operatorStack);
                ExprNode *right = (ExprNode *)pop(operandStack);
                ExprNode *left = (ExprNode *)pop(operandStack);

                ExprNode *node = createExprNode((char *)opPtr, OPERATOR);
                free(opPtr);
                node->left = left;
                node->right = right;

                push(operandStack, (void *)node);
            }
            if (!isStackEmpty(operatorStack))
            {
                void *leftParen = pop(operatorStack);
                free(leftParen);
            }
        }
        else
        {
            NodeType type;
            if (isValidNumber(token))
            {
                type = OPERAND;
            } /*else if (isValidMethodCall(token)) {
                type = FUNCTION;}*/
            else if (isValidIdentifier(token))
            {
                type = VARIABLE;
            }
            else
            {
                // 清理并返回
                while (!isStackEmpty(operatorStack))
                {
                    void *op = pop(operatorStack);
                    free(op);
                }
                while (!isStackEmpty(operandStack))
                {
                    ExprNode *node = (ExprNode *)pop(operandStack);
                    freeExprTree(node);
                }
                freeStack(operatorStack);
                freeStack(operandStack);
                free(inputCopy);
                printf("TokenizeError: The input can not be interpreted as VARIABLE or OPERAND.\n");
                return NULL;
            }

            ExprNode *node = createExprNode(token, type);
            push(operandStack, (void *)node);
        }

        token = strtok(NULL, " ");
    }

    // 处理剩余的运算符
    while (!isStackEmpty(operatorStack))
    {
        void *opPtr = pop(operatorStack);
        ExprNode *right = (ExprNode *)pop(operandStack);
        ExprNode *left = (ExprNode *)pop(operandStack);

        ExprNode *node = createExprNode((char *)opPtr, OPERATOR);
        free(opPtr);
        node->left = left;
        node->right = right;

        push(operandStack, (void *)node);
    }

    // 获取最终结果
    ExprNode *result = NULL;
    if (!isStackEmpty(operandStack))
    {
        result = (ExprNode *)pop(operandStack);
    }

    // 清理资源
    freeStack(operatorStack);
    freeStack(operandStack);
    free(inputCopy);

    return result;
}
Stack *createStack()
{
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    if (stack)
    {
        stack->top = -1;
    }
    return stack;
}

void push(Stack *stack, void *data)
{
    if (stack->top < STACK_MAX - 1)
    {
        stack->data[++stack->top] = data;
    }
    else
    {
        printf("MemoryError: stackoverflow！\n");
    }
}

void *pop(Stack *stack)
{
    if (!isStackEmpty(stack))
    {
        return stack->data[stack->top--];
    }
    return NULL;
}

void *peek(Stack *stack)
{
    if (!isStackEmpty(stack))
    {
        return stack->data[stack->top];
    }
    return NULL;
}

int isStackEmpty(Stack *stack)
{
    return stack->top == -1;
}

void freeStack(Stack *stack)
{
    free(stack);
}

// 查找函数
Function *findFunction(const char *name)
{
    for (int i = 0; i < funcCount; i++)
    {
        if (strcmp(functions[i].name, name) == 0)
        {
            return &functions[i];
        }
    }
    return NULL;
}

// 函数声明的解析和保存
int parseAndSaveFunction(const char *declaration)
{
    if (!isValidMethodDeclaration(declaration))
    {
        printf("TokenizeError:Invalid function declaration\n");
        return 0;
    }

    // 如果函数表已满
    if (funcCount >= MAX_FUNCTIONS)
    {
        printf("MemoryError: Function table is full\n");
        return 0;
    }

    int len = strlen(declaration);
    char *temp = (char *)malloc(len + 1);
    if (!temp)
    {
        return 0;
    }
    strncpy(temp, declaration, len + 1);

    // 1. 提取函数名
    int i = 1;         // 跳过开头的'_'
    int nameStart = 1; // 从'_'后面开始
    while (i < len && (isalnum(declaration[i]) || declaration[i] == '_'))
    {
        i++;
    }
    int nameLen = i - nameStart;

    // 复制函数名（不包括开头的'_'）
    char *funcName = (char *)malloc(nameLen + 2); // +2 for '_' and '\0'
    if (!funcName)
    {
        free(temp);
        return 0;
    }
    funcName[0] = '_'; // 添加'_'
    strncpy(funcName + 1, declaration + nameStart, nameLen);
    funcName[nameLen + 1] = '\0';

    // 检查函数是否已存在
    if (findFunction(funcName))
    {
        printf("RunError: Function '%s' already exists\n", funcName);
        free(funcName);
        free(temp);
        return 0;
    }

    // 保存函数名
    strncpy(functions[funcCount].name, funcName, MAX_NAME_LEN - 1);
    functions[funcCount].name[MAX_NAME_LEN - 1] = '\0';

    free(funcName);

    // 2. 提取参数列表
    char *params_start = strchr(declaration, '[') + 1;
    char *params_end = strchr(params_start, ']');
    int params_len = params_end - params_start;

    char *params_list = (char *)malloc(params_len + 1);
    if (!params_list)
    {
        free(temp);
        return 0;
    }
    strncpy(params_list, params_start, params_len);
    params_list[params_len] = '\0';

    // 解析参数
    functions[funcCount].param_count = 0;
    char *param = strtok(params_list, ";");
    while (param && functions[funcCount].param_count < MAX_FUNCTION_PARAM_LEN)
    {
        functions[funcCount].params[functions[funcCount].param_count] = strdup(param);
        if (!functions[funcCount].params[functions[funcCount].param_count])
        {
            // 内存分配失败，清理并返回
            for (int j = 0; j < functions[funcCount].param_count; j++)
            {
                free(functions[funcCount].params[j]);
            }
            free(params_list);
            free(temp);
            return 0;
        }
        functions[funcCount].param_count++;
        param = strtok(NULL, ";");
    }

    // 3. 提取函数体
    char *body_start = strchr(declaration, '{') + 1;
    char *body_end = strrchr(declaration, '}');
    *body_end = '\0'; // 临时移除结尾的 '}'

    // 创建函数体的表达式树
    functions[funcCount].body = tokenize(body_start);
    if (!functions[funcCount].body)
    {
        // 清理已分配的内存
        for (int j = 0; j < functions[funcCount].param_count; j++)
        {
            free(functions[funcCount].params[j]);
        }
        free(params_list);
        free(temp);
        return 0;
    }

    funcCount++;
    free(params_list);
    free(temp);
    return 1;
}

// 清理函数表的辅助函数
void freeFunctions()
{
    for (int i = 0; i < funcCount; i++)
    {
        for (int j = 0; j < functions[i].param_count; j++)
        {
            free(functions[i].params[j]);
        }
        freeExprTree(functions[i].body);
    }
    funcCount = 0;
}

// 显示当前变量和方程
void displayState()
{
    printf("\n=== Current State ===\n");

    // 显示所有变量
    printf("\nVariables (%d/%d):\n", varCount, MAX_VARIABLES);
    if (varCount > 0)
    {
        printf("Name       Value\n");
        printf("--------------------\n");
        for (int i = 0; i < varCount; i++)
        {
            printf("%-10s %s\n", variables[i].name, variables[i].value);
        }
    }
    else
    {
        printf("No variables defined\n");
    }

    // 显示所有函数
    printf("\nFunctions (%d/%d):\n", funcCount, MAX_FUNCTIONS);
    if (funcCount > 0)
    {
        printf("Name       Parameters\n");
        printf("--------------------\n");
        for (int i = 0; i < funcCount; i++)
        {
            printf("%-10s ", functions[i].name);
            // 打印参数列表
            printf("[");
            for (int j = 0; j < functions[i].param_count; j++)
            {
                printf("%s", functions[i].params[j]);
                if (j < functions[i].param_count - 1)
                {
                    printf(";");
                }
            }
            printf("]\n");
        }
    }
    else
    {
        printf("No functions defined\n");
    }
    printf("\n===================\n");
}
// 找到变量
char *findVariable(const char *name)
{
    for (int i = 0; i < varCount; i++)
    {
        if (strcmp(variables[i].name, name) == 0)
        {
            return variables[i].value;
        }
    }
    return NULL;
}

// 设置变量值
int setVariable(const char *name, const char *value)
{
    if (!isValidIdentifier(name))
    {
        printf("TokenizeError:Contains invalid identifier.\n");
        return 0;
    }

    if (strlen(name) > MAX_NAME_LEN)
    {
        printf("TokenizeError:Name is too long , the maximum length is %d\n", MAX_NAME_LEN);
        return 0;
    }

    // 判断变量的value是否合法的功能已经在表达式转换tokenize函数中实现了

    // 查找变量是否已存在
    for (int i = 0; i < varCount; i++)
    {
        if (strcmp(variables[i].name, name) == 0)
        {
            // 更新值
            free(variables[i].value);
            variables[i].value = strdup(value);
            return 0;
        }
    }

    // 添加新变量
    if (varCount < MAX_VARIABLES)
    {

        strncpy(variables[varCount].name, name, MAX_NAME_LEN - 1); // 如果变量名长度小于 MAX_NAME_LEN - 1，则只复制实际长度的字符
        variables[varCount].name[MAX_NAME_LEN - 1] = '\0';         // 确保字符串结束
        variables[varCount].value = strdup(value);
        varCount++;
    }
    else
    {
        printf("MemoryError: Variable table is full\n");
        return 0;
    }
}

// 释放变量表
void freeVariables()
{
    for (int i = 0; i < varCount; i++)
    {
        free(variables[i].value);
    }
    varCount = 0;
}

// 0.0000000000000000000000001623789679814300000000000000000004198578167894627938461 - 0.12379

// 992605220868066282766377431096119976175390944321187133079925859643530232757637994382166
// 99260522086806628276637743109611997617539094432118713307992585964353023275763799438216608724
// 617937590195283227456782666320189708558797514975705685.85514424348023553810121087543805282047
// 617937590195283227456782666320189708558797514975705685.85514424348023553810121087543805282047

// 78317819057891910748902.17857867126478981758978346980284710923 / 7890127657136476966147296471389
// 0.00000000992605220868

//82211.89177008270466486756
//82211.89177008270466486756
//7360482044939286.99695278996293989874
//7360482044939286.99695278996293989874