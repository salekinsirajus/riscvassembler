main:
    addi t0, zero, 0x5A          # Create a test pattern in register t0
    
    sw t0, -4(sp)        # Test STORE: Save t0 onto the stack memory
    lw t1, -4(sp)        # Test LOAD: Read it back from stack into t1
