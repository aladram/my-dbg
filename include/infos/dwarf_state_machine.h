#ifndef DWARF_STATE_MACHINE_H
# define DWARF_STATE_MACHINE_H

# include <stdint.h>

# include "my_dwarf.h"

struct my_dw_sm **run_machine(struct my_dw_lconf *lconf,
                              struct my_dw_sm *sm);

#endif /* DWARF_STATE_MACHINE_H */
