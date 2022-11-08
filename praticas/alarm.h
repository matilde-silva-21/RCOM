#include <unistd.h>
#include <signal.h>
#include <stdio.h>

#define FALSE 0
#define TRUE 1

int alarmEnabled = FALSE;
int alarmCount = 0;

void alarmHandler(int signal);

int startAlarm();

