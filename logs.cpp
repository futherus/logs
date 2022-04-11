#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <errno.h>

#include "logs.h"

static const size_t TXT_TIME_CAP      = 512;
static const size_t LOGS_FILENAME_MAX = 127;
static const size_t LOGS_MIN_CAP      = 1;
static const size_t LOGS_CAP_MULTPLR  = 2;

static const char LOG_INTRO[] =
R"(
<html>
    <head>
        <title>
            Log
        </title>
        <style>
            .ok {color: springgreen;font-weight: bold;}
            .error{color: red;font-weight: bold; font-size: 30}
            .log{color: #C5D0E6; font-size: 14}
            .msg{color: #E59E1F;text-align: center;font-weight: bold; font-size: 20}
            .title{color: #E59E1F;text-align: center; font-size: 17}
        </style>
    </head>
    <body bgcolor = "#2F353B">
        <pre class = "log">
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Activated this stream
)";

static const char LOG_OUTRO[] = 
R"(
Disactivated this stream
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        </pre>
    </body>
</html>
)";

struct Logstream
{
    FILE* stream = nullptr;
    char filename[LOGS_FILENAME_MAX + 1] = {};
};

static struct Logs
{
    Logstream* arr  = nullptr;
    size_t     size = 0;
    size_t     cap  = 0;
} LOGS;

// resizes array of Logstreams in LOGS structure
// note: allocated memory won't be free()'d manually as it can happen only at
//       program termination
static int logs_resize(size_t new_cap)
{
    Logstream* tmp_arr = (Logstream*) calloc(new_cap, sizeof(Logstream));

    if(tmp_arr == nullptr)
    {
        fprintf(stderr, "logs: bad allocation\n");
        return 1;
    }

    LOGS.arr = tmp_arr;
    LOGS.cap = new_cap;

    return 0;
}

// closes all opened streams
static void logs_close_all()
{
    for(size_t iter = 0; iter < LOGS.size; iter++)
    {
        if(LOGS.arr[iter].stream)
        {
            fprintf(LOGS.arr[iter].stream, "%s", LOG_OUTRO);

            if(fclose(LOGS.arr[iter].stream) != 0)
                fprintf(stderr, "logs: `%s` closed unsuccessfully\n", LOGS.arr[iter].filename);
        }
    }

    // function does not free() memory initialized in LOGS because it's called
    // at program termination and free() only decreases perfomance

    return;
}

// initializes LOGS structure and default Logstream
FILE* logs_init(const char* filename)
{
    assert(filename);

    if(logs_resize(LOGS_MIN_CAP))
        return nullptr;

    atexit(&logs_close_all);

    return logs_open(filename); // open default stream
}

// opens file as log file
FILE* logs_open(const char* filename)
{
    assert(filename);

    if(LOGS.arr == nullptr)
    {
        fprintf(stderr, "logs: logs are not initialised\n");
        return nullptr;
    }
    
    if(LOGS.size == LOGS.cap)
        logs_resize(LOGS.cap * LOGS_CAP_MULTPLR);

    FILE* stream = fopen(filename, "w");
            
    if(stream == nullptr)
    {
        fprintf(stderr, "logs: cannot open `%s`: %s\n", filename, strerror(errno));
        return nullptr;
    }

    int buf_err = setvbuf(stream, nullptr, _IONBF, 0);
    if(buf_err)
    {
        fprintf(stderr, "logs: cannot disable buffering `%s`\n", filename);
    }

    char txt_time[TXT_TIME_CAP] = "";

    time_t t_time = time(nullptr);
    strftime(txt_time, TXT_TIME_CAP, "%H:%M:%S %d.%m.%Y", localtime(&t_time));
    fprintf(stream, "%s (%s)\n\n", LOG_INTRO, txt_time);

    LOGS.arr[LOGS.size].stream = stream;
    strncpy(LOGS.arr[LOGS.size].filename, filename, LOGS_FILENAME_MAX);
    LOGS.size++;

    return stream;
}

// finds stream with name = filename
FILE* logs_get(const char* filename)
{
    if(LOGS.arr == nullptr)
    {
        fprintf(stderr, "logs: logs are not initialized\n");
        return nullptr;
    }
    
    if(filename == nullptr)
        return LOGS.arr[0].stream;
    
    for(size_t iter = 0; iter < LOGS.size; iter++)
    {
        if(strcmp(LOGS.arr[iter].filename, filename) == 0)
            return LOGS.arr[iter].stream;
    }

    fprintf(stderr, "logs: `%s` is not opened\n", filename);

    return nullptr;
}
