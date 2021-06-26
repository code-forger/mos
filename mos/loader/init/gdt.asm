global set_gdtp

extern gdtp

set_gdtp:
    LGDT  [gdtp]
    JMP 0x08:set_gdtp_end
set_gdtp_end:
    MOV ax, 0x10
    MOV ds, ax
    MOV es, ax
    MOV fs, ax
    MOV gs, ax
    MOV ss, ax
    ret