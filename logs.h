#ifndef DUMP_SYSTEM_H
#define DUMP_SYSTEM_H

#include <stdio.h>
#include <string.h>

#define VERBOSE

#define ASSERT$(CONDITION__, ERROR__, ACTION__)                                       \
    do                                                                                \
    {                                                                                 \
        if(!(CONDITION__))                                                            \
        {                                                                             \
            fprintf(stderr, "\x1b[31;1mERROR:\x1b[0m %s\n", #ERROR__);                \
                                                                                      \
            FILE* stream__ = logs_get();                                              \
            if(stream__ != nullptr)                                                   \
            {                                                                         \
                fprintf(stream__, "<span class = \"error\">ERROR: %s\n</span>"        \
                                  "\t\t\t\tat %s : %d : %s\n",                        \
                         #ERROR__, __FILE__, __LINE__, __PRETTY_FUNCTION__);          \
            }                                                                         \
                                                                                      \
            {ACTION__}                                                                \
        }                                                                             \
    } while(0)                                                                        \

#define ASSERT_RET$(CONDITION__, ERROR__)             \
    ASSERT(CONDITION__, ERROR__, return ERROR__; )   \

#define PASS$(CONDITION__, ACTION__)                                                  \
    do                                                                                \
    {                                                                                 \
        if(!(CONDITION__))                                                            \
        {                                                                             \
            FILE* stream__ = logs_get();                                              \
            if(stream__ != nullptr)                                                   \
            {                                                                         \
                fprintf(stream__, "\t\t\t\tat %s : %d : %s\n",                        \
                        __FILE__, __LINE__, __PRETTY_FUNCTION__);                     \
            }                                                                         \
                                                                                      \
            {ACTION__}                                                                \
        }                                                                             \
    } while(0)                                                                        \

#ifdef VERBOSE
#define LOG$(MESSAGE__, ...)                                                          \
    {                                                                                 \
        FILE* stream__ = logs_get();                                                  \
        if(stream__ != nullptr)                                                       \
        {                                                                             \
            fprintf(stream__, "<span class = \"title\">" MESSAGE__ "\n</span>"        \
                              "\t\t\t\tat %s : %d : %s\n",                            \
                    ##__VA_ARGS__, __FILE__, __LINE__, __PRETTY_FUNCTION__);          \
        }                                                                             \
    }                                                                                 \

#define MSG$(MESSAGE__, ...)                                                          \
    {                                                                                 \
        FILE* stream__ = logs_get();                                                  \
        if(stream__ != nullptr)                                                       \
        {                                                                             \
            fprintf(stream__, "<span class = \"msg\">" MESSAGE__ " \n</span>",        \
                    ##__VA_ARGS__);                                                   \
        }                                                                             \
    }                                                                                 \

#else
#define LOG$(MESSAGE__, ...) {void(0);}
#define MSG$(MESSAGE__, ...) {void(0);}
#endif

FILE* logs_init(const char* filename);
FILE* logs_open(const char* filename);
FILE* logs_get (const char* filename = nullptr);

#endif // DUMP_SYSTEM_H
