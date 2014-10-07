global set_idtp

extern idtp

set_idtp:
   LIDT  [idtp]
   RET