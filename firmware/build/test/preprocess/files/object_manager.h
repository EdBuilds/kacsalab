#include "errors/errors.h"






ERRORS_return_t OBJECT_MANAGER_init(void);

const char * OBJECT_MANAGER_get_name(uint8_t obj_idx);

ERRORS_return_t OBJECT_MANAGER_print_state(uint8_t obj_idx, char *buffer, size_t buffer_len, size_t *used_buffer_space);
