#ifndef __RVL_RISCV_FEATURE_H__
#define __RVL_RISCV_FEATURE_H__

#define RVL_TARGET_RISCV_FEATURE_CSR
#define RVL_TARGET_RISCV_FEATURE_CSR_UMODE
#define RVL_TARGET_RISCV_FEATURE_CSR_CUSTOM \
        "<reg name=\"pushmcause\" bitsize=\"32\" regnum=\"2030\" save-restore=\"no\" type=\"int\" group=\"csr\"/>" \
        "<reg name=\"pushmepc\" bitsize=\"32\" regnum=\"2031\" save-restore=\"no\" type=\"int\" group=\"csr\"/>" \
        "<reg name=\"sleepvalue\" bitsize=\"32\" regnum=\"2065\" save-restore=\"no\" type=\"int\" group=\"csr\"/>" \
        "<reg name=\"txevt\" bitsize=\"32\" regnum=\"2066\" save-restore=\"no\" type=\"int\" group=\"csr\"/>" \
        "<reg name=\"wfe\" bitsize=\"32\" regnum=\"2064\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"

#endif // __RVL_RISCV_FEATURE_H__
