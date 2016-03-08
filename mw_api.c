#include "mw_api.h"

void MW_Run (int argc, char **argv, struct mw_api_spec *f) {
  f->create(argc, argv);
}
