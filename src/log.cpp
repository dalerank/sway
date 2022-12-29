#include <stdio.h>
#include <memory>

static FILE *log_file = nullptr;

void write_log(const char *type, const char *message, int err) {
    if (log_file) {
        fwrite(type, sizeof(char), strlen(type), log_file);
        if (message)
            fwrite(message, sizeof(char), strlen(message), log_file);
        else {
            char buf[128];
            const char *errt = itoa(err, buf, 10);
            fwrite(buf, sizeof(char), strlen(buf), log_file);
        }
        fwrite("\n", sizeof(char), 1, log_file);
        fflush(log_file);
    }
}

void write_log(const char *type, const char *message) {
    write_log(type, message, 0);
}

void setup_logging(void) {
    log_file = fopen("sway-log.txt", "w");
}