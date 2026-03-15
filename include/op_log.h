#ifndef OP_LOG_H
#define OP_LOG_H

void oplog_append_success(const char *operation, const char *detail, double result);
void oplog_append_error(const char *operation, const char *detail, const char *error_msg);

#endif