rd /Q /S project\MDK-ARM(uV5)\Flash
rd /Q /S project\MDK-ARM(uV5)\CpuRAM
rd /Q /S project\MDK-ARM(uV5)\ExtSRAM
rd /Q /S project\MDK-ARM(uV5)\DebugConfig
rd /Q /S project\MDK-ARM(uV5)\RTE
rd /Q /S project\MDK-ARM(uV5)\Exe
del /Q project\MDK-ARM(uV5)\*.bak
del /Q project\MDK-ARM(uV5)\*.dep
del /Q project\MDK-ARM(uV5)\JLink*
del /Q project\MDK-ARM(uV5)\project.uvguix.*

del /Q project\EWARMv6\Project.dep
del /Q project\EWARMv6\Flash
del /Q project\EWARMv6\CpuRAM
del /Q project\EWARMv6\settings
rd  /Q /S project\EWARMv6\Flash
rd /Q /S project\EWARMv6\CpuRAM
rd /Q /S project\EWARMv6\settings

