/*
 * ACBFHapticsBridge v7-r7-kick-walk4x
 * Reconstructed exact injected kick-gate stub.
 *
 * Runtime VA: 0x180017388
 * Length: 28 bytes
 * SHA-256 of verified R7 ASI:
 * fb77b55728185e453fcc2046716f1d299bb78e92aededae906c9c56db58383b9
 *
 * The .byte form below is intentionally used so assembling this source emits
 * the exact machine-code sequence present in the verified R7 binary.
 */

    .section .text
    .global r7_kick_stub
r7_kick_stub:
    /* push rax */
    .byte 0x50

    /* movzx eax, byte ptr [rip+0x21f4]  ; -> VA 0x180019584 TriggerThreshold */
    .byte 0x0f,0xb6,0x05,0xf4,0x21,0x00,0x00

    /* cmp byte ptr [rsp+0x13f], al
       Note: rsp was decremented by push rax, so this corresponds to the
       worker's original bRightTrigger byte at rsp+0x137. */
    .byte 0x38,0x84,0x24,0x3f,0x01,0x00,0x00

    /* pop rax */
    .byte 0x58

    /* jb 0x18001739f  ; trigger below threshold -> original fallthrough */
    .byte 0x72,0x05

    /* jmp 0x1800164af ; existing raw-X scheduler */
    .byte 0xe9,0x10,0xf1,0xff,0xff

    /* jmp 0x1800148dd ; original Square/X fallthrough */
    .byte 0xe9,0x39,0xd5,0xff,0xff
