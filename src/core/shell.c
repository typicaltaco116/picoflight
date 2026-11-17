// Primary Header
#include <picoflight/core/shell.h>

// Local Headers
#include <picoflight/core/cli.h>
#include <picoflight/core/config.h>
#include <picoflight/core/utilities.h>
#include <picoflight/actuators/dshotx4.h>
#include <picoflight/sensors/IMU_processing.h>

// Pico-SDK
#include <pico/stdlib.h>

// Standard Headers
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

int32_t shellExecuteGet(char **argv, int32_t argc)
{
    if (argc != 3) {
        printf("Usage: %s <int/float> <parameter>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *typeStr = argv[1];
    const char *nameStr = argv[2];

    configFetch();

    if (exactStrcmp(typeStr, "int")) {
        if (configCheckExists(nameStr))
            printf("%s = %d\n", nameStr, (int)configGetEntry(nameStr));
        else
            printf("%s error: %s not defined\n", argv[0], nameStr);
    }
    else if (exactStrcmp(typeStr, "float")) {
        if (configCheckExists(nameStr))
            printf(
                "%s = %.03f\n",
                nameStr,
                safeCastFloat(configGetEntry(nameStr))
            );
        else
            printf("%s error: %s not defined\n", argv[0], nameStr);
    }
    else {
        printf("%s error: Invalid type.\n", argv[0]);
    }

    return EXIT_FAILURE;
} 

int32_t shellExecuteSet(char **argv, int32_t argc)
{
    if (argc != 4) {
        printf("Usage: %s <int/float> <parameter> <value>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *typeStr = argv[1];
    const char *nameStr = argv[2];
    const char *valStr  = argv[3];

    uint32_t data;
    if (exactStrcmp(typeStr, "int")) {
        data = (uint32_t)atoi(valStr);
    }
    else if (exactStrcmp(typeStr, "float")) {
        data = safeCastInt(atof(valStr));
    }
    else {
        printf("%s error: Invalid type.\n", argv[0]);
        return EXIT_FAILURE;
    }

    configFetch();

    int32_t exitCode = 0;
    if (configCheckExists(nameStr)) {
        configModifyEntry(nameStr, data);
    }
    else {
        exitCode = configAddNewEntry(nameStr, data);
    }

    configCommit();

    return exitCode;

} 

int32_t shellExecuteEcho(char **argv, int32_t argc)
{
    if (argc == 1)
        return EXIT_SUCCESS;

    for (int i = 1; i < argc; ++i) {
        puts(argv[i]);
    }
    return EXIT_SUCCESS;
} 

int32_t shellExecuteReset(char **argv, int32_t argc)
{
    (void)argc; (void)argv;
    configReset();
    return EXIT_SUCCESS;
}

int32_t shellExecuteDirection(char **argv, int32_t argc)
{
    if (argc != 3) {
        printf("Usage: %s <motor idx> <reverse/normal>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int32_t motorIdx = atoi(argv[1]);

    if (motorIdx > 3 || motorIdx < 0) {
        printf("%s error: Invalid motor index.\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    dshotCommands_e cmd[] = {
        DSHOT_CMD_MOTOR_STOP,
        DSHOT_CMD_MOTOR_STOP,
        DSHOT_CMD_MOTOR_STOP,
        DSHOT_CMD_MOTOR_STOP
    };
    if (exactStrcmp(argv[2], "reverse")) {
        cmd[motorIdx] = DSHOT_CMD_SPIN_DIRECTION_REVERSED;
    }
    else if (exactStrcmp(argv[2], "normal")) {
        cmd[motorIdx] = DSHOT_CMD_SPIN_DIRECTION_NORMAL;
    }
    else {
        printf("%s error: Invalid motor direction.\n", argv[0]);
        return EXIT_FAILURE;
    }

    dshotx4SendCommands(cmd);
    sleep_us(200);
    dshotx4SendCommand(DSHOT_CMD_MOTOR_STOP);
    sleep_us(200);
    
    return EXIT_SUCCESS;
}

int32_t shellExecuteCalibrate(char **argv, int32_t argc)
{
    if (argc != 2) {
        printf("Usage: %s <cycles>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int32_t numCycles = atoi(argv[1]);

    IMUSaveCalibrationBlocking(numCycles);

    if (IMUGetIsCalibrated())
        return EXIT_SUCCESS;
    else
        return EXIT_FAILURE;
}
