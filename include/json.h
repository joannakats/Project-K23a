#ifndef JSON_H
#define JSON_H

#include "spec.h"
#include "vocabulary.h"

int read_spec_from_json(char *path, hashtable **spec_fields, bow *vocabulary);

#endif /* JSON_H */
