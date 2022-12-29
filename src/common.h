#ifndef SWAY_COMMON_H
#define SWAY_COMMON_H

void write_log(const char *type, const char *message);
void write_log(const char *type, const char *message, int err);
void setup_logging(void);

#endif // SWAY_COMMON_H