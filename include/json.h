#ifndef JSON_H
#define JSON_H

#include "spec.h"

int read_spec_from_json(char *path, int *spec_field_count, field **spec_fields);

#endif /* JSON_H */
