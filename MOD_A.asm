MOD_A:  BEGIN
EXTERN: Y
EXTERN: MOD_B
PUBLIC  VAL
PUBLIC  L1
        SECTION TEXT
        INPUT   Y
        LOAD    VAL
        ADD     Y
        STORE   Y + 2
        JMPP    MOD_B
L1:     STOP
        SECTION DATA
VAL:    CONST 5
        END