MOD_B:  BEGIN
EXTERN: VAL
EXTERN: L1
PUBLIC  Y
PUBLIC  MOD_B
        SECTION TEXT
        OUTPUT  Y
        OUTPUT  VAL
        OUTPUT  Y + 2
        JMP     L1
        SECTION DATA
Y:      SPACE 3
        END