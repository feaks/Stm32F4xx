SET PATH=C:\Keil_v5\ARM\ARMCC\Bin;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;D:\Program Files\TortoiseSVN\bin;D:\Android\sdk\platform-tools;D:\Android\sdk\tools;C:\Program Files (x86)\Windows Kits\8.1\Windows Performance Toolkit\;C:\Program Files\Microsoft SQL Server\110\Tools\Binn\;C:\Program Files (x86)\Java\jdk1.7.0_79\jre\bin;C:\Program Files (x86)\Java\jdk1.7.0_79\bin;C:\Program Files\TortoiseGit\bin;C:\Program Files (x86)\Autodesk\Backburner\
SET CPU_TYPE=STM32F407VE
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=Flash
SET CPU_CLOCK=0x007A1200
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\main.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\stm32f4xx_it.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\data_transfer.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\filter.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\height_ctrl.__i"
