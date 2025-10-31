
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////基本计算函数////////////////////////////////////////////////////////////////
/////基础加减乘除/////

int scale = 30; // 默认除法小数位数
// 辅助函数
int compare_numbers_abs(const char *num1, const char *num2);
char *shift_left(const char *num, int n);

// 正整数加减乘
char *add_big_abs(const char *num1, const char *num2);
char *subtract_big_abs(const char *num1, const char *num2);
char *multiply_big_abs(const char *num1, const char *num2);

// Karatsuba算法实现
char *mul_kara(const char *num1, const char *num2);

// 正小数加减乘除函数
char *add_big_decimal(const char *num1, const char *num2);
char *subtract_big_decimal(const char *num1, const char *num2);
char *multiply_big_decimal(const char *num1, const char *num2);
char *divide_big_decimal(const char *num1, const char *num2, int scale);
// 自然数加减乘除
char *complete_add(const char *num1, const char *num2);
char *complete_sub(const char *num1, const char *num2);
char *complete_mul(const char *num1, const char *num2);
char *complete_div(const char *num1, const char *num2, int scale);

void memory_error()
{
    printf("Memory allocation failed\n");
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
// 正数乘法
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
    if (compare_numbers_abs(num2, "0") == 0)
    {
        return NULL;
    }

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


int main()
{
    char *result = newton_inverse("7890127657136476966147296471389", 20, 23);
    if (result)
    {
        printf("Result: %s\n", result);
        free(result);
    }
    char *result1 = complete_mul("78317819057891910748902.17857867126478981758978346980284710923","0.0000000000000000000000000000001267406616793");
    if (result1)
    {
        printf("Result: %s\n", result1);
        free(result1);
    }
    char *result3 = complete_div("78317819057891910748902.17857867126478981758978346980284710923", "7890127657136476966147296471389", 20);
    if (result3)
    {
        printf("Result: %s\n", result3);
        free(result3);
    }
    return 0;
}

//78317819057891910748902.17857867126478981758978346980284710923 / 7890127657136476966147296471389

//617937590195283227456782666320189708558797514975705685.85514424348023553810121087543805282047
//617937590195283227456782666320189708558797514975705685.85514424348023553810121087543805282047

//0.0000000000000000000000000000001267406616793
//0.0000000000000000000000000000001267406616793


//.0000000099260522086769125208948421091300865