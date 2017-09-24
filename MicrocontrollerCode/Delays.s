.equ __P24FJ64GA002,1
    
.include "p24Fxxxx.inc"
#include "xc.inc"

.text
.global _wait_1us, _wait_10us, _wait_50us, _wait_1_ms
 
_wait_1us:
    REPEAT  #9   ;Formula: microseconds/0.0625 - 7
    NOP
    RETURN
    
_wait_10us:
    REPEAT  #153   ;Formula: microseconds/0.0625 - 7
    NOP
    RETURN
    
_wait_50us:
    REPEAT  #793   ;Formula: microseconds/0.0625 - 7
    NOP
    RETURN
    
_wait_1_ms:
    REPEAT  #15993  ;Formula: microseconds/0.0625 - 7
    NOP
    RETURN
    


