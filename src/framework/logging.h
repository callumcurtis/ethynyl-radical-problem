/*
 * logging.h
 *
 * For: UVic CSC 230, Summer 2022.
 *
 * Copyright: Mike Zastre 2022
 *
 * DO NOT MODIFY THIS SOURCE-CODE FILE WITHOUT EXPRESS WRITTEN
 * PERMISSION FROM A MEMBER OF THE TEACHING TEAM.
 */
#ifndef _LOGGING_H_
#define _LOGGING_H_

#define LOG_MAX_LINES 100
#define LOG_MAX_LINE_LEN 80

extern char kosmos_log[LOG_MAX_LINES][LOG_MAX_LINE_LEN];
extern unsigned char kosmos_log_numlines;

void kosmos_log_init(void);
void kosmos_log_dump(void);
void kosmos_log_add_entry(int, int, int, int, char *);
void kosmos_log_add_line(char *);

#endif
