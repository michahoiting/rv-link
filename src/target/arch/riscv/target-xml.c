
#include <stddef.h>

#include "rvl-target.h"
#include "riscv-feature.h"


static const char target_xml_str[] =
{
        "<?xml version=\"1.0\"?>"
        "<!DOCTYPE target SYSTEM \"gdb-target.dtd\">"
        "<target version=\"1.0\">"
        "<feature name=\"org.gnu.gdb.riscv.cpu\">"
        "<reg name=\"zero\" bitsize=\"32\" regnum=\"0\" save-restore=\"yes\" type=\"int\" group=\"general\"/>"
        "<reg name=\"ra\" bitsize=\"32\" regnum=\"1\" save-restore=\"yes\" type=\"int\" group=\"general\"/>"
        "<reg name=\"sp\" bitsize=\"32\" regnum=\"2\" save-restore=\"yes\" type=\"int\" group=\"general\"/>"
        "<reg name=\"gp\" bitsize=\"32\" regnum=\"3\" save-restore=\"yes\" type=\"int\" group=\"general\"/>"
        "<reg name=\"tp\" bitsize=\"32\" regnum=\"4\" save-restore=\"yes\" type=\"int\" group=\"general\"/>"
        "<reg name=\"t0\" bitsize=\"32\" regnum=\"5\" save-restore=\"yes\" type=\"int\" group=\"general\"/>"
        "<reg name=\"t1\" bitsize=\"32\" regnum=\"6\" save-restore=\"yes\" type=\"int\" group=\"general\"/>"
        "<reg name=\"t2\" bitsize=\"32\" regnum=\"7\" save-restore=\"yes\" type=\"int\" group=\"general\"/>"
        "<reg name=\"fp\" bitsize=\"32\" regnum=\"8\" save-restore=\"yes\" type=\"int\" group=\"general\"/>"
        "<reg name=\"s1\" bitsize=\"32\" regnum=\"9\" save-restore=\"yes\" type=\"int\" group=\"general\"/>"
        "<reg name=\"a0\" bitsize=\"32\" regnum=\"10\" save-restore=\"yes\" type=\"int\" group=\"general\"/>"
        "<reg name=\"a1\" bitsize=\"32\" regnum=\"11\" save-restore=\"yes\" type=\"int\" group=\"general\"/>"
        "<reg name=\"a2\" bitsize=\"32\" regnum=\"12\" save-restore=\"yes\" type=\"int\" group=\"general\"/>"
        "<reg name=\"a3\" bitsize=\"32\" regnum=\"13\" save-restore=\"yes\" type=\"int\" group=\"general\"/>"
        "<reg name=\"a4\" bitsize=\"32\" regnum=\"14\" save-restore=\"yes\" type=\"int\" group=\"general\"/>"
        "<reg name=\"a5\" bitsize=\"32\" regnum=\"15\" save-restore=\"yes\" type=\"int\" group=\"general\"/>"
        "<reg name=\"a6\" bitsize=\"32\" regnum=\"16\" save-restore=\"yes\" type=\"int\" group=\"general\"/>"
        "<reg name=\"a7\" bitsize=\"32\" regnum=\"17\" save-restore=\"yes\" type=\"int\" group=\"general\"/>"
        "<reg name=\"s2\" bitsize=\"32\" regnum=\"18\" save-restore=\"yes\" type=\"int\" group=\"general\"/>"
        "<reg name=\"s3\" bitsize=\"32\" regnum=\"19\" save-restore=\"yes\" type=\"int\" group=\"general\"/>"
        "<reg name=\"s4\" bitsize=\"32\" regnum=\"20\" save-restore=\"yes\" type=\"int\" group=\"general\"/>"
        "<reg name=\"s5\" bitsize=\"32\" regnum=\"21\" save-restore=\"yes\" type=\"int\" group=\"general\"/>"
        "<reg name=\"s6\" bitsize=\"32\" regnum=\"22\" save-restore=\"yes\" type=\"int\" group=\"general\"/>"
        "<reg name=\"s7\" bitsize=\"32\" regnum=\"23\" save-restore=\"yes\" type=\"int\" group=\"general\"/>"
        "<reg name=\"s8\" bitsize=\"32\" regnum=\"24\" save-restore=\"yes\" type=\"int\" group=\"general\"/>"
        "<reg name=\"s9\" bitsize=\"32\" regnum=\"25\" save-restore=\"yes\" type=\"int\" group=\"general\"/>"
        "<reg name=\"s10\" bitsize=\"32\" regnum=\"26\" save-restore=\"yes\" type=\"int\" group=\"general\"/>"
        "<reg name=\"s11\" bitsize=\"32\" regnum=\"27\" save-restore=\"yes\" type=\"int\" group=\"general\"/>"
        "<reg name=\"t3\" bitsize=\"32\" regnum=\"28\" save-restore=\"yes\" type=\"int\" group=\"general\"/>"
        "<reg name=\"t4\" bitsize=\"32\" regnum=\"29\" save-restore=\"yes\" type=\"int\" group=\"general\"/>"
        "<reg name=\"t5\" bitsize=\"32\" regnum=\"30\" save-restore=\"yes\" type=\"int\" group=\"general\"/>"
        "<reg name=\"t6\" bitsize=\"32\" regnum=\"31\" save-restore=\"yes\" type=\"int\" group=\"general\"/>"
        "<reg name=\"pc\" bitsize=\"32\" regnum=\"32\" save-restore=\"yes\" type=\"int\" group=\"general\"/>"
        "</feature>"

#ifdef RVL_TARGET_RISCV_FEATURE_CSR
        "<feature name=\"org.gnu.gdb.riscv.csr\">"
        "<reg name=\"mstatus\" bitsize=\"32\" regnum=\"833\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"misa\" bitsize=\"32\" regnum=\"834\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mie\" bitsize=\"32\" regnum=\"837\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mtvec\" bitsize=\"32\" regnum=\"838\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mcounteren\" bitsize=\"32\" regnum=\"839\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"

#ifdef RVL_TARGET_RISCV_FEATURE_CSR_HPM
        "<reg name=\"mhpmevent3\" bitsize=\"32\" regnum=\"868\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmevent4\" bitsize=\"32\" regnum=\"869\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmevent5\" bitsize=\"32\" regnum=\"870\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmevent6\" bitsize=\"32\" regnum=\"871\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmevent7\" bitsize=\"32\" regnum=\"872\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmevent8\" bitsize=\"32\" regnum=\"873\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmevent9\" bitsize=\"32\" regnum=\"874\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmevent10\" bitsize=\"32\" regnum=\"875\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmevent11\" bitsize=\"32\" regnum=\"876\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmevent12\" bitsize=\"32\" regnum=\"877\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmevent13\" bitsize=\"32\" regnum=\"878\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmevent14\" bitsize=\"32\" regnum=\"879\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmevent15\" bitsize=\"32\" regnum=\"880\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmevent16\" bitsize=\"32\" regnum=\"881\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmevent17\" bitsize=\"32\" regnum=\"882\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmevent18\" bitsize=\"32\" regnum=\"883\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmevent19\" bitsize=\"32\" regnum=\"884\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmevent20\" bitsize=\"32\" regnum=\"885\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmevent21\" bitsize=\"32\" regnum=\"886\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmevent22\" bitsize=\"32\" regnum=\"887\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmevent23\" bitsize=\"32\" regnum=\"888\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmevent24\" bitsize=\"32\" regnum=\"889\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmevent25\" bitsize=\"32\" regnum=\"890\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmevent26\" bitsize=\"32\" regnum=\"891\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmevent27\" bitsize=\"32\" regnum=\"892\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmevent28\" bitsize=\"32\" regnum=\"893\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmevent29\" bitsize=\"32\" regnum=\"894\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmevent30\" bitsize=\"32\" regnum=\"895\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmevent31\" bitsize=\"32\" regnum=\"896\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
#endif // RVL_TARGET_RISCV_FEATURE_CSR_HPM

        "<reg name=\"mscratch\" bitsize=\"32\" regnum=\"897\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mepc\" bitsize=\"32\" regnum=\"898\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mcause\" bitsize=\"32\" regnum=\"899\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mtval\" bitsize=\"32\" regnum=\"900\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mip\" bitsize=\"32\" regnum=\"901\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"

#ifdef RVL_TARGET_RISCV_FEATURE_CSR_PMP
        "<reg name=\"pmpcfg0\" bitsize=\"32\" regnum=\"993\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"pmpcfg1\" bitsize=\"32\" regnum=\"994\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"pmpcfg2\" bitsize=\"32\" regnum=\"995\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"pmpcfg3\" bitsize=\"32\" regnum=\"996\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"pmpaddr0\" bitsize=\"32\" regnum=\"1009\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"pmpaddr1\" bitsize=\"32\" regnum=\"1010\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"pmpaddr2\" bitsize=\"32\" regnum=\"1011\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"pmpaddr3\" bitsize=\"32\" regnum=\"1012\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"pmpaddr4\" bitsize=\"32\" regnum=\"1013\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"pmpaddr5\" bitsize=\"32\" regnum=\"1014\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"pmpaddr6\" bitsize=\"32\" regnum=\"1015\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"pmpaddr7\" bitsize=\"32\" regnum=\"1016\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"pmpaddr8\" bitsize=\"32\" regnum=\"1017\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"pmpaddr9\" bitsize=\"32\" regnum=\"1018\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"pmpaddr10\" bitsize=\"32\" regnum=\"1019\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"pmpaddr11\" bitsize=\"32\" regnum=\"1020\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"pmpaddr12\" bitsize=\"32\" regnum=\"1021\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"pmpaddr13\" bitsize=\"32\" regnum=\"1022\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"pmpaddr14\" bitsize=\"32\" regnum=\"1023\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"pmpaddr15\" bitsize=\"32\" regnum=\"1024\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
#endif // RVL_TARGET_RISCV_FEATURE_CSR_PMP

#ifdef RVL_TARGET_RISCV_FEATURE_CSR_DMODE
        "<reg name=\"tselect\" bitsize=\"32\" regnum=\"2017\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"tdata1\" bitsize=\"32\" regnum=\"2018\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"tdata2\" bitsize=\"32\" regnum=\"2019\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"tdata3\" bitsize=\"32\" regnum=\"2020\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"dcsr\" bitsize=\"32\" regnum=\"2033\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"dpc\" bitsize=\"32\" regnum=\"2034\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"dscratch\" bitsize=\"32\" regnum=\"2035\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
#endif // RVL_TARGET_RISCV_FEATURE_CSR_DMODE

        "<reg name=\"mcycle\" bitsize=\"32\" regnum=\"2881\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"minstret\" bitsize=\"32\" regnum=\"2883\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"

#ifdef RVL_TARGET_RISCV_FEATURE_CSR_HPM
        "<reg name=\"mhpmcounter3\" bitsize=\"32\" regnum=\"2884\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter4\" bitsize=\"32\" regnum=\"2885\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter5\" bitsize=\"32\" regnum=\"2886\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter6\" bitsize=\"32\" regnum=\"2887\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter7\" bitsize=\"32\" regnum=\"2888\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter8\" bitsize=\"32\" regnum=\"2889\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter9\" bitsize=\"32\" regnum=\"2890\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter10\" bitsize=\"32\" regnum=\"2891\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter11\" bitsize=\"32\" regnum=\"2892\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter12\" bitsize=\"32\" regnum=\"2893\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter13\" bitsize=\"32\" regnum=\"2894\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter14\" bitsize=\"32\" regnum=\"2895\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter15\" bitsize=\"32\" regnum=\"2896\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter16\" bitsize=\"32\" regnum=\"2897\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter17\" bitsize=\"32\" regnum=\"2898\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter18\" bitsize=\"32\" regnum=\"2899\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter19\" bitsize=\"32\" regnum=\"2900\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter20\" bitsize=\"32\" regnum=\"2901\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter21\" bitsize=\"32\" regnum=\"2902\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter22\" bitsize=\"32\" regnum=\"2903\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter23\" bitsize=\"32\" regnum=\"2904\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter24\" bitsize=\"32\" regnum=\"2905\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter25\" bitsize=\"32\" regnum=\"2906\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter26\" bitsize=\"32\" regnum=\"2907\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter27\" bitsize=\"32\" regnum=\"2908\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter28\" bitsize=\"32\" regnum=\"2909\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter29\" bitsize=\"32\" regnum=\"2910\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter30\" bitsize=\"32\" regnum=\"2911\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter31\" bitsize=\"32\" regnum=\"2912\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
#endif // RVL_TARGET_RISCV_FEATURE_CSR_HPM

        "<reg name=\"mcycleh\" bitsize=\"32\" regnum=\"3009\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"minstreth\" bitsize=\"32\" regnum=\"3011\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"

#ifdef RVL_TARGET_RISCV_FEATURE_CSR_HPM
        "<reg name=\"mhpmcounter3h\" bitsize=\"32\" regnum=\"3012\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter4h\" bitsize=\"32\" regnum=\"3013\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter5h\" bitsize=\"32\" regnum=\"3014\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter6h\" bitsize=\"32\" regnum=\"3015\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter7h\" bitsize=\"32\" regnum=\"3016\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter8h\" bitsize=\"32\" regnum=\"3017\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter9h\" bitsize=\"32\" regnum=\"3018\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter10h\" bitsize=\"32\" regnum=\"3019\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter11h\" bitsize=\"32\" regnum=\"3020\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter12h\" bitsize=\"32\" regnum=\"3021\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter13h\" bitsize=\"32\" regnum=\"3022\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter14h\" bitsize=\"32\" regnum=\"3023\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter15h\" bitsize=\"32\" regnum=\"3024\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter16h\" bitsize=\"32\" regnum=\"3025\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter17h\" bitsize=\"32\" regnum=\"3026\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter18h\" bitsize=\"32\" regnum=\"3027\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter19h\" bitsize=\"32\" regnum=\"3028\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter20h\" bitsize=\"32\" regnum=\"3029\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter21h\" bitsize=\"32\" regnum=\"3030\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter22h\" bitsize=\"32\" regnum=\"3031\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter23h\" bitsize=\"32\" regnum=\"3032\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter24h\" bitsize=\"32\" regnum=\"3033\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter25h\" bitsize=\"32\" regnum=\"3034\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter26h\" bitsize=\"32\" regnum=\"3035\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter27h\" bitsize=\"32\" regnum=\"3036\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter28h\" bitsize=\"32\" regnum=\"3037\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter29h\" bitsize=\"32\" regnum=\"3038\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter30h\" bitsize=\"32\" regnum=\"3039\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhpmcounter31h\" bitsize=\"32\" regnum=\"3040\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
#endif // RVL_TARGET_RISCV_FEATURE_CSR_HPM

#ifdef RVL_TARGET_RISCV_FEATURE_CSR_UMODE

        "<reg name=\"cycle\" bitsize=\"32\" regnum=\"3137\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"time\" bitsize=\"32\" regnum=\"3138\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"instret\" bitsize=\"32\" regnum=\"3139\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"

#ifdef RVL_TARGET_RISCV_FEATURE_CSR_HPM
        "<reg name=\"hpmcounter3\" bitsize=\"32\" regnum=\"3140\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter4\" bitsize=\"32\" regnum=\"3141\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter5\" bitsize=\"32\" regnum=\"3142\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter6\" bitsize=\"32\" regnum=\"3143\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter7\" bitsize=\"32\" regnum=\"3144\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter8\" bitsize=\"32\" regnum=\"3145\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter9\" bitsize=\"32\" regnum=\"3146\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter10\" bitsize=\"32\" regnum=\"3147\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter11\" bitsize=\"32\" regnum=\"3148\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter12\" bitsize=\"32\" regnum=\"3149\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter13\" bitsize=\"32\" regnum=\"3150\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter14\" bitsize=\"32\" regnum=\"3151\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter15\" bitsize=\"32\" regnum=\"3152\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter16\" bitsize=\"32\" regnum=\"3153\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter17\" bitsize=\"32\" regnum=\"3154\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter18\" bitsize=\"32\" regnum=\"3155\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter19\" bitsize=\"32\" regnum=\"3156\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter20\" bitsize=\"32\" regnum=\"3157\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter21\" bitsize=\"32\" regnum=\"3158\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter22\" bitsize=\"32\" regnum=\"3159\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter23\" bitsize=\"32\" regnum=\"3160\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter24\" bitsize=\"32\" regnum=\"3161\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter25\" bitsize=\"32\" regnum=\"3162\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter26\" bitsize=\"32\" regnum=\"3163\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter27\" bitsize=\"32\" regnum=\"3164\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter28\" bitsize=\"32\" regnum=\"3165\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter29\" bitsize=\"32\" regnum=\"3166\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter30\" bitsize=\"32\" regnum=\"3167\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter31\" bitsize=\"32\" regnum=\"3168\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
#endif // RVL_TARGET_RISCV_FEATURE_CSR_HPM

        "<reg name=\"cycleh\" bitsize=\"32\" regnum=\"3265\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"timeh\" bitsize=\"32\" regnum=\"3266\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"instreth\" bitsize=\"32\" regnum=\"3267\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"

#ifdef RVL_TARGET_RISCV_FEATURE_CSR_HPM
        "<reg name=\"hpmcounter3h\" bitsize=\"32\" regnum=\"3268\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter4h\" bitsize=\"32\" regnum=\"3269\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter5h\" bitsize=\"32\" regnum=\"3270\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter6h\" bitsize=\"32\" regnum=\"3271\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter7h\" bitsize=\"32\" regnum=\"3272\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter8h\" bitsize=\"32\" regnum=\"3273\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter9h\" bitsize=\"32\" regnum=\"3274\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter10h\" bitsize=\"32\" regnum=\"3275\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter11h\" bitsize=\"32\" regnum=\"3276\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter12h\" bitsize=\"32\" regnum=\"3277\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter13h\" bitsize=\"32\" regnum=\"3278\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter14h\" bitsize=\"32\" regnum=\"3279\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter15h\" bitsize=\"32\" regnum=\"3280\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter16h\" bitsize=\"32\" regnum=\"3281\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter17h\" bitsize=\"32\" regnum=\"3282\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter18h\" bitsize=\"32\" regnum=\"3283\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter19h\" bitsize=\"32\" regnum=\"3284\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter20h\" bitsize=\"32\" regnum=\"3285\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter21h\" bitsize=\"32\" regnum=\"3286\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter22h\" bitsize=\"32\" regnum=\"3287\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter23h\" bitsize=\"32\" regnum=\"3288\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter24h\" bitsize=\"32\" regnum=\"3289\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter25h\" bitsize=\"32\" regnum=\"3290\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter26h\" bitsize=\"32\" regnum=\"3291\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter27h\" bitsize=\"32\" regnum=\"3292\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter28h\" bitsize=\"32\" regnum=\"3293\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter29h\" bitsize=\"32\" regnum=\"3294\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter30h\" bitsize=\"32\" regnum=\"3295\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"hpmcounter31h\" bitsize=\"32\" regnum=\"3296\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
#endif // RVL_TARGET_RISCV_FEATURE_CSR_HPM
#endif // RVL_TARGET_RISCV_FEATURE_CSR_UMODE

        "<reg name=\"mvendorid\" bitsize=\"32\" regnum=\"3922\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"marchid\" bitsize=\"32\" regnum=\"3923\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mimpid\" bitsize=\"32\" regnum=\"3924\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"
        "<reg name=\"mhartid\" bitsize=\"32\" regnum=\"3925\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"

#if 0 // GDB not support custom CSR yet
#ifdef RVL_TARGET_RISCV_FEATURE_CSR_CUSTOM
        RVL_TARGET_RISCV_FEATURE_CSR_CUSTOM
#endif // RVL_TARGET_RISCV_FEATURE_CSR_CUSTOM
#endif // 0

        "</feature>"
#endif // RVL_TARGET_RISCV_FEATURE_CSR

#ifdef RVL_TARGET_RISCV_FEATURE_VIRTUAL
        "<feature name=\"org.gnu.gdb.riscv.virtual\">"
        "<reg name=\"priv\" bitsize=\"8\" regnum=\"4161\" save-restore=\"no\" type=\"int\" group=\"general\"/>"
        "</feature>"
#endif // RVL_TARGET_RISCV_FEATURE_VIRTUAL

        "</target>"
};

const char *rvl_target_get_target_xml(void)
{
    return target_xml_str;
}

size_t rvl_target_get_target_xml_len(void)
{
    return sizeof(target_xml_str) - 1;
}
