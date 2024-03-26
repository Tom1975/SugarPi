
HVS for Raspberry PI4
---------------------

- Register address :
  - BCM_PERIPH_BASE_VIRT     = **0xFE000000**
  - Scaler adress : SCALER_BASE (BCM_PERIPH_BASE_VIRT + 0x400000) => **0xFE400000**
  - Scaler mlist memory : SCALER_LIST_MEMORY  (SCALER_BASE + 0x4000) => **0xFE404000**

  - SCALER5_CTL0_UNITY : 1U << 15 = **0x10000**
