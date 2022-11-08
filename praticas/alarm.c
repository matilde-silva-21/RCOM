#include "alarm.h"

// Alarm function handler
void alarmHandler(int signal)
{
    alarmEnabled = FALSE;
    alarmCount++;

    printf("\nAlarm #%d\n", alarmCount);
}

//Starts the alarm
int startAlarm()
{
    // Set alarm function handler
    (void)signal(SIGALRM, alarmHandler);

    if (alarmEnabled == FALSE)
    {
        alarm(3); // Set alarm to be triggered in 3s
        alarmEnabled = TRUE;
    }
    

    return 0;
}
